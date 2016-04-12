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
#ifndef CYPHER_PARSER_AST_H
#define CYPHER_PARSER_AST_H

#include "cypher-parser.h"


unsigned int cypher_ast_set_ordinals(cypher_astnode_t *ast, unsigned int n);

int cypher_ast_fprintv(cypher_astnode_t * const *asts, unsigned int n,
        FILE *stream, unsigned int width,
        const struct cypher_parser_colorization *colorization,
        uint_fast32_t flags);


extern const struct cypher_astnode_vt cypher_statement_astnode_vt;
extern const struct cypher_astnode_vt cypher_statement_option_astnode_vt;
extern const struct cypher_astnode_vt cypher_cypher_option_astnode_vt;
extern const struct cypher_astnode_vt cypher_cypher_option_param_astnode_vt;
extern const struct cypher_astnode_vt cypher_explain_option_astnode_vt;
extern const struct cypher_astnode_vt cypher_profile_option_astnode_vt;
extern const struct cypher_astnode_vt cypher_schema_command_astnode_vt;
extern const struct cypher_astnode_vt cypher_create_node_prop_index_astnode_vt;
extern const struct cypher_astnode_vt cypher_drop_node_prop_index_astnode_vt;
extern const struct cypher_astnode_vt
        cypher_create_node_prop_constraint_astnode_vt;
extern const struct cypher_astnode_vt
        cypher_drop_node_prop_constraint_astnode_vt;
extern const struct cypher_astnode_vt
        cypher_create_rel_prop_constraint_astnode_vt;
extern const struct cypher_astnode_vt
        cypher_drop_rel_prop_constraint_astnode_vt;
extern const struct cypher_astnode_vt cypher_query_astnode_vt;
extern const struct cypher_astnode_vt cypher_query_option_astnode_vt;
extern const struct cypher_astnode_vt cypher_using_periodic_commit_astnode_vt;
extern const struct cypher_astnode_vt cypher_query_clause_astnode_vt;
extern const struct cypher_astnode_vt cypher_load_csv_astnode_vt;
extern const struct cypher_astnode_vt cypher_start_astnode_vt;
extern const struct cypher_astnode_vt cypher_start_point_astnode_vt;
extern const struct cypher_astnode_vt cypher_node_index_lookup_astnode_vt;
extern const struct cypher_astnode_vt cypher_node_index_query_astnode_vt;
extern const struct cypher_astnode_vt cypher_node_id_lookup_astnode_vt;
extern const struct cypher_astnode_vt cypher_all_nodes_scan_astnode_vt;
extern const struct cypher_astnode_vt cypher_rel_index_lookup_astnode_vt;
extern const struct cypher_astnode_vt cypher_rel_index_query_astnode_vt;
extern const struct cypher_astnode_vt cypher_rel_id_lookup_astnode_vt;
extern const struct cypher_astnode_vt cypher_all_rels_scan_astnode_vt;
extern const struct cypher_astnode_vt cypher_match_astnode_vt;
extern const struct cypher_astnode_vt cypher_match_hint_astnode_vt;
extern const struct cypher_astnode_vt cypher_using_index_astnode_vt;
extern const struct cypher_astnode_vt cypher_using_join_astnode_vt;
extern const struct cypher_astnode_vt cypher_using_scan_astnode_vt;
extern const struct cypher_astnode_vt cypher_merge_astnode_vt;
extern const struct cypher_astnode_vt cypher_merge_action_astnode_vt;
extern const struct cypher_astnode_vt cypher_on_match_astnode_vt;
extern const struct cypher_astnode_vt cypher_on_create_astnode_vt;
extern const struct cypher_astnode_vt cypher_create_astnode_vt;
extern const struct cypher_astnode_vt cypher_set_astnode_vt;
extern const struct cypher_astnode_vt cypher_set_item_astnode_vt;
extern const struct cypher_astnode_vt cypher_set_property_astnode_vt;
extern const struct cypher_astnode_vt cypher_set_all_properties_astnode_vt;
extern const struct cypher_astnode_vt cypher_merge_properties_astnode_vt;
extern const struct cypher_astnode_vt cypher_set_labels_astnode_vt;
extern const struct cypher_astnode_vt cypher_delete_astnode_vt;
extern const struct cypher_astnode_vt cypher_remove_astnode_vt;
extern const struct cypher_astnode_vt cypher_remove_item_astnode_vt;
extern const struct cypher_astnode_vt cypher_remove_labels_astnode_vt;
extern const struct cypher_astnode_vt cypher_remove_property_astnode_vt;
extern const struct cypher_astnode_vt cypher_foreach_astnode_vt;
extern const struct cypher_astnode_vt cypher_with_astnode_vt;
extern const struct cypher_astnode_vt cypher_unwind_astnode_vt;
extern const struct cypher_astnode_vt cypher_call_astnode_vt;
extern const struct cypher_astnode_vt cypher_return_astnode_vt;
extern const struct cypher_astnode_vt cypher_projection_astnode_vt;
extern const struct cypher_astnode_vt cypher_order_by_astnode_vt;
extern const struct cypher_astnode_vt cypher_sort_item_astnode_vt;
extern const struct cypher_astnode_vt cypher_union_astnode_vt;
extern const struct cypher_astnode_vt cypher_expression_astnode_vt;
extern const struct cypher_astnode_vt cypher_unary_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_binary_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_comparison_astnode_vt;
extern const struct cypher_astnode_vt cypher_apply_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_apply_all_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_property_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_subscript_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_slice_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_labels_operator_astnode_vt;
extern const struct cypher_astnode_vt cypher_list_comprehension_astnode_vt;
extern const struct cypher_astnode_vt cypher_case_astnode_vt;
extern const struct cypher_astnode_vt cypher_filter_astnode_vt;
extern const struct cypher_astnode_vt cypher_extract_astnode_vt;
extern const struct cypher_astnode_vt cypher_reduce_astnode_vt;
extern const struct cypher_astnode_vt cypher_all_astnode_vt;
extern const struct cypher_astnode_vt cypher_any_astnode_vt;
extern const struct cypher_astnode_vt cypher_single_astnode_vt;
extern const struct cypher_astnode_vt cypher_none_astnode_vt;
extern const struct cypher_astnode_vt cypher_collection_astnode_vt;
extern const struct cypher_astnode_vt cypher_map_astnode_vt;
extern const struct cypher_astnode_vt cypher_identifier_astnode_vt;
extern const struct cypher_astnode_vt cypher_parameter_astnode_vt;
extern const struct cypher_astnode_vt cypher_string_astnode_vt;
extern const struct cypher_astnode_vt cypher_integer_astnode_vt;
extern const struct cypher_astnode_vt cypher_float_astnode_vt;
extern const struct cypher_astnode_vt cypher_true_astnode_vt;
extern const struct cypher_astnode_vt cypher_false_astnode_vt;
extern const struct cypher_astnode_vt cypher_null_astnode_vt;
extern const struct cypher_astnode_vt cypher_label_astnode_vt;
extern const struct cypher_astnode_vt cypher_reltype_astnode_vt;
extern const struct cypher_astnode_vt cypher_prop_name_astnode_vt;
extern const struct cypher_astnode_vt cypher_function_name_astnode_vt;
extern const struct cypher_astnode_vt cypher_index_name_astnode_vt;
extern const struct cypher_astnode_vt cypher_proc_name_astnode_vt;
extern const struct cypher_astnode_vt cypher_pattern_astnode_vt;
extern const struct cypher_astnode_vt cypher_named_path_astnode_vt;
extern const struct cypher_astnode_vt cypher_shortest_path_astnode_vt;
extern const struct cypher_astnode_vt cypher_pattern_path_astnode_vt;
extern const struct cypher_astnode_vt cypher_node_pattern_astnode_vt;
extern const struct cypher_astnode_vt cypher_rel_pattern_astnode_vt;
extern const struct cypher_astnode_vt cypher_range_astnode_vt;
extern const struct cypher_astnode_vt cypher_command_astnode_vt;
extern const struct cypher_astnode_vt cypher_comment_astnode_vt;
extern const struct cypher_astnode_vt cypher_line_comment_astnode_vt;
extern const struct cypher_astnode_vt cypher_block_comment_astnode_vt;
extern const struct cypher_astnode_vt cypher_error_astnode_vt;

void cypher_ast_free(cypher_astnode_t *ast);

ssize_t snprint_sequence(char *str, size_t size,
        const cypher_astnode_t * const *elements, unsigned int nelements);


#endif/*CYPHER_PARSER_AST_H*/
