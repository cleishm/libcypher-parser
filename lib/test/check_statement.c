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
#include "../../lib/src/cypher-parser.h"
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


START_TEST (parse_statement_with_no_options)
{
    struct cypher_input_position last = cypher_input_position_zero;
    result = cypher_parse("RETURN 1;", &last, NULL, 0);
    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(last.offset, 9);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0  0..9  statement           body=@1\n"
"@1  0..9  > query             clauses=[@2]\n"
"@2  0..8  > > RETURN          projections=[@3]\n"
"@3  7..8  > > > projection    expression=@4, alias=@5\n"
"@4  7..8  > > > > integer     1\n"
"@5  7..8  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);
    ck_assert_int_eq(cypher_astnode_range(ast).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(ast).end.offset, 9);

    const cypher_astnode_t *body = cypher_ast_statement_get_body(ast);
    ck_assert_int_eq(cypher_astnode_type(body), CYPHER_AST_QUERY);
    ck_assert_int_eq(cypher_astnode_range(body).start.offset, 0);
    ck_assert_int_eq(cypher_astnode_range(body).end.offset, 9);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 0);
    ck_assert_ptr_eq(cypher_ast_statement_get_option(ast, 0), NULL);
}
END_TEST


START_TEST (parse_statement_with_cypher_option)
{
    result = cypher_parse("CYPHER RETURN 1;", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..16  statement           options=[@1], body=@2\n"
"@1   0..7   > CYPHER\n"
"@2   7..16  > query             clauses=[@3]\n"
"@3   7..15  > > RETURN          projections=[@4]\n"
"@4  14..15  > > > projection    expression=@5, alias=@6\n"
"@5  14..15  > > > > integer     1\n"
"@6  14..15  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    ck_assert_ptr_eq(cypher_ast_cypher_option_get_version(option), NULL);
    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 0);
    ck_assert_ptr_eq(cypher_ast_cypher_option_get_param(option, 0), NULL);
}
END_TEST


START_TEST (parse_statement_with_cypher_option_containing_version)
{
    result = cypher_parse("CYPHER 3.0 RETURN 1;", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..20  statement           options=[@1], body=@3\n"
"@1   0..10  > CYPHER            version=@2\n"
"@2   7..10  > > string          \"3.0\"\n"
"@3  11..20  > query             clauses=[@4]\n"
"@4  11..19  > > RETURN          projections=[@5]\n"
"@5  18..19  > > > projection    expression=@6, alias=@7\n"
"@6  18..19  > > > > integer     1\n"
"@7  18..19  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    const cypher_astnode_t *version =
            cypher_ast_cypher_option_get_version(option);
    ck_assert_int_eq(cypher_astnode_type(version), CYPHER_AST_STRING);
    ck_assert_str_eq(cypher_ast_string_get_value(version), "3.0");

    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 0);
    ck_assert_ptr_eq(cypher_ast_cypher_option_get_param(option, 0), NULL);
}
END_TEST


START_TEST (parse_statement_params_types)
{
    result = cypher_parse("CYPHER int_val=1 float_val=2.3 true_val=true false_val=false null_val=NULL string_val='str' arr_val=[1,2,3] map_val={ int_val:1, float_val:2.3} RETURN 1", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
" @0    0..152  statement             options=[@1], body=@33\n"
" @1    0..144  > CYPHER              params=[@2, @5, @8, @11, @14, @17, @20, @26]\n"
" @2    7..17   > > cypher parameter  @3 = @4\n"
" @3    7..14   > > > string          \"int_val\"\n"
" @4   15..16   > > > integer         1\n"
" @5   17..31   > > cypher parameter  @6 = @7\n"
" @6   17..26   > > > string          \"float_val\"\n"
" @7   27..30   > > > float           2.3\n"
" @8   31..45   > > cypher parameter  @9 = @10\n"
" @9   31..39   > > > string          \"true_val\"\n"
"@10   40..44   > > > TRUE\n"
"@11   45..61   > > cypher parameter  @12 = @13\n"
"@12   45..54   > > > string          \"false_val\"\n"
"@13   55..60   > > > FALSE\n"
"@14   61..75   > > cypher parameter  @15 = @16\n"
"@15   61..69   > > > string          \"null_val\"\n"
"@16   70..74   > > > NULL\n"
"@17   75..92   > > cypher parameter  @18 = @19\n"
"@18   75..85   > > > string          \"string_val\"\n"
"@19   86..91   > > > string          \"str\"\n"
"@20   92..108  > > cypher parameter  @21 = @22\n"
"@21   92..99   > > > string          \"arr_val\"\n"
"@22  100..107  > > > collection      [@23, @24, @25]\n"
"@23  101..102  > > > > integer       1\n"
"@24  103..104  > > > > integer       2\n"
"@25  105..106  > > > > integer       3\n"
"@26  108..144  > > cypher parameter  @27 = @28\n"
"@27  108..115  > > > string          \"map_val\"\n"
"@28  116..143  > > > map             {@29:@30, @31:@32}\n"
"@29  118..125  > > > > prop name     `int_val`\n"
"@30  126..127  > > > > integer       1\n"
"@31  129..138  > > > > prop name     `float_val`\n"
"@32  139..142  > > > > float         2.3\n"
"@33  144..152  > query               clauses=[@34]\n"
"@34  144..152  > > RETURN            projections=[@35]\n"
"@35  151..152  > > > projection      expression=@36, alias=@37\n"
"@36  151..152  > > > > integer       1\n"
"@37  151..152  > > > > identifier    `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);
    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 8);

    const cypher_astnode_t *param =
            cypher_ast_cypher_option_get_param(option, 0);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);

    const cypher_astnode_t *name =
            cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    const cypher_astnode_t *value = 
            cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_INTEGER);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "int_val");
    ck_assert_str_eq(cypher_ast_integer_get_valuestr(value), "1");

    param = cypher_ast_cypher_option_get_param(option, 1);
    ck_assert_int_eq(cypher_astnode_type(param),CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_FLOAT);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "float_val");
    ck_assert_str_eq(cypher_ast_float_get_valuestr(value), "2.3");

    param = cypher_ast_cypher_option_get_param(option, 2);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_TRUE);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "true_val");

    param = cypher_ast_cypher_option_get_param(option, 3);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_FALSE);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "false_val");

    param = cypher_ast_cypher_option_get_param(option, 4);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_NULL);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "null_val");

    param = cypher_ast_cypher_option_get_param(option, 5);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_STRING);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "string_val");
    ck_assert_str_eq(cypher_ast_string_get_value(value), "str");

    param = cypher_ast_cypher_option_get_param(option, 6);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_COLLECTION);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "arr_val");

    param = cypher_ast_cypher_option_get_param(option, 7);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_MAP);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "map_val");
}
END_TEST

START_TEST (parse_params_only)
{
    result = cypher_parse("CYPHER param1=1 param2='str' MATCH (n) WHERE n.x = $param1 and n.y = $param2 RETURN n", NULL, NULL, CYPHER_PARSE_ONLY_PARAMETERS);
    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..85  statement             options=[@1], body=@8\n"
"@1   0..29  > CYPHER              params=[@2, @5]\n"
"@2   7..16  > > cypher parameter  @3 = @4\n"
"@3   7..13  > > > string          \"param1\"\n"
"@4  14..15  > > > integer         1\n"
"@5  16..29  > > cypher parameter  @6 = @7\n"
"@6  16..22  > > > string          \"param2\"\n"
"@7  23..28  > > > string          \"str\"\n"
"@8  29..85  > string              \"MATCH (n) WHERE n.x = $param1 and n.y = $param2 RETURN n\"\n";
    ck_assert_str_eq(memstream_buffer, expected);
    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 2);

    const cypher_astnode_t *param =
            cypher_ast_cypher_option_get_param(option, 0);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);

    const cypher_astnode_t *name =
            cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    const cypher_astnode_t *value = 
            cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_INTEGER);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "param1");
    ck_assert_str_eq(cypher_ast_integer_get_valuestr(value), "1");

    param = cypher_ast_cypher_option_get_param(option, 1);
    ck_assert_int_eq(cypher_astnode_type(param),CYPHER_AST_CYPHER_OPTION_PARAM);
    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_STRING);
    ck_assert_str_eq(cypher_ast_string_get_value(name), "param2");
    ck_assert_str_eq(cypher_ast_string_get_value(value), "str");
}
END_TEST

START_TEST (parse_params_only_without_params)
{
    result = cypher_parse("MATCH (n) WHERE n.x = $param1 and n.y = $param2 RETURN n", NULL, NULL, CYPHER_PARSE_ONLY_PARAMETERS);
    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0  0..56  statement  body=@1\n"
"@1  0..56  > string   \"MATCH (n) WHERE n.x = $param1 and n.y = $param2 RETURN n\"\n";
    ck_assert_str_eq(memstream_buffer, expected);
    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 0);
}
END_TEST


START_TEST (parse_statement_with_cypher_option_containing_params)
{
    result = cypher_parse("CYPHER runtime=\"fast\" RETURN 1;",
            NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..31  statement             options=[@1], body=@5\n"
"@1   0..22  > CYPHER              params=[@2]\n"
"@2   7..22  > > cypher parameter  @3 = @4\n"
"@3   7..14  > > > string          \"runtime\"\n"
"@4  15..21  > > > string          \"fast\"\n"
"@5  22..31  > query               clauses=[@6]\n"
"@6  22..30  > > RETURN            projections=[@7]\n"
"@7  29..30  > > > projection      expression=@8, alias=@9\n"
"@8  29..30  > > > > integer       1\n"
"@9  29..30  > > > > identifier    `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    ck_assert_ptr_eq(cypher_ast_cypher_option_get_version(option), NULL);

    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 1);
    const cypher_astnode_t *param =
            cypher_ast_cypher_option_get_param(option, 0);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);

    const cypher_astnode_t *name =
            cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    const cypher_astnode_t *value =
            cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_STRING);

    ck_assert_str_eq(cypher_ast_string_get_value(name), "runtime");
    ck_assert_str_eq(cypher_ast_string_get_value(value), "fast");
}
END_TEST


START_TEST (parse_statement_with_cypher_option_containing_version_and_params)
{
    result = cypher_parse("CYPHER 2.3 runtime=\"fast\" planner=\"slow\" RETURN 1;",
            NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
" @0   0..50  statement             options=[@1], body=@9\n"
" @1   0..41  > CYPHER              version=@2, params=[@3, @6]\n"
" @2   7..10  > > string            \"2.3\"\n"
" @3  11..26  > > cypher parameter  @4 = @5\n"
" @4  11..18  > > > string          \"runtime\"\n"
" @5  19..25  > > > string          \"fast\"\n"
" @6  26..41  > > cypher parameter  @7 = @8\n"
" @7  26..33  > > > string          \"planner\"\n"
" @8  34..40  > > > string          \"slow\"\n"
" @9  41..50  > query               clauses=[@10]\n"
"@10  41..49  > > RETURN            projections=[@11]\n"
"@11  48..49  > > > projection      expression=@12, alias=@13\n"
"@12  48..49  > > > > integer       1\n"
"@13  48..49  > > > > identifier    `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    const cypher_astnode_t *version =
            cypher_ast_cypher_option_get_version(option);
    ck_assert_int_eq(cypher_astnode_type(version), CYPHER_AST_STRING);
    ck_assert_str_eq(cypher_ast_string_get_value(version), "2.3");

    ck_assert_int_eq(cypher_ast_cypher_option_nparams(option), 2);

    const cypher_astnode_t *param =
            cypher_ast_cypher_option_get_param(option, 0);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);

    const cypher_astnode_t *name =
            cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    const cypher_astnode_t *value =
            cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_STRING);

    ck_assert_str_eq(cypher_ast_string_get_value(name), "runtime");
    ck_assert_str_eq(cypher_ast_string_get_value(value), "fast");

    param = cypher_ast_cypher_option_get_param(option, 1);
    ck_assert_int_eq(cypher_astnode_type(param),
            CYPHER_AST_CYPHER_OPTION_PARAM);

    name = cypher_ast_cypher_option_param_get_name(param);
    ck_assert_int_eq(cypher_astnode_type(name), CYPHER_AST_STRING);
    value = cypher_ast_cypher_option_param_get_value(param);
    ck_assert_int_eq(cypher_astnode_type(value), CYPHER_AST_STRING);

    ck_assert_str_eq(cypher_ast_string_get_value(name), "planner");
    ck_assert_str_eq(cypher_ast_string_get_value(value), "slow");
}
END_TEST


START_TEST (parse_statement_with_explain_option)
{
    result = cypher_parse("EXPLAIN RETURN 1;", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..17  statement           options=[@1], body=@2\n"
"@1   0..7   > EXPLAIN\n"
"@2   8..17  > query             clauses=[@3]\n"
"@3   8..16  > > RETURN          projections=[@4]\n"
"@4  15..16  > > > projection    expression=@5, alias=@6\n"
"@5  15..16  > > > > integer     1\n"
"@6  15..16  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_EXPLAIN_OPTION);
}
END_TEST


START_TEST (parse_statement_with_profile_option)
{
    result = cypher_parse("PROFILE RETURN 1;", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..17  statement           options=[@1], body=@2\n"
"@1   0..7   > PROFILE\n"
"@2   8..17  > query             clauses=[@3]\n"
"@3   8..16  > > RETURN          projections=[@4]\n"
"@4  15..16  > > > projection    expression=@5, alias=@6\n"
"@5  15..16  > > > > integer     1\n"
"@6  15..16  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 1);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_PROFILE_OPTION);
}
END_TEST


START_TEST (parse_statement_with_multiple_options)
{
    result = cypher_parse("CYPHER 3.0 PROFILE RETURN 1;", NULL, NULL, 0);

    ck_assert(cypher_parse_result_fprint_ast(result, memstream, 0, NULL, 0) == 0);
    fflush(memstream);
    const char *expected = "\n"
"@0   0..28  statement           options=[@1, @3], body=@4\n"
"@1   0..10  > CYPHER            version=@2\n"
"@2   7..10  > > string          \"3.0\"\n"
"@3  11..18  > PROFILE\n"
"@4  19..28  > query             clauses=[@5]\n"
"@5  19..27  > > RETURN          projections=[@6]\n"
"@6  26..27  > > > projection    expression=@7, alias=@8\n"
"@7  26..27  > > > > integer     1\n"
"@8  26..27  > > > > identifier  `1`\n";
    ck_assert_str_eq(memstream_buffer, expected);

    ck_assert_int_eq(cypher_parse_result_ndirectives(result), 1);
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    ck_assert_int_eq(cypher_astnode_type(ast), CYPHER_AST_STATEMENT);

    ck_assert_int_eq(cypher_ast_statement_noptions(ast), 2);
    const cypher_astnode_t *option = cypher_ast_statement_get_option(ast, 0);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_CYPHER_OPTION);

    option = cypher_ast_statement_get_option(ast, 1);
    ck_assert(cypher_astnode_instanceof(option, CYPHER_AST_STATEMENT_OPTION));
    ck_assert_int_eq(cypher_astnode_type(option), CYPHER_AST_PROFILE_OPTION);
}
END_TEST


TCase* statement_tcase(void)
{
    TCase *tc = tcase_create("statement");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, parse_statement_with_no_options);
    tcase_add_test(tc, parse_statement_with_cypher_option);
    tcase_add_test(tc, parse_statement_with_cypher_option_containing_version);
    tcase_add_test(tc, parse_statement_with_cypher_option_containing_params);
    tcase_add_test(tc, parse_statement_with_cypher_option_containing_version_and_params);
    tcase_add_test(tc, parse_statement_with_explain_option);
    tcase_add_test(tc, parse_statement_with_profile_option);
    tcase_add_test(tc, parse_statement_with_multiple_options);
    tcase_add_test(tc, parse_statement_params_types);
    tcase_add_test(tc, parse_params_only);
    tcase_add_test(tc, parse_params_only_without_params);
    return tc;
}
