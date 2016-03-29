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


struct apply_operator
{
    cypher_astnode_t _astnode;
    bool distinct;
    const cypher_astnode_t *func;
    const cypher_astnode_t **args;
    unsigned int nargs;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void apply_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_apply_operator_astnode_vt =
    { .name = "apply",
      .detailstr = detailstr,
      .free = apply_free };


cypher_astnode_t *cypher_ast_apply_operator(const cypher_astnode_t *func,
        bool distinct, cypher_astnode_t * const *args, unsigned int nargs,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(func, CYPHER_AST_FUNCTION_NAME), NULL);
    REQUIRE(nargs == 0 || args != NULL, NULL);

    struct apply_operator *node = calloc(1, sizeof(struct apply_operator));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_APPLY_OPERATOR,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->distinct = distinct;
    node->func = func;

    if (nargs > 0)
    {
        node->args = mdup(args, nargs * sizeof(cypher_astnode_t *));
        if (node->args == NULL)
        {
            goto cleanup;
        }
        node->nargs = nargs;
    }
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void apply_free(cypher_astnode_t *self)
{
    struct apply_operator *node =
        container_of(self, struct apply_operator, _astnode);
    free(node->args);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_APPLY_OPERATOR), -1);
    struct apply_operator *node =
        container_of(self, struct apply_operator, _astnode);

    ssize_t r = snprintf(str, size, "@%u(%s", node->func->ordinal,
            node->distinct? "DISTINCT " : "");
    if (r < 0)
    {
        return -1;
    }
    size_t n = r;

    for (unsigned int i = 0; i < node->nargs; )
    {
        ssize_t r = snprintf(str+n, (n < size)? size-n : 0,
                "@%u", node->args[i]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
        if (++i < node->nargs)
        {
            if (n < size)
            {
                str[n] = ',';
            }
            n++;
            if (n < size)
            {
                str[n] = ' ';
            }
            n++;
        }
    }

    if (n+1 < size)
    {
        str[n] = ')';
    }
    n++;
    return n;
}
