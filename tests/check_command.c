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
#include "memstream.h"
#include <check.h>
#include <errno.h>
#include <unistd.h>


static cypher_parse_result_t *result;
static char *memstream_buffer;
static size_t memstream_size;
static FILE *memstream;


static void setup(void)
{
    result = NULL;
    memstream = open_memstream(&memstream_buffer, &memstream_size);
    fputc('\n', memstream);
}


static void teardown(void)
{
    cypher_parse_result_free(result);
    fclose(memstream);
    free(memstream_buffer);
}


START_TEST (parse_single_command_with_no_args)
{
    struct cypher_input_position last = cypher_input_position_zero;
    result = cypher_parse(":hunter\n", &last, NULL, 0);
    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(last.offset, 8);

    ck_assert(cypher_parse_result_fprint(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0  0..7  command   name=@1, args=[]\n"
"@1  1..7  > string  \"hunter\"\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_COMMAND);
    ck_assert_int_eq(cypher_astnode_range(ast).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(ast).end.offset, 7);

    const cypher_astnode_t *name = cypher_ast_command_get_name(ast);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    ck_assert_int_eq(cypher_astnode_range(name).start.offset, 1);
    ck_assert_int_eq(cypher_astnode_range(name).end.offset, 7);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "hunter");

    ck_assert_int_eq(cypher_ast_command_narguments(ast), 0);
    ck_assert_ptr_eq(cypher_ast_command_get_argument(ast, 0), NULL);
}
END_TEST


START_TEST (parse_single_command_with_args)
{
    struct cypher_input_position last = cypher_input_position_zero;
    result = cypher_parse(":hunter s thompson\n", &last, NULL, 0);
    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(last.offset, 19);

    ck_assert(cypher_parse_result_fprint(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..18  command   name=@1, args=[@2, @3]\n"
"@1   1..7   > string  \"hunter\"\n"
"@2   8..9   > string  \"s\"\n"
"@3  10..18  > string  \"thompson\"\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_COMMAND);
    ck_assert_int_eq(cypher_astnode_range(ast).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(ast).end.offset, 18);

    const cypher_astnode_t *name = cypher_ast_command_get_name(ast);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    ck_assert_int_eq(cypher_astnode_range(name).start.offset, 1);
    ck_assert_int_eq(cypher_astnode_range(name).end.offset, 7);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "hunter");

    ck_assert_int_eq(cypher_ast_command_narguments(ast), 2);

    const cypher_astnode_t *arg = cypher_ast_command_get_argument(ast, 0);
    ck_assert_int_eq(cypher_astnode_type(arg), CYPHER_AST_STRING);
    ck_assert_int_eq(cypher_astnode_range(arg).start.offset, 8);
    ck_assert_int_eq(cypher_astnode_range(arg).end.offset, 9);
    ck_assert_str_eq(cypher_ast_string_get_value(arg), "s");

    arg = cypher_ast_command_get_argument(ast, 1);
    ck_assert_int_eq(cypher_astnode_type(arg), CYPHER_AST_STRING);
    ck_assert_int_eq(cypher_astnode_range(arg).start.offset, 10);
    ck_assert_int_eq(cypher_astnode_range(arg).end.offset, 18);
    ck_assert_str_eq(cypher_ast_string_get_value(arg), "thompson");

    ck_assert_ptr_eq(cypher_ast_command_get_argument(ast, 2), NULL);
}
END_TEST


START_TEST (parse_single_command_with_quoted_args)
{
    struct cypher_input_position last = cypher_input_position_zero;
    result = cypher_parse(":thompson 'hunter s'\n", &last, NULL, 0);
    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(last.offset, 21);

    ck_assert(cypher_parse_result_fprint(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..20  command   name=@1, args=[@2]\n"
"@1   1..9   > string  \"thompson\"\n"
"@2  10..20  > string  \"hunter s\"\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_COMMAND);
    ck_assert_int_eq(cypher_astnode_range(ast).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(ast).end.offset, 20);

    ck_assert_int_eq(cypher_ast_command_narguments(ast), 1);

    const cypher_astnode_t *name = cypher_ast_command_get_name(ast);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "thompson");

    const cypher_astnode_t *arg = cypher_ast_command_get_argument(ast, 0);
    ck_assert_str_eq(cypher_ast_string_get_value(arg), "hunter s");
}
END_TEST


START_TEST (parse_single_command_with_partial_quoted_args)
{
    struct cypher_input_position last = cypher_input_position_zero;
    result = cypher_parse(":thompson lastname='hunter s'\n", &last, NULL, 0);
    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(last.offset, 30);

    ck_assert(cypher_parse_result_fprint(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..29  command   name=@1, args=[@2]\n"
"@1   1..9   > string  \"thompson\"\n"
"@2  10..29  > string  \"lastname=hunter s\"\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_COMMAND);
    ck_assert_int_eq(cypher_astnode_range(ast).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(ast).end.offset, 29);

    ck_assert_int_eq(cypher_ast_command_narguments(ast), 1);

    const cypher_astnode_t *name = cypher_ast_command_get_name(ast);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "thompson");

    const cypher_astnode_t *arg = cypher_ast_command_get_argument(ast, 0);
    ck_assert_int_eq(cypher_astnode_range(arg).start.offset, 10);
    ck_assert_int_eq(cypher_astnode_range(arg).end.offset, 29);
    ck_assert_str_eq(cypher_ast_string_get_value(arg), "lastname=hunter s");
}
END_TEST


TCase* command_tcase(void)
{
    TCase *tc = tcase_create("command");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, parse_single_command_with_no_args);
    tcase_add_test(tc, parse_single_command_with_args);
    tcase_add_test(tc, parse_single_command_with_quoted_args);
    tcase_add_test(tc, parse_single_command_with_partial_quoted_args);
    return tc;
}
