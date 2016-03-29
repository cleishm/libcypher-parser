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


struct node_pattern
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t **labels;
    size_t nlabels;
    const cypher_astnode_t *properties;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void pattern_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_node_pattern_astnode_vt =
    { .name = "node pattern",
      .detailstr = detailstr,
      .free = pattern_free };


cypher_astnode_t *cypher_ast_node_pattern(const cypher_astnode_t *identifier,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        const cypher_astnode_t *properties, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(nlabels == 0 || labels != NULL, NULL);

    struct node_pattern *node = calloc(1, sizeof(struct node_pattern));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_NODE_PATTERN,
                children, nchildren, range))
    {
        goto cleanup;
    }
    node->identifier = identifier;
    if (nlabels > 0)
    {
        node->labels = mdup(labels, nlabels * sizeof(cypher_astnode_t *));
        if (node->labels == NULL)
        {
            goto cleanup;
        }
        node->nlabels = nlabels;
    }
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
    struct node_pattern *node =
            container_of(self, struct node_pattern, _astnode);
    free(node->labels);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_NODE_PATTERN), -1);
    struct node_pattern *node =
            container_of(self, struct node_pattern, _astnode);

    size_t n = 0;
    if (n < size)
    {
        str[n] = '(';
    }
    n++;

    ssize_t r;
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

    for (unsigned int i = 0; i < node->nlabels; ++i)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, ":@%u",
                node->labels[i]->ordinal);
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

    if (n < size)
    {
        str[n] = ')';
    }
    n++;

    return n;
}
