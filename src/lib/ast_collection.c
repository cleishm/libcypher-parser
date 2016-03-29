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


struct collection
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t **elements;
    size_t nelements;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void collection_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_collection_astnode_vt =
    { .name = "collection",
      .detailstr = detailstr,
      .free = collection_free };


cypher_astnode_t *cypher_ast_collection(
        cypher_astnode_t * const *elements, unsigned int nelements,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(nelements == 0 || elements != NULL, NULL);

    struct collection *node = calloc(1, sizeof(struct collection));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_COLLECTION,
                children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    if (nelements > 0)
    {
        node->elements = mdup(elements, nelements * sizeof(cypher_astnode_t *));
        if (node->elements == NULL)
        {
            free(node);
            return NULL;
        }
        node->nelements = nelements;
    }
    return &(node->_astnode);
}


void collection_free(cypher_astnode_t *self)
{
    struct collection *node = container_of(self, struct collection, _astnode);
    free(node->elements);
    cypher_astnode_free(self);
}


unsigned int cypher_ast_collection_length(const cypher_astnode_t *astnode)
{
    REQUIRE(cypher_astnode_instanceof(astnode, CYPHER_AST_COLLECTION), 0);
    struct collection *node =
            container_of(astnode, struct collection, _astnode);
    return node->nelements;
}


const cypher_astnode_t *cypher_ast_collection_get(
        const cypher_astnode_t *astnode, unsigned int index)
{
    REQUIRE(cypher_astnode_instanceof(astnode, CYPHER_AST_COLLECTION), NULL);
    struct collection *node =
            container_of(astnode, struct collection, _astnode);

    if (index >= node->nelements)
    {
        return NULL;
    }
    return node->elements[index];
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_COLLECTION), -1);
    struct collection *node = container_of(self, struct collection, _astnode);

    return snprint_sequence(str, size, node->elements, node->nelements);
}
