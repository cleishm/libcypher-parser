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
#include <check.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

TCase* call_tcase(void);
TCase* case_tcase(void);
TCase* command_tcase(void);
TCase* constraints_tcase(void);
TCase* create_tcase(void);
TCase* delete_tcase(void);
TCase* eof_tcase(void);
TCase* error_tracking_tcase(void);
TCase* errors_tcase(void);
TCase* expression_tcase(void);
TCase* foreach_tcase(void);
TCase* indexes_tcase(void);
TCase* list_comprehensions_tcase(void);
TCase* load_csv_tcase(void);
TCase* map_projection_tcase(void);
TCase* match_tcase(void);
TCase* merge_tcase(void);
TCase* pattern_tcase(void);
TCase* pattern_comprehension_tcase(void);
TCase* query_tcase(void);
TCase* quick_parse_tcase(void);
TCase* quick_fparse_tcase(void);
TCase* reduce_tcase(void);
TCase* remove_tcase(void);
TCase* return_tcase(void);
TCase* segments_tcase(void);
TCase* set_tcase(void);
TCase* start_tcase(void);
TCase* statement_tcase(void);
TCase* union_tcase(void);
TCase* unwind_tcase(void);
TCase* util_tcase(void);
TCase* with_tcase(void);

Suite *libcypher_parser_suite(void)
{
	Suite *s = suite_create("libcypher-parser");
	suite_add_tcase(s, call_tcase());
	suite_add_tcase(s, case_tcase());
	suite_add_tcase(s, command_tcase());
	suite_add_tcase(s, constraints_tcase());
	suite_add_tcase(s, create_tcase());
	suite_add_tcase(s, delete_tcase());
	suite_add_tcase(s, eof_tcase());
	suite_add_tcase(s, error_tracking_tcase());
	suite_add_tcase(s, errors_tcase());
	suite_add_tcase(s, expression_tcase());
	suite_add_tcase(s, foreach_tcase());
	suite_add_tcase(s, indexes_tcase());
	suite_add_tcase(s, list_comprehensions_tcase());
	suite_add_tcase(s, load_csv_tcase());
	suite_add_tcase(s, map_projection_tcase());
	suite_add_tcase(s, match_tcase());
	suite_add_tcase(s, merge_tcase());
	suite_add_tcase(s, pattern_tcase());
	suite_add_tcase(s, pattern_comprehension_tcase());
	suite_add_tcase(s, query_tcase());
	suite_add_tcase(s, reduce_tcase());
	suite_add_tcase(s, remove_tcase());
	suite_add_tcase(s, return_tcase());
	suite_add_tcase(s, segments_tcase());
	suite_add_tcase(s, set_tcase());
	suite_add_tcase(s, start_tcase());
	suite_add_tcase(s, statement_tcase());
	suite_add_tcase(s, union_tcase());
	suite_add_tcase(s, unwind_tcase());
	suite_add_tcase(s, util_tcase());
	suite_add_tcase(s, with_tcase());
	return s;
}