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
#include "../src/lib/error_tracking.h"
#include <check.h>
#include <errno.h>
#include <unistd.h>


static const struct cypher_input_position pos = { 1,1,0 };

static cp_error_tracking_t et;
cypher_parse_error_t *errors;
unsigned int nerrors;


static void setup(void)
{
    cp_et_init(&et, cypher_parser_no_colorization);
    errors = NULL;
    nerrors = 0;
}


static void teardown(void)
{
    cp_et_cleanup(&et);
    cypher_parse_errors_cleanup(errors, nerrors);
    free(errors);
}


START_TEST (report_with_no_labels)
{
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 0);
}
END_TEST


START_TEST (report_with_one_label)
{
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label"), 0);
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 1);
    ck_assert_str_eq(errors[0].msg, "Invalid input 'x': expected label");
}
END_TEST


START_TEST (report_with_two_labels)
{
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label1"), 0);
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label2"), 0);
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 1);
    ck_assert_str_eq(errors[0].msg,
            "Invalid input 'x': expected label1 or label2");
}
END_TEST


START_TEST (report_with_three_labels)
{
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label1"), 0);
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label2"), 0);
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label3"), 0);
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 1);
    ck_assert_str_eq(errors[0].msg,
            "Invalid input 'x': expected label1, label2 or label3");
}
END_TEST


START_TEST (report_with_newline_char)
{
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'\n',"label"), 0);
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 1);
    ck_assert_str_eq(errors[0].msg, "Invalid input '\\n': expected label");
}
END_TEST


START_TEST (report_with_duplicate_labels)
{
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label1"), 0);
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label2"), 0);
    ck_assert_int_eq(cp_et_note_potential_error(&et,pos,'x',"label1"), 0);
    ck_assert_int_eq(cp_et_reify_potentials(&et), 0);
    errors = cp_et_extract_errors(&et, &nerrors);
    ck_assert_int_eq(nerrors, 1);
    ck_assert_str_eq(errors[0].msg,
            "Invalid input 'x': expected label1 or label2");
}
END_TEST


TCase* error_tracking_tcase(void)
{
    TCase *tc = tcase_create("error tracking");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, report_with_no_labels);
    tcase_add_test(tc, report_with_one_label);
    tcase_add_test(tc, report_with_two_labels);
    tcase_add_test(tc, report_with_three_labels);
    tcase_add_test(tc, report_with_newline_char);
    tcase_add_test(tc, report_with_duplicate_labels);
    return tc;
}
