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
#include "../config.h"
#include "../src/lib/cypher-parser.h"
#include <check.h>
#include <errno.h>
#include <unistd.h>


#define MAX_SEGMENTS 8

static FILE *in;
static FILE *out;
static unsigned int nsegments;
static char *segments[MAX_SEGMENTS];
static bool eofs[MAX_SEGMENTS];


static void setup(void)
{
    int fds[2];
    ck_assert(pipe(fds) == 0);
    in = fdopen(fds[0], "r");
    ck_assert_ptr_ne(in, NULL);
    out = fdopen(fds[1], "w");
    ck_assert_ptr_ne(out, NULL);

    nsegments = 0;
    memset(segments, 0, sizeof(segments));
    memset(eofs, 0, sizeof(eofs));
}


static void teardown(void)
{
    if (out != NULL)
    {
        fclose(out);
    }
    fclose(in);

    for (unsigned int i = 0; i < nsegments; ++i)
    {
        free(segments[i]);
    }
}


static void fill_stream(const char *content)
{
    fputs(content, out);
    fclose(out);
    out = NULL;
}


static int segment_callback(void *data, const char *s, size_t n, bool eof)
{
    ck_assert(nsegments < MAX_SEGMENTS);
    segments[nsegments] = strndup(s, n);
    ck_assert(segments[nsegments] != NULL);
    eofs[nsegments] = eof;
    ++nsegments;
    return 0;
}


START_TEST (fparse_empty)
{
    fill_stream("");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 0);
}
END_TEST


START_TEST (fparse_single)
{
    fill_stream("return 1;");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 1);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
}
END_TEST


START_TEST (fparse_multiple)
{
    fill_stream("return 1; return 2;\n   return 3    ;");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 3);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], " return 2;");
    ck_assert(!eofs[1]);
    ck_assert_str_eq(segments[2], "\n   return 3    ;");
    ck_assert(!eofs[2]);
}
END_TEST


START_TEST (fparse_commands)
{
    fill_stream(":foo bar\"baz\"\n");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 1);

    ck_assert_str_eq(segments[0], ":foo bar\"baz\"\n");
    ck_assert(!eofs[0]);
}
END_TEST


START_TEST (fparse_statements_only)
{
    fill_stream("return 1; :foo bar\"baz\"\n return 2;");

    int result = cypher_quick_fparse(in, segment_callback, NULL,
            CYPHER_PARSE_ONLY_STATEMENTS);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 2);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], " :foo bar\"baz\"\n return 2;");
    ck_assert(!eofs[1]);
}
END_TEST


START_TEST (fparse_eof_statement)
{
    fill_stream("return 1; return 2");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 2);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], " return 2");
    ck_assert(eofs[1]);
}
END_TEST


START_TEST (fparse_eof_command)
{
    fill_stream(":bar\n:foo bar\"baz\"");

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 2);

    ck_assert_str_eq(segments[0], ":bar\n");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], ":foo bar\"baz\"");
    ck_assert(eofs[1]);
}
END_TEST


TCase* quick_fparse_tcase(void)
{
    TCase *tc = tcase_create("quick_fparse");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, fparse_empty);
    tcase_add_test(tc, fparse_single);
    tcase_add_test(tc, fparse_multiple);
    tcase_add_test(tc, fparse_commands);
    tcase_add_test(tc, fparse_statements_only);
    tcase_add_test(tc, fparse_eof_statement);
    tcase_add_test(tc, fparse_eof_command);
    return tc;
}
