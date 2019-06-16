cmake_minimum_required (VERSION 2.8)

project (libcypher-parser C)

set (MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})

include (CheckCCompilerFlag)
include (CheckSymbolExists)
include (CheckCSourceCompiles)
include (GNUInstallDirs)
include (CMakeParseArguments)

check_c_compiler_flag("-fPIC" CC_HAVE_FPIC)
check_c_compiler_flag("-fvisibility=hidden" CC_HAVE_VISIBILITY)

# Linker flags are ignored by check_c_compiler_flag, so we have
# to use this workaround
set (CMAKE_REQUIRED_FLAGS "-Wl,--exclude-libs=ALL")
check_c_compiler_flag("" LINKER_HAS_EXCLUDE_LIBS)
unset (CMAKE_REQUIRED_FLAGS)

if (LINKER_HAS_EXCLUDE_LIBS)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--exclude-libs=ALL")
endif ()

if (CC_HAVE_FPIC)
  set (PIC_C_FLAGS "-fPIC")
endif ()

if (CC_HAVE_VISIBILITY)
  set (VISI_C_FLAGS "-fvisibility=hidden")
endif ()

find_package(check)
find_package(fmem)
find_package(leg)

list (APPEND CMAKE_REQUIRED_DEFINITIONS
  -D_GNU_SOURCE
  -D_CRT_RAND_S
  -DVC_EXTRALEAN
  -DWIN32_LEAN_AND_MEAN)

set (SOURCES)

list (APPEND SOURCES 
    lib/src/ast.c
    lib/src/ast_all.c
    lib/src/ast_all_nodes_scan.c
    lib/src/ast_all_rels_scan.c
    lib/src/ast_any.c
    lib/src/ast_apply_all_operator.c
    lib/src/ast_apply_operator.c
    lib/src/ast_binary_operator.c
    lib/src/ast_block_comment.c
    lib/src/ast_boolean.c
    lib/src/ast_call.c
    lib/src/ast_case.c
    lib/src/ast_collection.c
    lib/src/ast_command.c
    lib/src/ast_comment.c
    lib/src/ast_comparison.c
    lib/src/ast_create.c
    lib/src/ast_create_node_prop_constraint.c
    lib/src/ast_create_node_prop_index.c
    lib/src/ast_create_rel_prop_constraint.c
    lib/src/ast_cypher_option.c
    lib/src/ast_cypher_option_param.c
    lib/src/ast_delete.c
    lib/src/ast_drop_node_prop_constraint.c
    lib/src/ast_drop_node_prop_index.c
    lib/src/ast_drop_rel_prop_constraint.c
    lib/src/ast_error.c
    lib/src/ast_explain_option.c
    lib/src/ast_expression.c
    lib/src/ast_extract.c
    lib/src/ast_false.c
    lib/src/ast_filter.c
    lib/src/ast_float.c
    lib/src/ast_foreach.c
    lib/src/ast_function_name.c
    lib/src/ast_identifier.c
    lib/src/ast_index_name.c
    lib/src/ast_integer.c
    lib/src/ast_label.c
    lib/src/ast_labels_operator.c
    lib/src/ast_line_comment.c
    lib/src/ast_list_comprehension.c
    lib/src/ast_load_csv.c
    lib/src/ast_map.c
    lib/src/ast_map_projection.c
    lib/src/ast_map_projection_all_properties.c
    lib/src/ast_map_projection_identifier.c
    lib/src/ast_map_projection_literal.c
    lib/src/ast_map_projection_property.c
    lib/src/ast_map_projection_selector.c
    lib/src/ast_match.c
    lib/src/ast_match_hint.c
    lib/src/ast_merge.c
    lib/src/ast_merge_action.c
    lib/src/ast_merge_properties.c
    lib/src/ast_named_path.c
    lib/src/ast_node_id_lookup.c
    lib/src/ast_node_index_lookup.c
    lib/src/ast_node_index_query.c
    lib/src/ast_node_pattern.c
    lib/src/ast_none.c
    lib/src/ast_null.c
    lib/src/ast_on_create.c
    lib/src/ast_on_match.c
    lib/src/ast_order_by.c
    lib/src/ast_parameter.c
    lib/src/ast_pattern.c
    lib/src/ast_pattern_comprehension.c
    lib/src/ast_pattern_path.c
    lib/src/ast_proc_name.c
    lib/src/ast_profile_option.c
    lib/src/ast_projection.c
    lib/src/ast_prop_name.c
    lib/src/ast_property_operator.c
    lib/src/ast_query.c
    lib/src/ast_query_clause.c
    lib/src/ast_query_option.c
    lib/src/ast_range.c
    lib/src/ast_reduce.c
    lib/src/ast_rel_id_lookup.c
    lib/src/ast_rel_index_lookup.c
    lib/src/ast_rel_index_query.c
    lib/src/ast_rel_pattern.c
    lib/src/ast_reltype.c
    lib/src/ast_remove.c
    lib/src/ast_remove_item.c
    lib/src/ast_remove_labels.c
    lib/src/ast_remove_property.c
    lib/src/ast_return.c
    lib/src/ast_schema_command.c
    lib/src/ast_set.c
    lib/src/ast_set_all_properties.c
    lib/src/ast_set_item.c
    lib/src/ast_set_labels.c
    lib/src/ast_set_property.c
    lib/src/ast_shortest_path.c
    lib/src/ast_single.c
    lib/src/ast_slice_operator.c
    lib/src/ast_sort_item.c
    lib/src/ast_start.c
    lib/src/ast_start_point.c
    lib/src/ast_statement.c
    lib/src/ast_statement_option.c
    lib/src/ast_string.c
    lib/src/ast_subscript_operator.c
    lib/src/ast_true.c
    lib/src/ast_unary_operator.c
    lib/src/ast_union.c
    lib/src/ast_unwind.c
    lib/src/ast_using_index.c
    lib/src/ast_using_join.c
    lib/src/ast_using_periodic_commit.c
    lib/src/ast_using_scan.c
    lib/src/ast_with.c
    lib/src/errors.c
    lib/src/operators.c
    lib/src/parser.c
    lib/src/parser_config.c
    lib/src/result.c
    lib/src/segment.c
    lib/src/string_buffer.c
    lib/src/util.c
    lib/src/vector.c)

add_library (libcypher-parser ${SOURCES})
target_compile_options(libcypher-parser PUBLIC /wd4267 /wd4244 /wd4047)
file(COPY windows/config.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

set (TESTS)

list (APPEND TESTS
    lib/test/check_call.c
    lib/test/check_case.c
    lib/test/check_command.c
    lib/test/check_constraints.c
    lib/test/check_create.c
    lib/test/check_delete.c
    lib/test/check_eof.c
    lib/test/check_error_tracking.c
    lib/test/check_errors.c
    lib/test/check_expression.c
    lib/test/check_foreach.c
    lib/test/check_indexes.c
    lib/test/check_libcypher-parser.c
    lib/test/check_list_comprehensions.c
    lib/test/check_load_csv.c
    lib/test/check_map_projection.c
    lib/test/check_match.c
    lib/test/check_merge.c
    lib/test/check_pattern.c
    lib/test/check_pattern_comprehension.c
    lib/test/check_query.c
    lib/test/check_reduce.c
    lib/test/check_remove.c
    lib/test/check_return.c
    lib/test/check_segments.c
    lib/test/check_set.c
    lib/test/check_start.c
    lib/test/check_statement.c
    lib/test/check_union.c
    lib/test/check_unwind.c
    lib/test/check_util.c
    lib/test/check_with.c
    Windows/memstream.c
    Windows/test_suite.c
)

add_executable (tests ${TESTS})
target_link_libraries(tests libcypher-parser)

if (LEG_FOUND)
    add_custom_command(
        TARGET libcypher-parser
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
        COMMAND ${LEG_PROGRAMS} parser.leg > parser_leg.c)
endif (LEG_FOUND)

function (cr_link_libraries _TARGET)
  set (multiValueArgs IF)
  cmake_parse_arguments (ARGS "" "" "${multiValueArgs}" ${ARGN})

  if (ARGS_IF)
    if (${ARGS_IF})
    else ()
      return ()
    endif ()
  endif ()

  target_link_libraries(${_TARGET} ${ARGS_UNPARSED_ARGUMENTS})
endfunction ()

if (FMEM_FOUND)
  target_include_directories(libcypher-parser PUBLIC ${FMEM_INCLUDE_DIRS})
  target_link_libraries(libcypher-parser ${FMEM_LIBRARIES})
endif (FMEM_FOUND)

if (CHECK_FOUND)
  target_include_directories(tests PUBLIC ${CHECK_INCLUDE_DIRS} ${CHECK_INCLUDE_DIRS}/..)
  target_link_libraries(tests ${CHECK_LIBRARIES} ${FMEM_LIBRARIES})
endif (CHECK_FOUND)

set (CMAKE_REQUIRED_DEFINITIONS -fvisibility=hidden)
check_c_source_compiles (
  "__attribute__((visibility(\"default\"))) int main(void) { return 0; }"
  CC_HAVE_VISIBILITY)
set (CMAKE_REQUIRED_DEFINITIONS)

install(TARGETS libcypher-parser tests
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
