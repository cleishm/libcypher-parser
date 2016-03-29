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
/**
 * @file cypher-parser.h
 */
#ifndef CYPHER_PARSER_H
#define CYPHER_PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>

#if __GNUC__ > 3
#define __cypherlang_pure __attribute__((pure))
#define __cypherlang_malloc __attribute__((malloc))
#define __cypherlang_must_check __attribute__((warn_unused_result))
#define __cypherlang_format(string_index, first) \
        __attribute__((format (printf, string_index, first)))
#else
#define __cypherlang_pure /*pure*/
#define __cypherlang_malloc /*malloc*/
#define __cypherlang_must_check /*must check*/
#define __cypherlang_format(string_index, first) /*format*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC visibility push(default)


/*
 * =====================================
 * version
 * =====================================
 */

/**
 * The version string for libcypher-parser.
 */
__cypherlang_pure
const char *libcypher_parser_version(void);


/*
 * =====================================
 * colorization
 * =====================================
 */

struct cypher_parser_colorization
{
    const char *normal[2];
    const char *error[2];
    const char *error_token[2];
    const char *error_message[2];
    const char *ast_ordinal[2];
    const char *ast_range[2];
    const char *ast_indent[2];
    const char *ast_type[2];
    const char *ast_desc[2];
};

/** Colorization rules for uncolorized output. */
extern const struct cypher_parser_colorization *cypher_parser_no_colorization;
/** Colorization rules for ANSI terminal output. */
extern const struct cypher_parser_colorization *cypher_parser_ansi_colorization;


/*
 * =====================================
 * abstract syntax tree
 * =====================================
 */

/**
 * An abstract syntax tree node.
 */
typedef struct cypher_astnode cypher_astnode_t;

/**
 * A cypher AST node type.
 */
typedef uint8_t cypher_astnode_type_t;

/** Type for an AST statement node. */
extern const cypher_astnode_type_t CYPHER_AST_STATEMENT;
/** Type for an AST `CYPHER` option node. */
extern const cypher_astnode_type_t CYPHER_AST_CYPHER_OPTION;
/** Type for an AST `CYPHER` option parameter node. */
extern const cypher_astnode_type_t CYPHER_AST_CYPHER_OPTION_PARAM;
/** Type for an AST `EXPLAIN` option node. */
extern const cypher_astnode_type_t CYPHER_AST_EXPLAIN_OPTION;
/** Type for an AST `PROFILE` option node. */
extern const cypher_astnode_type_t CYPHER_AST_PROFILE_OPTION;
/** Type for an AST `CREATE INDEX` node. */
extern const cypher_astnode_type_t CYPHER_AST_CREATE_INDEX;
/** Type for an AST `DROP INDEX` node. */
extern const cypher_astnode_type_t CYPHER_AST_DROP_INDEX;
/** Type for an AST create unique node property constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_CREATE_UNIQUE_NODE_PROP_CONSTRAINT;
/** Type for an AST drop unique node property constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_DROP_UNIQUE_NODE_PROP_CONSTRAINT;
/** Type for an AST create node property exists constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_CREATE_NODE_PROP_EXISTS_CONSTRAINT;
/** Type for an AST drop node property exists constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_DROP_NODE_PROP_EXISTS_CONSTRAINT;
/** Type for an AST create rel property exists constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_CREATE_REL_PROP_EXISTS_CONSTRAINT;
/** Type for an AST drop rel property exists constraint node. */
extern const cypher_astnode_type_t CYPHER_AST_DROP_REL_PROP_EXISTS_CONSTRAINT;
/** Type for an AST query node. */
extern const cypher_astnode_type_t CYPHER_AST_QUERY;
/** Type for an AST `USING PERIODIC COMMIT` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_USING_PERIODIC_COMMIT;
/** Type for an AST `LOAD CSV` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_LOAD_CSV;
/** Type for an AST `START` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_START;
/** Type for an AST node-index lookup node. */
extern const cypher_astnode_type_t CYPHER_AST_NODE_INDEX_LOOKUP;
/** Type for an AST node-index query node. */
extern const cypher_astnode_type_t CYPHER_AST_NODE_INDEX_QUERY;
/** Type for an AST node-by-id-lookup node. */
extern const cypher_astnode_type_t CYPHER_AST_NODE_ID_LOOKUP;
/** Type for an AST all-nodes-scan node. */
extern const cypher_astnode_type_t CYPHER_AST_ALL_NODES_SCAN;
/** Type for an AST rel-index lookup node. */
extern const cypher_astnode_type_t CYPHER_AST_REL_INDEX_LOOKUP;
/** Type for an AST rel-index query node. */
extern const cypher_astnode_type_t CYPHER_AST_REL_INDEX_QUERY;
/** Type for an AST rel-by-id-lookup node. */
extern const cypher_astnode_type_t CYPHER_AST_REL_ID_LOOKUP;
/** Type for an AST all-rels-scan node. */
extern const cypher_astnode_type_t CYPHER_AST_ALL_RELS_SCAN;
/** Type for an AST `MATCH` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_MATCH;
/** Type for an AST `USING INDEX` hint node. */
extern const cypher_astnode_type_t CYPHER_AST_USING_INDEX_HINT;
/** Type for an AST `USING JOIN ON` hint node. */
extern const cypher_astnode_type_t CYPHER_AST_USING_JOIN_HINT;
/** Type for an AST `USING SCAN` hint node. */
extern const cypher_astnode_type_t CYPHER_AST_USING_SCAN_HINT;
/** Type for an AST `MERGE` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_MERGE;
/** Type for an AST `ON MATCH` action node. */
extern const cypher_astnode_type_t CYPHER_AST_ON_MATCH;
/** Type for an AST `ON CREATE` action node. */
extern const cypher_astnode_type_t CYPHER_AST_ON_CREATE;
/** Type for an AST `CREATE` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_CREATE;
/** Type for an AST `SET` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_SET;
/** Type for an AST set property node. */
extern const cypher_astnode_type_t CYPHER_AST_SET_PROPERTY;
/** Type for an AST set all properties node. */
extern const cypher_astnode_type_t CYPHER_AST_SET_ALL_PROPERTIES;
/** Type for an AST merge properties node. */
extern const cypher_astnode_type_t CYPHER_AST_MERGE_PROPERTIES;
/** Type for an AST set labels node. */
extern const cypher_astnode_type_t CYPHER_AST_SET_LABELS;
/** Type for an AST `DELETE` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_DELETE;
/** Type for an AST `REMOVE` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_REMOVE;
/** Type for an AST remove property node. */
extern const cypher_astnode_type_t CYPHER_AST_REMOVE_PROPERTY;
/** Type for an AST remove labels node. */
extern const cypher_astnode_type_t CYPHER_AST_REMOVE_LABELS;
/** Type for an AST `FOREACH` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_FOREACH;
/** Type for an AST `WITH` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_WITH;
/** Type for an AST `UNWIND` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_UNWIND;
/** Type for an AST `CALL` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_CALL;
/** Type for an AST `RETURN` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_RETURN;
/** Type for an AST projection node. */
extern const cypher_astnode_type_t CYPHER_AST_PROJECTION;
/** Type for an AST `ORDER BY` node. */
extern const cypher_astnode_type_t CYPHER_AST_ORDER_BY;
/** Type for an AST sort item node. */
extern const cypher_astnode_type_t CYPHER_AST_SORT_ITEM;
/** Type for an AST `UNION` clause node. */
extern const cypher_astnode_type_t CYPHER_AST_UNION;
/** Type for an AST unary operator node. */
extern const cypher_astnode_type_t CYPHER_AST_UNARY_OPERATOR;
/** Type for an AST binary operator node. */
extern const cypher_astnode_type_t CYPHER_AST_BINARY_OPERATOR;
/** Type for an AST n-ary comparison operator node. */
extern const cypher_astnode_type_t CYPHER_AST_COMPARISON;
/** Type for an AST apply operator node. */
extern const cypher_astnode_type_t CYPHER_AST_APPLY_OPERATOR;
/** Type for an AST apply * operator node. */
extern const cypher_astnode_type_t CYPHER_AST_APPLY_ALL_OPERATOR;
/** Type for an AST property operator node. */
extern const cypher_astnode_type_t CYPHER_AST_PROPERTY_OPERATOR;
/** Type for an AST subscript operator node. */
extern const cypher_astnode_type_t CYPHER_AST_SUBSCRIPT_OPERATOR;
/** Type for an AST subscript operator node. */
extern const cypher_astnode_type_t CYPHER_AST_SLICE_OPERATOR;
/** Type for an AST label check operator node. */
extern const cypher_astnode_type_t CYPHER_AST_LABELS_OPERATOR;
/** Type for an AST list comprehension node. */
extern const cypher_astnode_type_t CYPHER_AST_LIST_COMPREHENSION;
/** Type for an AST CASE expression node. */
extern const cypher_astnode_type_t CYPHER_AST_CASE;
/** Type for an AST filter expression node. */
extern const cypher_astnode_type_t CYPHER_AST_FILTER;
/** Type for an AST extract expression node. */
extern const cypher_astnode_type_t CYPHER_AST_EXTRACT;
/** Type for an AST reduce expression node. */
extern const cypher_astnode_type_t CYPHER_AST_REDUCE;
/** Type for an AST all predicate node. */
extern const cypher_astnode_type_t CYPHER_AST_ALL;
/** Type for an AST any predicate node. */
extern const cypher_astnode_type_t CYPHER_AST_ANY;
/** Type for an AST single predicate node. */
extern const cypher_astnode_type_t CYPHER_AST_SINGLE;
/** Type for an AST none predicate node. */
extern const cypher_astnode_type_t CYPHER_AST_NONE;
/** Type for an AST collection node. */
extern const cypher_astnode_type_t CYPHER_AST_COLLECTION;
/** Type for an AST literal map node. */
extern const cypher_astnode_type_t CYPHER_AST_MAP;
/** Type for an AST identifier node. */
extern const cypher_astnode_type_t CYPHER_AST_IDENTIFIER;
/** Type for an AST parameter node. */
extern const cypher_astnode_type_t CYPHER_AST_PARAMETER;
/** Type for an AST string literal node. */
extern const cypher_astnode_type_t CYPHER_AST_STRING;
/** Type for an AST integer literal node. */
extern const cypher_astnode_type_t CYPHER_AST_INTEGER;
/** Type for an AST float literal node. */
extern const cypher_astnode_type_t CYPHER_AST_FLOAT;
/** Type for an AST TRUE literal node. */
extern const cypher_astnode_type_t CYPHER_AST_TRUE;
/** Type for an AST FALSE literal node. */
extern const cypher_astnode_type_t CYPHER_AST_FALSE;
/** Type for an AST NULL literal node. */
extern const cypher_astnode_type_t CYPHER_AST_NULL;
/** Type for an AST label node. */
extern const cypher_astnode_type_t CYPHER_AST_LABEL;
/** Type for an AST reltype node. */
extern const cypher_astnode_type_t CYPHER_AST_RELTYPE;
/** Type for an AST prop name node. */
extern const cypher_astnode_type_t CYPHER_AST_PROP_NAME;
/** Type for an AST function name node. */
extern const cypher_astnode_type_t CYPHER_AST_FUNCTION_NAME;
/** Type for an AST index name node. */
extern const cypher_astnode_type_t CYPHER_AST_INDEX_NAME;
/** Type for an AST procedure name node. */
extern const cypher_astnode_type_t CYPHER_AST_PROC_NAME;
/** Type for an AST pattern node. */
extern const cypher_astnode_type_t CYPHER_AST_PATTERN;
/** Type for an AST named pattern path node. */
extern const cypher_astnode_type_t CYPHER_AST_NAMED_PATH;
/** Type for an AST shortestPath node. */
extern const cypher_astnode_type_t CYPHER_AST_SHORTEST_PATH;
/** Type for an AST pattern path node. */
extern const cypher_astnode_type_t CYPHER_AST_PATTERN_PATH;
/** Type for an AST node pattern node. */
extern const cypher_astnode_type_t CYPHER_AST_NODE_PATTERN;
/** Type for an AST relationship pattern node. */
extern const cypher_astnode_type_t CYPHER_AST_REL_PATTERN;
/** Type for an AST range node. */
extern const cypher_astnode_type_t CYPHER_AST_RANGE;
/** Type for an AST command node. */
extern const cypher_astnode_type_t CYPHER_AST_COMMAND;
/** Type for an AST line comment node. */
extern const cypher_astnode_type_t CYPHER_AST_LINE_COMMENT;
/** Type for an AST block comment node. */
extern const cypher_astnode_type_t CYPHER_AST_BLOCK_COMMENT;
/** Type for an AST error node. */
extern const cypher_astnode_type_t CYPHER_AST_ERROR;

/**
 * A cypher expression operator.
 */
typedef struct cypher_operator cypher_operator_t;

/** The infix binary `OR` operator. */
extern const cypher_operator_t *CYPHER_OP_OR;
/** The infix binary `XOR` operator. */
extern const cypher_operator_t *CYPHER_OP_XOR;
/** The infix binary `AND` operator. */
extern const cypher_operator_t *CYPHER_OP_AND;
/** The prefix unary `NOT` operator. */
extern const cypher_operator_t *CYPHER_OP_NOT;
/** The infix binary `=` operator. */
extern const cypher_operator_t *CYPHER_OP_EQUAL;
/** The infix binary `<>` operator. */
extern const cypher_operator_t *CYPHER_OP_NEQUAL;
/** The n-ary `<` operator. */
extern const cypher_operator_t *CYPHER_OP_LT;
/** The n-ary `>` operator. */
extern const cypher_operator_t *CYPHER_OP_GT;
/** The n-ary `<=` operator. */
extern const cypher_operator_t *CYPHER_OP_LTE;
/** The n-ary `>=` operator. */
extern const cypher_operator_t *CYPHER_OP_GTE;
/** The infix binary `+` operator. */
extern const cypher_operator_t *CYPHER_OP_PLUS;
/** The infix binary `-` operator. */
extern const cypher_operator_t *CYPHER_OP_MINUS;
/** The infix binary `*` operator. */
extern const cypher_operator_t *CYPHER_OP_MULT;
/** The infix binary `/` operator. */
extern const cypher_operator_t *CYPHER_OP_DIV;
/** The infix binary `%` operator. */
extern const cypher_operator_t *CYPHER_OP_MOD;
/** The infix binary `^` operator. */
extern const cypher_operator_t *CYPHER_OP_POW;
/** The prefix unary `+` operator. */
extern const cypher_operator_t *CYPHER_OP_UNARY_PLUS;
/** The prefix unary `-` operator. */
extern const cypher_operator_t *CYPHER_OP_UNARY_MINUS;
/** The postfix binary subscript (`[exp]`) operator. */
extern const cypher_operator_t *CYPHER_OP_SUBSCRIPT;
/** The infix binary `=~` operator. */
extern const cypher_operator_t *CYPHER_OP_REGEX;
/** The infix binary `IN` operator. */
extern const cypher_operator_t *CYPHER_OP_IN;
/** The infix binary `STARTS WITH` operator. */
extern const cypher_operator_t *CYPHER_OP_STARTS_WITH;
/** The infix binary `CONTAINS` operator. */
extern const cypher_operator_t *CYPHER_OP_CONTAINS;
/** The infix binary `IS NULL` operator. */
extern const cypher_operator_t *CYPHER_OP_IS_NULL;
/** The infix binary `IS NOT NULL` operator. */
extern const cypher_operator_t *CYPHER_OP_IS_NOT_NULL;
/** The infix binary property lookup (`l.r`) operator. */
extern const cypher_operator_t *CYPHER_OP_PROPERTY;
/** The n-ary label check (`l:r`) operator. */
extern const cypher_operator_t *CYPHER_OP_LABEL;

/**
 * Get the type of an AST node.
 *
 * @param [node] The AST node.
 * @return The type of the node.
 */
__cypherlang_pure
cypher_astnode_type_t cypher_astnode_type(const cypher_astnode_t *node);

/**
 * Check the type of an AST node.
 *
 * @param [node] The AST node.
 * @param [type] The AST node type.
 * @return `true` if the node is of the specified type and `false` otherwise.
 */
__cypherlang_pure
bool cypher_astnode_instanceof(const cypher_astnode_t *node,
        cypher_astnode_type_t type);

/**
 * Get a string description of the AST node type.
 *
 * @param [type] The AST node type.
 * @return A pointer to a null terminated string containing the type name.
 */
__cypherlang_pure
const char *cypher_astnode_typestr(cypher_astnode_type_t type);

/**
 * Get the number of children from an AST node.
 *
 * @param [node] The AST node.
 * @return The number of children.
 */
__cypherlang_pure
unsigned int cypher_astnode_nchildren(const cypher_astnode_t *node);

/**
 * Get a child from an AST node.
 *
 * @param [node] The AST node.
 * @param [index] The index of the child.
 * @return A pointer to the child of the AST node, or `NULL` if there is no
 *         argument at the specified index.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_astnode_child(const cypher_astnode_t *node,
        unsigned int index);

/**
 * A position in the input.
 */
struct cypher_input_position
{
    unsigned int line;
    unsigned int column;
    size_t offset;
};

extern const struct cypher_input_position cypher_input_position_zero;

/**
 * A range in the input.
 */
struct cypher_input_range
{
    struct cypher_input_position start;
    struct cypher_input_position end;
};

/**
 * Get the range of an AST node in the input.
 *
 * @param [node] The AST node.
 * @return The range.
 */
__cypherlang_pure
struct cypher_input_range cypher_astnode_range(const cypher_astnode_t *node);

/**
 * The direction of a relationship pattern.
 */
enum cypher_rel_direction
{
    CYPHER_REL_INBOUND,
    CYPHER_REL_OUTBOUND,
    CYPHER_REL_BOTH
};


/**
 * Construct a CYPHER_AST_STATEMENT node.
 *
 * @param [options] Options for the statement.
 * @param [noptions] The number of options (maybe zero).
 * @param [body] The body of the statement, which must be either an
 *         CYPHER_AST_QUERY or CYPHER_AST_SCHEMA_COMMAND.
 * @param [children] The children of the node.
 * @param [nchildren] The number of children.
 * @param [range] The input range.
 * @return An AST node, or NULL if an error occurs (errno will be set).
 */
__cypherlang_must_check
cypher_astnode_t *cypher_ast_statement(cypher_astnode_t * const *options,
        unsigned int noptions, const cypher_astnode_t *body,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

/**
 * Construct a CYPHER_AST_CYPHER_OPTION node.
 *
 * @param [version] The version string, which must be a CYPHER_AST_STRING or
 *         null.
 * @param [params] Parameters for the option.
 * @param [nparams] The number of parameters (maybe zero).
 * @param [children] The children of the node.
 * @param [nchildren] The number of children.
 * @param [range] The input range.
 * @return An AST node, or NULL if an error occurs (errno will be set).
 */
__cypherlang_must_check
cypher_astnode_t *cypher_ast_cypher_option(const cypher_astnode_t *version,
        cypher_astnode_t * const *params, unsigned int nparams,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

/**
 * Construct a CYPHER_AST_CYPHER_OPTION_PARAM node.
 *
 * @param [name] The parameter name, which must be a CYPHER_AST_STRING.
 * @param [value] The parameter value, which must be a CYPHER_AST_STRING.
 * @param [children] The children of the node.
 * @param [nchildren] The number of children.
 * @param [range] The input range.
 * @return An AST node, or NULL if an error occurs (errno will be set).
 */
__cypherlang_must_check
cypher_astnode_t *cypher_ast_cypher_option_param(const cypher_astnode_t *name,
        const cypher_astnode_t *value, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

/**
 * Construct a CYPHER_AST_EXPLAIN_OPTION node.
 *
 * @param [children] The children of the node.
 * @param [nchildren] The number of children.
 * @param [range] The input range.
 * @return An AST node, or NULL if an error occurs (errno will be set).
 */
__cypherlang_must_check
cypher_astnode_t *cypher_ast_explain_option(cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

/**
 * Construct a CYPHER_AST_PROFILE_OPTION node.
 *
 * @param [children] The children of the node.
 * @param [nchildren] The number of children.
 * @param [range] The input range.
 * @return An AST node, or NULL if an error occurs (errno will be set).
 */
__cypherlang_must_check
cypher_astnode_t *cypher_ast_profile_option(cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_create_index(const cypher_astnode_t *label,
        const cypher_astnode_t *prop_name, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_drop_index(const cypher_astnode_t *label,
        const cypher_astnode_t *prop_name, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_create_unique_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_drop_unique_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_create_node_prop_exists_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_drop_node_prop_exists_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_create_rel_prop_exists_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_drop_rel_prop_exists_constraint(
        const cypher_astnode_t *identifier, const cypher_astnode_t *label,
        const cypher_astnode_t *property, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_query(cypher_astnode_t * const *clauses,
        unsigned int nclauses, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_using_periodic_commit(
        const cypher_astnode_t *limit, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_load_csv(bool with_headers,
        const cypher_astnode_t *url, const cypher_astnode_t *identifier,
        const cypher_astnode_t *field_terminator, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_start(cypher_astnode_t * const *points,
        unsigned int npoints, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_node_index_lookup(
        const cypher_astnode_t *identifier, const cypher_astnode_t *index,
        const cypher_astnode_t *prop_name, const cypher_astnode_t *lookup,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_node_index_query(
        const cypher_astnode_t *identifier, const cypher_astnode_t *index,
        const cypher_astnode_t *query, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_node_id_lookup(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *ids, unsigned int nids,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_all_nodes_scan(const cypher_astnode_t *identifier,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_rel_index_lookup(
        const cypher_astnode_t *identifier, const cypher_astnode_t *index,
        const cypher_astnode_t *prop_name, const cypher_astnode_t *lookup,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_rel_index_query(
        const cypher_astnode_t *identifier, const cypher_astnode_t *index,
        const cypher_astnode_t *query, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_rel_id_lookup(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *ids, unsigned int nids,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_all_rels_scan(const cypher_astnode_t *identifier,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_match(bool optional,
        const cypher_astnode_t *pattern, cypher_astnode_t * const *hints,
        unsigned int nhints, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_using_index(const cypher_astnode_t *identifier,
        const cypher_astnode_t *label, const cypher_astnode_t *prop_name,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_using_join(
        cypher_astnode_t * const *identifiers, unsigned int nidentifiers,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_using_scan(const cypher_astnode_t *identifier,
        const cypher_astnode_t *label, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_merge(const cypher_astnode_t *path,
        cypher_astnode_t * const *actions, unsigned int nactions,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_create(bool unique,
        const cypher_astnode_t *pattern, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_on_match(cypher_astnode_t * const *items,
        unsigned int nitems, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_on_create(cypher_astnode_t * const *items,
        unsigned int nitems, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_set(cypher_astnode_t * const *items,
        unsigned int nitems, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_set_property(const cypher_astnode_t *property,
        const cypher_astnode_t *expression, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_set_all_properties(
        const cypher_astnode_t *identifier, const cypher_astnode_t *expression,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_merge_properties(
        const cypher_astnode_t *identifier, const cypher_astnode_t *expression,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_set_labels(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_delete(bool detach,
        cypher_astnode_t * const *expressions, unsigned int nexpressions,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_remove(cypher_astnode_t * const *items,
        unsigned int nitems, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_remove_property(const cypher_astnode_t *property,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_remove_labels(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_foreach(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, cypher_astnode_t * const *clauses,
        unsigned int nclauses, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_with(bool distinct, bool include_existing,
        cypher_astnode_t * const *items, unsigned int nitems,
        const cypher_astnode_t *order_by, const cypher_astnode_t *skip,
        const cypher_astnode_t *limit, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_unwind(const cypher_astnode_t *expression,
        const cypher_astnode_t *alias, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_call(const cypher_astnode_t *proc_name,
        cypher_astnode_t * const *args, unsigned int nargs,
        cypher_astnode_t * const *projections, unsigned int nprojections,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_return(bool distinct, bool include_existing,
        cypher_astnode_t * const *items, unsigned int nitems,
        const cypher_astnode_t *order_by, const cypher_astnode_t *skip,
        const cypher_astnode_t *limit, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_projection(const cypher_astnode_t *expression,
        const cypher_astnode_t *alias, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_order_by(cypher_astnode_t * const *items,
        unsigned int nitems, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_sort_item(const cypher_astnode_t *expression,
        bool ascending, cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_union(bool all, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_unary_operator(const cypher_operator_t *op,
        const cypher_astnode_t *arg, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_binary_operator(const cypher_operator_t *op,
        const cypher_astnode_t *arg1, const cypher_astnode_t *arg2,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_comparison(const cypher_astnode_t *left,
        const cypher_operator_t * const *ops, cypher_astnode_t * const *args,
        unsigned int length, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_apply_operator(const cypher_astnode_t *func,
        bool distinct, cypher_astnode_t * const *args, unsigned int nargs,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_apply_all_operator(const cypher_astnode_t *func,
        bool distinct, cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_property_operator(const cypher_astnode_t *map,
        const cypher_astnode_t *prop_name, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_subscript_operator(
        const cypher_astnode_t *expression, const cypher_astnode_t *subscript,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_slice_operator(const cypher_astnode_t *expression,
        const cypher_astnode_t *start, const cypher_astnode_t *end,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_labels_operator(const cypher_astnode_t *expression,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_list_comprehension(
        const cypher_astnode_t *identifier, const cypher_astnode_t *expression,
        const cypher_astnode_t *prediate, const cypher_astnode_t *eval,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_case(const cypher_astnode_t *expression,
        cypher_astnode_t * const *alternatives, unsigned int nalternatives,
        const cypher_astnode_t *deflt, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_filter(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_extract(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *eval,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_reduce(const cypher_astnode_t *accumulator,
        const cypher_astnode_t *init, const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *eval,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_all(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_any(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_single(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_none(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_collection(
        cypher_astnode_t * const *elements, unsigned int nelements,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

/**
 * Get the collection length from a COLLECTION node.
 *
 * If the node is not an instance of `CYPHER_AST_COLLECTION` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @return The length of the collection.
 */
__cypherlang_pure
unsigned int cypher_ast_collection_length(const cypher_astnode_t *node);

/**
 * Get an element from a COLLECTION node.
 *
 * If the node is not an instance of `CYPHER_AST_COLLECTION` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @param [index] The index of the element.
 * @return A pointer to the element of the collection, or `NULL` if there is no
 *         element at the specified index.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_ast_collection_get(const cypher_astnode_t *node,
        unsigned int index);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_map(cypher_astnode_t * const *pairs,
        unsigned int npairs, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_identifier(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_parameter(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_string(const char *s, size_t n,
        struct cypher_input_range range);

/**
 * Get the null terminated string value from a STRING node.
 *
 * If the node is not an instance of `CYPHER_AST_STRING` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @return A pointer to the null terminated string.
 */
__cypherlang_pure
const char *cypher_ast_string_value(const cypher_astnode_t *node);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_integer(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_float(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_true(struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_false(struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_null(struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_label(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_reltype(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_prop_name(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_function_name(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_index_name(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_proc_name(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_pattern(cypher_astnode_t * const *paths,
        unsigned int npaths, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_named_path(const cypher_astnode_t *identifier,
        const cypher_astnode_t *path, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_shortest_path(bool single,
        const cypher_astnode_t *path, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_pattern_path(cypher_astnode_t * const *elements,
        unsigned int nelements, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_node_pattern(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        const cypher_astnode_t *properties, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_rel_pattern(enum cypher_rel_direction direction,
        const cypher_astnode_t *identifier, cypher_astnode_t * const *reltypes,
        unsigned int nreltypes, const cypher_astnode_t *properties,
        cypher_astnode_t *varlength, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_range(const cypher_astnode_t *start,
        const cypher_astnode_t *end, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_command(const cypher_astnode_t *name,
        cypher_astnode_t * const *args, unsigned int nargs,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

/**
 * Get the name from a CYPHER_AST_COMMAND node.
 *
 * If the node is not an instance of `CYPHER_AST_COMMAND` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @return A pointer to the name of the command, which will be of type
 *         `CYPHER_AST_STRING`.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_ast_command_name(const cypher_astnode_t *node);

/**
 * Get the number of arguments from a CYPHER_AST_COMMAND node.
 *
 * If the node is not an instance of `CYPHER_AST_COMMAND` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @return The number of arguments to the command.
 */
__cypherlang_pure
unsigned int cypher_ast_command_narguments(const cypher_astnode_t *node);

/**
 * Get an argument from a CYPHER_AST_COMMAND node.
 *
 * If the node is not an instance of `CYPHER_AST_COMMAND` then the result will
 * be undefined.
 *
 * @param [node] The AST node.
 * @param [index] The index of the argument.
 * @return A pointer to the argument of the command, which will be of type
 *         `CYPHER_AST_STRING`, or `NULL` if there is no argument at the
 *         specified index.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_ast_command_argument(
        const cypher_astnode_t *node, unsigned int index);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_line_comment(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_block_comment(const char *s, size_t n,
        struct cypher_input_range range);

__cypherlang_must_check
cypher_astnode_t *cypher_ast_error(const char *s, size_t n,
        struct cypher_input_range range);

#define CYPHER_AST_RENDER_DEFAULT 0

/**
 * Print a represetation of an AST to a stream.
 *
 * Useful for debugging purposes.
 *
 * @param [ast] The AST node.
 * @param [stream] The stream to print to.
 * @param [width] The width to render, which is advisory and may be exceeded.
 *         A value of 0 implies no restriction on width.
 * @param [colorization] A colorization scheme to apply, or null.
 * @param [flags] A bitmask of flags to control rendering.
 * @return 0 on success, or -1 if an error occurs (errno will be set).
 */
int cypher_ast_fprint(const cypher_astnode_t *ast, FILE *stream,
        unsigned int width,
        const struct cypher_parser_colorization *colorization,
        uint_fast32_t flags);


/*
 * =====================================
 * parser
 * =====================================
 */

/**
 * Configuration for a cypher parser.
 */
typedef struct cypher_parser_config cypher_parser_config_t;

/**
 * Generate a new parser configuration.
 *
 * The returned configuration must be later released using
 * `cypher_parser_config_free(...)`.
 *
 * @return A pointer to a new parser configuration, or `NULL` if an error
 *         occurs (errno will be set).
 */
__cypherlang_must_check
cypher_parser_config_t *cypher_parser_new_config(void);

/**
 * Release a parser configuration.
 *
 * @param [config] A pointer to a parser configuration. This pointer will
 *         be invalid after the function returns.
 */
void cypher_parser_config_free(cypher_parser_config_t *config);

/**
 * Set the initial input position for parsing.
 *
 * By default, the initial position is line 1, column 1 (offset 0). By setting
 * a different position, all ranges in the AST, and positions in any error
 * messages, will be offset by the specified position - as if the input had
 * contained unparsed whitespace up until that point.
 *
 * @param [config] The parser configuration.
 * @param [position] The initial input position.
 */
void cypher_parser_config_set_initial_position(cypher_parser_config_t *config,
        struct cypher_input_position position);

/**
 * Set the initial ordinal for parsing.
 *
 * All AST nodes parsed will be numbered starting from the specified initial
 * ordinal, which is 0 by default. This affects the numbering of AST nodes
 * in rendering.
 *
 * @param [config] The parser configuration.
 * @param [n] The initial oridinal.
 */
void cypher_parser_config_set_initial_ordinal(cypher_parser_config_t *config,
        unsigned int n);


/**
 * Set the colorization scheme for error messages.
 *
 * @param [config] The parser configuration.
 * @param [colorization] The colorization scheme to use.
 */
void cypher_parser_config_set_error_colorization(cypher_parser_config_t *config,
        const struct cypher_parser_colorization *colorization);

/**
 * A parse result.
 */
typedef struct cypher_parse_result cypher_parse_result_t;

/**
 * A parse error.
 */
typedef struct cypher_parse_error cypher_parse_error_t;


#define CYPHER_PARSE_DEFAULT 0
#define CYPHER_PARSE_SINGLE (1<<0)
#define CYPHER_PARSE_ONLY_STATEMENTS (1<<1)

/**
 * @fn cypher_parse_result_t *cypher_parse(const char *s, struct cypher_input_position *last, cypher_parser_config_t *config, uint_fast32_t flags);
 * @brief Parse a command or statement from a string.
 *
 * The first statement or command is parsed from the input string, and a result
 * returned. The result must be passed to `cypher_parse_result_free(...)` to
 * release dynamically allocated memory.
 *
 * @param [s] A null terminated string to parse.
 * @param [last] Either `NULL`, or a pointer to a `struct cypher_input_position`
 *         that will be set position of the last character consumed from the
 *         input.
 * @param [config] Either `NULL`, or a pointer to configuration for the parser.
 * @param [flags] A bitmask of flags to control parsing.
 * @return A pointer to a `cypher_parse_result_t`, or `NULL` if an error occurs
 *         (errno will be set).
 */
#define cypher_parse(s,l,c,f) (cypher_uparse(s,strlen(s),l,c,f))

/**
 * Parse a statement or command from a string.
 *
 * The first statement or command is parsed from the input string, and a result
 * returned. The result must be passed to `cypher_parse_result_free(...)` to
 * release dynamically allocated memory.
 *
 * @param [s] The string to parse.
 * @param [n] The size of the string.
 * @param [last] Either `NULL`, or a pointer to a `struct cypher_input_position`
 *         that will be set position of the last character consumed from the
 *         input.
 * @param [config] Either `NULL`, or a pointer to configuration for the parser.
 * @param [flags] A bitmask of flags to control parsing.
 * @return A pointer to a `cypher_parse_result_t`, or `NULL` if an error occurs
 *         (errno will be set).
 */
__cypherlang_must_check
cypher_parse_result_t *cypher_uparse(const char *s, size_t n,
        struct cypher_input_position *last, cypher_parser_config_t *config,
        uint_fast32_t flags);

/**
 * Parse a statement or command from a stream.
 *
 * The first statement or command is parsed from the input string, and a result
 * returned. The result must be passed to `cypher_parse_result_free(...)` to
 * release dynamically allocated memory.
 *
 * @param [stream] The stream to parse.
 * @param [last] Either `NULL`, or a pointer to a `struct cypher_input_position`
 *         that will be set position of the last character consumed from the
 *         input.
 * @param [config] Either `NULL`, or a pointer to configuration for the parser.
 * @param [flags] A bitmask of flags to control parsing.
 * @return A pointer to a `cypher_parse_result_t`, or `NULL` if an error occurs
 *         (errno will be set).
 */
__cypherlang_must_check
cypher_parse_result_t *cypher_fparse(FILE *stream,
        struct cypher_input_position *last, cypher_parser_config_t *config,
        uint_fast32_t flags);

/**
 * Get the number of elements parsed.
 *
 * @param [result] The parse result.
 * @return The number of elements.
 */
__cypherlang_pure
unsigned int cypher_parse_result_nelements(const cypher_parse_result_t *result);

/**
 * Get a parsed elements from a parse result.
 *
 * @param [result] The parse result.
 * @param [index] The element index.
 * @return A pointer to the AST node for the element, or `NULL` if there is no
 *         element at the specified index.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_parse_result_element(
        const cypher_parse_result_t *result, unsigned int index);

/**
 * Get the total number of astnodes parsed.
 *
 * Includes all children, at any depth, of all result elements.
 *
 * @param [result] The parse result.
 * @return The number of elements.
 */
__cypherlang_pure
unsigned int cypher_parse_result_node_count(const cypher_parse_result_t *result);

/**
 * Get the number of statements or commands parsed.
 *
 * @param [result] The parse result.
 * @return The number of statements or commands parsed.
 */
__cypherlang_pure
unsigned int cypher_parse_result_ndirectives(
        const cypher_parse_result_t *result);

/**
 * Get the AST for a parsed statement or command from a parse result.
 *
 * @param [result] The parse result.
 * @param [index] The directive index.
 * @return Either a CYPHER_AST_STATEMENT, a CYPHER_AST_COMMAND node,
 *         or `NULL` if there is no directive at the specified index.
 */
__cypherlang_pure
const cypher_astnode_t *cypher_parse_result_directive(
        const cypher_parse_result_t *result, unsigned int index);

/**
 * Get the number of errors encountered during parsing.
 *
 * @param [result] The parse result.
 * @return The number of errors.
 */
__cypherlang_pure
unsigned int cypher_parse_result_nerrors(const cypher_parse_result_t *result);

/**
 * Get an error description from a parse result.
 *
 * @param [result] The parse result.
 * @param [index] The error index.
 * @return A pointer to the error description, or `NULL` if there is no value
 *         at the specified index.
 */
__cypherlang_pure
const cypher_parse_error_t *cypher_parse_result_error(
        const cypher_parse_result_t *result, unsigned int index);

/**
 * Get the position of an error.
 *
 * @param [error] The parse error.
 * @return The input position.
 */
__cypherlang_pure
struct cypher_input_position cypher_parse_error_position(
        const cypher_parse_error_t *error);

/**
 * Get the error message of an error.
 *
 * @param [error] The parse error.
 * @return The message.
 */
__cypherlang_pure
const char *cypher_parse_error_message(const cypher_parse_error_t *error);

/**
 * Get the error context of an error.
 *
 * @param [error] The parse error.
 * @return The context string.
 */
__cypherlang_pure
const char *cypher_parse_error_context(const cypher_parse_error_t *error);

/**
 * Get the offset into the an errors context.
 *
 * @param [error] The parse error.
 * @return The offset into the errors context string.
 */
__cypherlang_pure
size_t cypher_parse_error_context_offset(const cypher_parse_error_t *error);

/**
 * Print a represetation of a parse result to a stream.
 *
 * Useful for debugging purposes.
 *
 * @param [result] The parse result.
 * @param [stream] The stream to print to.
 * @param [width] The width to render, which is advisory and may be exceeded.
 *         A value of 0 implies no restriction on width.
 * @param [colorization] A colorization scheme to apply, or null.
 * @param [flags] A bitmask of flags to control rendering.
 * @return 0 on success, or -1 if an error occurs (errno will be set).
 */
int cypher_parse_result_fprint(const cypher_parse_result_t *result,
        FILE *stream, unsigned int width,
        const struct cypher_parser_colorization *colorization,
        uint_fast32_t flags);

/**
 * Free memory associated with a parse result.
 *
 * The result will no longer be valid after this function is invoked.
 *
 * @param [result] The parse result.
 */
void cypher_parse_result_free(cypher_parse_result_t *result);


#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif/*CYPHER_PARSER_H*/
