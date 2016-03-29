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


struct rel_pattern
{
    cypher_astnode_t _astnode;
    enum cypher_rel_direction direction;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t **reltypes;
    size_t nreltypes;
    const cypher_astnode_t *varlength;
    const cypher_astnode_t *properties;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void pattern_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_rel_pattern_astnode_vt =
    { .name = "rel pattern",
      .detailstr = detailstr,
      .free = pattern_free };


cypher_astnode_t *cypher_ast_rel_pattern(enum cypher_rel_direction direction,
        const cypher_astnode_t *identifier, cypher_astnode_t * const *reltypes,
        unsigned int nreltypes, const cypher_astnode_t *properties,
        cypher_astnode_t *varlength, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(nreltypes == 0 || reltypes != NULL, NULL);
    struct rel_pattern *node = calloc(1, sizeof(struct rel_pattern));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_REL_PATTERN,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->direction = direction;
    node->identifier = identifier;
    if (nreltypes > 0)
    {
        node->reltypes = mdup(reltypes, nreltypes * sizeof(cypher_astnode_t *));
        if (node->reltypes == NULL)
        {
            goto cleanup;
        }
        node->nreltypes = nreltypes;
    }
    node->varlength = varlength;
    node->properties = properties;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void pattern_free(cypher_astnode_t *self)
{
    struct rel_pattern *node = container_of(self, struct rel_pattern, _astnode);
    free(node->reltypes);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_REL_PATTERN), -1);
    struct rel_pattern *node = container_of(self, struct rel_pattern, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "%s-[",
            (node->direction == CYPHER_REL_INBOUND)? "<" : "");
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->identifier != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "@%u",
                node->identifier->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    for (unsigned int i = 0; i < node->nreltypes; ++i)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "%s@%u",
                (i == 0)? ":" : "|:", node->reltypes[i]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->varlength != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "*@%u",
                node->varlength->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->properties != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, " {@%u}",
                node->properties->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    r = snprintf(str+n, (n < size)? size-n : 0, "]-%s",
            (node->direction == CYPHER_REL_OUTBOUND)? ">" : "");
    if (r < 0)
    {
        return -1;
    }
    n += r;

    return n;
}
