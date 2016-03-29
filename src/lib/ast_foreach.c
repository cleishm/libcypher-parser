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
#include "astnode.h"
#include "util.h"
#include <assert.h>


struct foreach_clause
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t *expression;
    const cypher_astnode_t **clauses;
    unsigned int nclauses;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void foreach_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_foreach_astnode_vt =
    { .name = "FOREACH",
      .detailstr = detailstr,
      .free = foreach_free };


cypher_astnode_t *cypher_ast_foreach(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, cypher_astnode_t * const *clauses,
        unsigned int nclauses, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(identifier, CYPHER_AST_IDENTIFIER), NULL);
    REQUIRE(expression != NULL, NULL);
    REQUIRE(nclauses > 0, NULL);
    REQUIRE(clauses != NULL, NULL);

    struct foreach_clause *node = calloc(1, sizeof(struct foreach_clause));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_FOREACH,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->identifier = identifier;
    node->expression = expression;
    node->clauses = mdup(clauses, nclauses * sizeof(cypher_astnode_t *));
    if (node->clauses == NULL)
    {
        goto cleanup;
    }
    node->nclauses = nclauses;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void foreach_free(cypher_astnode_t *self)
{
    struct foreach_clause *node =
            container_of(self, struct foreach_clause, _astnode);
    free(node->clauses);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_FOREACH), -1);
    struct foreach_clause *node =
            container_of(self, struct foreach_clause, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "[@%u IN @%u | ",
            node->identifier->ordinal, node->expression->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    for (unsigned int i = 0; i < node->nclauses; ++i)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "%s@%u",
                (i > 0)? ", " : "", node->clauses[i]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (++n < size)
    {
        str[n-1] = ']';
        str[n] = '\0';
    }

    return n;
}
