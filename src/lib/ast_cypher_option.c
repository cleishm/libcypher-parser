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


struct cypher_option
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *version;
    const cypher_astnode_t **params;
    unsigned int nparams;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void cypher_option_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_cypher_option_astnode_vt =
    { .name = "CYPHER",
      .detailstr = detailstr,
      .free = cypher_option_free };


cypher_astnode_t *cypher_ast_cypher_option(const cypher_astnode_t *version,
        cypher_astnode_t * const *params, unsigned int nparams,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(version == NULL ||
            cypher_astnode_instanceof(version, CYPHER_AST_STRING), NULL);
    REQUIRE(nparams == 0 || params != NULL, NULL);

    struct cypher_option *node = calloc(1, sizeof(struct cypher_option));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_CYPHER_OPTION,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->version = version;

    if (nparams > 0)
    {
        node->params = mdup(params, nparams * sizeof(cypher_astnode_t *));
        if (node->params == NULL)
        {
            goto cleanup;
        }
        node->nparams = nparams;
    }
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void cypher_option_free(cypher_astnode_t *self)
{
    struct cypher_option *node =
        container_of(self, struct cypher_option, _astnode);
    free(node->params);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_CYPHER_OPTION), -1);
    struct cypher_option *node =
        container_of(self, struct cypher_option, _astnode);

    size_t n = 0;
    ssize_t r;
    if (node->version != NULL)
    {
        r = snprintf(str, size, "version=@%u", node->version->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->nparams > 0)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "%sparams=",
                (node->version != NULL)? ", ":"");
        if (r < 0)
        {
            return -1;
        }
        n += r;

        r = snprint_sequence(str+n, (n < size)? size-n : 0, node->params,
                node->nparams);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    return n;
}
