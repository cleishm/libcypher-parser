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


struct pattern_path
{
    cypher_astnode_t _astnode;
    size_t nelements;
    const cypher_astnode_t *elements[];
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


static const struct cypher_astnode_vt *parents[] =
    { &cypher_expression_astnode_vt };

const struct cypher_astnode_vt cypher_pattern_path_astnode_vt =
    { .parents = parents,
      .nparents = 1,
      .name = "pattern path",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_pattern_path(cypher_astnode_t * const *elements,
        unsigned int nelements, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(nelements % 2 == 1, NULL);
    REQUIRE(elements != NULL, NULL);
    for (unsigned int i = 0; i < nelements; ++i)
    {
        REQUIRE_TYPE(elements[i], (i%2 == 0)? CYPHER_AST_NODE_PATTERN :
                CYPHER_AST_REL_PATTERN, NULL);
    }

    struct pattern_path *node = calloc(1, sizeof(struct pattern_path) +
            nelements * sizeof(cypher_astnode_t *));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATTERN_PATH,
                children, nchildren, range))
    {
        goto cleanup;
    }
    memcpy(node->elements, elements, nelements * sizeof(cypher_astnode_t *));
    node->nelements = nelements;
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
    REQUIRE_TYPE(self, CYPHER_AST_PATTERN_PATH, -1);
    struct pattern_path *node =
            container_of(self, struct pattern_path, _astnode);
    assert(node->nelements % 2 == 1);

    size_t n = 0;
    for (unsigned int i = 0; i < node->nelements; ++i)
    {
        ssize_t r;
        if (i % 2 == 0)
        {
            r = snprintf(str+n, (n < size)? size-n : 0, "(@%u)",
                    node->elements[i]->ordinal);
        }
        else
        {
            r = snprintf(str+n, (n < size)? size-n : 0, "-[@%u]-",
                    node->elements[i]->ordinal);
        }
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
