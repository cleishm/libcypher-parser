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


struct map
{
    cypher_astnode_t _astnode;
    size_t npairs;
    const cypher_astnode_t *pairs[];
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


static const struct cypher_astnode_vt *parents[] =
    { &cypher_expression_astnode_vt };

const struct cypher_astnode_vt cypher_map_astnode_vt =
    { .parents = parents,
      .nparents = 1,
      .name = "map",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_map(cypher_astnode_t * const *pairs,
        unsigned int npairs, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(npairs % 2 == 0, NULL);
    for (unsigned int i = 0; i < npairs; i+=2)
    {
        REQUIRE_TYPE(pairs[i], CYPHER_AST_PROP_NAME, NULL);
        REQUIRE_TYPE(pairs[i+1], CYPHER_AST_EXPRESSION, NULL);
    }

    struct map *node = calloc(1, sizeof(struct map) +
            npairs * sizeof(cypher_astnode_t *));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_MAP,
                children, nchildren, range))
    {
        goto cleanup;
    }
    memcpy(node->pairs, pairs, npairs * sizeof(cypher_astnode_t *));
    node->npairs = npairs;
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
    REQUIRE_TYPE(self, CYPHER_AST_MAP, -1);
    struct map *node = container_of(self, struct map, _astnode);

    size_t n = 0;
    if (n < size)
    {
        str[n] = '{';
    }
    n++;
    for (unsigned int i = 0; i < node->npairs; )
    {
        ssize_t r = snprintf(str+n, (n < size)? size-n : 0,
                "%s@%u:@%u", (i > 0)? ", ":"",
                node->pairs[i]->ordinal, node->pairs[i+1]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
        i += 2;
    }
    if (n < size)
    {
        str[n] = '}';
    }
    n++;
    return n;
}
