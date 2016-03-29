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


struct rel_id_lookup
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t **ids;
    unsigned int nids;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void rel_id_lookup(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_rel_id_lookup_astnode_vt =
    { .name = "rel id lookup",
      .detailstr = detailstr,
      .free = rel_id_lookup };


cypher_astnode_t *cypher_ast_rel_id_lookup(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *ids, unsigned int nids,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(identifier, CYPHER_AST_IDENTIFIER), NULL);
    REQUIRE(nids > 0, NULL);
    REQUIRE(ids != NULL, NULL);

    struct rel_id_lookup *node = calloc(1, sizeof(struct rel_id_lookup));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_REL_ID_LOOKUP,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->identifier = identifier;
    node->ids = mdup(ids, nids * sizeof(cypher_astnode_t *));
    if (node->ids == NULL)
    {
        goto cleanup;
    }
    node->nids = nids;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void rel_id_lookup(cypher_astnode_t *self)
{
    struct rel_id_lookup *node =
            container_of(self, struct rel_id_lookup, _astnode);
    free(node->ids);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_REL_ID_LOOKUP), -1);
    struct rel_id_lookup *node =
            container_of(self, struct rel_id_lookup, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "@%u = rel(", node->identifier->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    for (unsigned int i = 0; i < node->nids; ++i)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "%s@%u",
                (i > 0)? ", " : "", node->ids[i]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if ((n+1) < size)
    {
        str[n] = ')';
        str[size-1] = '\0';
    }
    ++n;
    return n;
}
