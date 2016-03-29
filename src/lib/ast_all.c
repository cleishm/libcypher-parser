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


struct all
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t *expression;
    const cypher_astnode_t *predicate;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_all_astnode_vt =
    { .name = "all",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_all(const cypher_astnode_t *identifier,
        const cypher_astnode_t *expression, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(identifier, CYPHER_AST_IDENTIFIER), NULL);
    REQUIRE(expression != NULL, NULL);

    struct all *node = calloc(1, sizeof(struct all));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_ALL,
            children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    node->identifier = identifier;
    node->expression = expression;
    node->predicate = predicate;
    return &(node->_astnode);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_ALL), -1);
    struct all *node = container_of(self, struct all, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "[@%u IN @%u",
            node->identifier->ordinal,
            node->expression->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->predicate != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, " WHERE @%u",
                node->predicate->ordinal);
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
