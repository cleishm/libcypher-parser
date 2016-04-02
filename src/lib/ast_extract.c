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


struct extract
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t *expression;
    const cypher_astnode_t *eval;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


static const struct cypher_astnode_vt *parents[] =
    { &cypher_expression_astnode_vt };

const struct cypher_astnode_vt cypher_extract_astnode_vt =
    { .parents = parents,
      .nparents = 1,
      .name = "extract",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_extract(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *eval,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE_TYPE(identifier, CYPHER_AST_IDENTIFIER, NULL);
    REQUIRE_TYPE(expression, CYPHER_AST_EXPRESSION, NULL);
    REQUIRE_TYPE_OPTIONAL(eval, CYPHER_AST_EXPRESSION, NULL);

    struct extract *node = calloc(1, sizeof(struct extract));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_EXTRACT,
            children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    node->identifier = identifier;
    node->expression = expression;
    node->eval = eval;
    return &(node->_astnode);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE_TYPE(self, CYPHER_AST_EXTRACT, -1);
    struct extract *node = container_of(self, struct extract, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "[@%u IN @%u",
            node->identifier->ordinal,
            node->expression->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->eval != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, " | @%u",
                node->eval->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (n+1 < size)
    {
        str[n] = ']';
        str[n+1] = '\0';
    }

    return n+1;
}
