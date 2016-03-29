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


struct apply_all_operator
{
    cypher_astnode_t _astnode;
    bool distinct;
    const cypher_astnode_t *func;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_apply_all_operator_astnode_vt =
    { .name = "apply all",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_apply_all_operator(const cypher_astnode_t *func,
        bool distinct, cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(func, CYPHER_AST_FUNCTION_NAME), NULL);

    struct apply_all_operator *node =
            calloc(1, sizeof(struct apply_all_operator));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_APPLY_ALL_OPERATOR,
                children, nchildren, range))
    {
        goto cleanup;
    }
    node->distinct = distinct;
    node->func = func;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self,
                CYPHER_AST_APPLY_ALL_OPERATOR), -1);
    struct apply_all_operator *node =
        container_of(self, struct apply_all_operator, _astnode);

    return snprintf(str, size, "@%u(%s*)", node->func->ordinal,
            node->distinct? "DISTINCT " : "");
}
