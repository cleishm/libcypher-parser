/* vi:set ts=4 sw=4 expandtab:
 *
 * Copyright 2016, Chris Leishman (http://github.com/cleishm)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../../config.h"
#include "cypher-parser.h"
#include "util.h"
#include <assert.h>
#include <errno.h>
#include <setjmp.h>

typedef struct _yycontext yycontext;
typedef int (*yyrule)(yycontext *yy);
typedef int (*source_cb_t)(void *data, char *buf, int n);

static int parse(source_cb_t source, void *sourcedata,
        cypher_parser_quick_segment_callback_t callback, void *userdata,
        uint_fast32_t flags);
static void source(yycontext *yy, char *buf, int *result, int max_size);


struct source_from_buffer_data
{
    const char *buffer;
    size_t length;
};


static int source_from_buffer(void *data, char *buf, int n)
{
    struct source_from_buffer_data *input = data;
    int len = min(input->length, n);
    input->length -= len;
    if (len == 0)
    {
        return len;
    }
    memcpy(buf, input->buffer, len);
    return len;
}


static int source_from_stream(void *data, char *buf, int n)
{
    FILE *stream = data;
    int c = getc(stream);
    if (c == EOF)
    {
        return 0;
    }
    *buf = c;
    return 1;
}


int cypher_quick_uparse(const char *s, size_t n,
        cypher_parser_quick_segment_callback_t callback, void *userdata,
        uint_fast32_t flags)
{
    struct source_from_buffer_data sourcedata = { .buffer = s, .length = n };
    return parse(source_from_buffer, &sourcedata, callback, userdata, flags);
}


int cypher_quick_fparse(FILE *stream,
        cypher_parser_quick_segment_callback_t callback, void *userdata,
        uint_fast32_t flags)
{
    return parse(source_from_stream, stream, callback, userdata, flags);
}


#define YY_CTX_LOCAL
#define YY_PARSE(T) static T
#define YY_CTX_MEMBERS \
    source_cb_t source; \
    void *source_data; \
    cypher_parser_quick_segment_callback_t callback; \
    void *callback_data; \
    sigjmp_buf abort_env; \
    bool eof; \
    int result;

#define YY_MALLOC abort_malloc
#define YY_REALLOC abort_realloc

#define YY_INPUT(yy, buf, result, max_size) \
    source(yy, buf, &result, max_size)

static void *abort_malloc(yycontext *yy, size_t size);
static void *abort_realloc(yycontext *yy, void *ptr, size_t size);
static inline void source(yycontext *yy, char *buf, int *result, int max_size);
static inline void segment(yycontext *yy);

#pragma GCC diagnostic ignored "-Wunused-function"
#include "quick_parser_leg.c"


#define abort_parse(yy) \
    do { assert(errno != 0); siglongjmp(yy->abort_env, errno); } while (0)
static int safe_yyparsefrom(yycontext *yy, yyrule rule);


void source(yycontext *yy, char *buf, int *result, int max_size)
{
    if (buf == NULL)
    {
        *result = 0;
        return;
    }
    assert(yy != NULL && yy->source != NULL);
    *result = yy->source(yy->source_data, buf, max_size);
}


int parse(source_cb_t source, void *sourcedata,
        cypher_parser_quick_segment_callback_t callback, void *userdata,
        uint_fast32_t flags)
{
    yycontext yy;
    memset(&yy, 0, sizeof(yycontext));

    yy.source = source;
    yy.source_data = sourcedata;
    yy.callback = callback;
    yy.callback_data = userdata;
    int err = -1;

    yyrule rule = (flags & CYPHER_PARSE_ONLY_STATEMENTS)?
            yy_statement : yy_directive;

    for (;;)
    {
        int result = safe_yyparsefrom(&yy, rule);
        if (result <= 0)
        {
            goto cleanup;
        }

        if (yy.result)
        {
            err = yy.result;
            goto cleanup;
        }

        if (yy.eof)
        {
            break;
        }
    }

    err = 0;

cleanup:
    yyrelease(&yy);
    return err;
}


int safe_yyparsefrom(yycontext *yy, yyrule rule)
{
    int err;
    if ((err = sigsetjmp(yy->abort_env, 0)) != 0)
    {
        errno = err;
        return -1;
    }

    int result = yyparsefrom(yy, rule);
    memset(yy->abort_env, 0, sizeof(sigjmp_buf));
    return result;
}


void *abort_malloc(yycontext *yy, size_t size)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        abort_parse(yy);
    }
    return m;
}


void *abort_realloc(yycontext *yy, void *ptr, size_t size)
{
    void *m = realloc(ptr, size);
    if (m == NULL)
    {
        abort_parse(yy);
    }
    return m;
}


void segment(yycontext *yy)
{
    if (yy->__end == 0)
    {
        assert(yy->eof);
        yy->result = 0;
        return;
    }
    yy->result = yy->callback(yy->callback_data, yy->__buf + yy->__begin,
            yy->__end - yy->__begin, yy->eof);
}
