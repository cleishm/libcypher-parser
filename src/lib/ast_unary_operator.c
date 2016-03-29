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
#include "operators.h"
#include "util.h"
#include <assert.h>


struct unary_operator
{
    cypher_astnode_t _astnode;
    const cypher_operator_t *op;
    const cypher_astnode_t *arg;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_unary_operator_astnode_vt =
    { .name = "unary operator",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_unary_operator(const cypher_operator_t *op,
        const cypher_astnode_t *arg, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(op != NULL, NULL);
    REQUIRE(arg != NULL, NULL);

    struct unary_operator *node = calloc(1, sizeof(struct unary_operator));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_UNARY_OPERATOR,
            children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    node->op = op;
    node->arg = arg;
    return &(node->_astnode);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_UNARY_OPERATOR), -1);
    struct unary_operator *node =
        container_of(self, struct unary_operator, _astnode);
    return snprintf(str, size, "%s @%u", node->op->str, node->arg->ordinal);
}
