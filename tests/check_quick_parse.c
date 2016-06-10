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
    fclose(in);

    for (unsigned int i = 0; i < nsegments; ++i)
    {
        free(segments[i]);
    }
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


START_TEST (parse_empty)
{
    fputs("", out);
    fclose(out);

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 0);
}
END_TEST


START_TEST (parse_single)
{
    fputs("return 1;", out);
    fclose(out);

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 1);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
}
END_TEST


START_TEST (parse_multiple)
{
    fputs("return 1; return 2;\n   return 3    ;", out);
    fclose(out);

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


START_TEST (parse_commands)
{
    fputs(":foo bar\"baz\"\n", out);
    fclose(out);

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 1);

    ck_assert_str_eq(segments[0], ":foo bar\"baz\"\n");
    ck_assert(!eofs[0]);
}
END_TEST


START_TEST (parse_statements_only)
{
    fputs("return 1; :foo bar\"baz\"\n return 2;", out);
    fclose(out);

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


START_TEST (parse_eof_statement)
{
    fputs("return 1; return 2", out);
    fclose(out);

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 2);

    ck_assert_str_eq(segments[0], "return 1;");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], " return 2");
    ck_assert(eofs[1]);
}
END_TEST


START_TEST (parse_eof_command)
{
    fputs(":bar\n:foo bar\"baz\"", out);
    fclose(out);

    int result = cypher_quick_fparse(in, segment_callback, NULL, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(nsegments, 2);

    ck_assert_str_eq(segments[0], ":bar\n");
    ck_assert(!eofs[0]);
    ck_assert_str_eq(segments[1], ":foo bar\"baz\"");
    ck_assert(eofs[1]);
}
END_TEST


TCase* quick_parse_tcase(void)
{
    TCase *tc = tcase_create("quick_parse");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, parse_empty);
    tcase_add_test(tc, parse_single);
    tcase_add_test(tc, parse_multiple);
    tcase_add_test(tc, parse_commands);
    tcase_add_test(tc, parse_statements_only);
    tcase_add_test(tc, parse_eof_statement);
    tcase_add_test(tc, parse_eof_command);
    return tc;
}
