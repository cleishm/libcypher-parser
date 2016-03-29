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


struct comparison
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *left;
    const cypher_operator_t **ops;
    const cypher_astnode_t **args;
    unsigned int length;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void comparison_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_comparison_astnode_vt =
    { .name = "comparison",
      .detailstr = detailstr,
      .free = comparison_free };


cypher_astnode_t *cypher_ast_comparison(const cypher_astnode_t *left,
        const cypher_operator_t * const *ops, cypher_astnode_t * const *args,
        unsigned int length, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(left != NULL, NULL);
    REQUIRE(length > 0, NULL);
    REQUIRE(ops != NULL, NULL);
    REQUIRE(args != NULL, NULL);

    struct comparison *node = calloc(1, sizeof(struct comparison));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_COMPARISON,
            children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    node->left = left;
    node->ops = mdup(ops, length * sizeof(cypher_astnode_t *));
    if (node->ops == NULL)
    {
        goto cleanup;
    }
    node->args = mdup(args, length * sizeof(cypher_astnode_t *));
    if (node->args == NULL)
    {
        goto cleanup;
    }
    node->length = length;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node->ops);
    free(node);
    errno = errsv;
    return NULL;
}


void comparison_free(cypher_astnode_t *self)
{
    struct comparison *node = container_of(self, struct comparison, _astnode);
    free(node->ops);
    free(node->args);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_COMPARISON), -1);
    struct comparison *node = container_of(self, struct comparison, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "@%u", node->left->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    for (unsigned int l = 0; l < node->length; ++l)
    {
        r = snprintf(str + n, (n < size)? size-n : 0, " %s @%u",
                node->ops[l]->str, node->args[l]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
