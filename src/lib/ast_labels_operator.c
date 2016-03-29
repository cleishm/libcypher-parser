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


struct labels_operator
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *expression;
    const cypher_astnode_t **labels;
    unsigned int nlabels;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void labels_operator_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_labels_operator_astnode_vt =
    { .name = "has labels",
      .detailstr = detailstr,
      .free = labels_operator_free };


cypher_astnode_t *cypher_ast_labels_operator(const cypher_astnode_t *expression,
        cypher_astnode_t * const *labels, unsigned int nlabels,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(expression != NULL, NULL);
    REQUIRE(nlabels > 0, NULL);
    REQUIRE(labels != NULL, NULL);

    struct labels_operator *node = calloc(1, sizeof(struct labels_operator));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_LABELS_OPERATOR,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->expression = expression;
    node->labels = mdup(labels, nlabels * sizeof(cypher_astnode_t *));
    if (node->labels == NULL)
    {
        goto cleanup;
    }
    node->nlabels = nlabels;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void labels_operator_free(cypher_astnode_t *self)
{
    struct labels_operator *node =
        container_of(self, struct labels_operator, _astnode);
    free(node->labels);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_LABELS_OPERATOR), -1);
    struct labels_operator *node =
        container_of(self, struct labels_operator, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "@%u", node->expression->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    for (unsigned int l = 0; l < node->nlabels; ++l)
    {
        r = snprintf(str + n, (n < size)? size-n : 0, ":@%u",
                node->labels[l]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
