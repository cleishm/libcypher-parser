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


struct statement
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t **options;
    unsigned int noptions;
    const cypher_astnode_t *body;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void statement_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_statement_astnode_vt =
    { .name = "statement",
      .detailstr = detailstr,
      .free = statement_free };


cypher_astnode_t *cypher_ast_statement(cypher_astnode_t * const *options,
        unsigned int noptions, const cypher_astnode_t *body,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(noptions == 0 || options != NULL, NULL);

    struct statement *node = calloc(1, sizeof(struct statement));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_STATEMENT,
            children, nchildren, range))
    {
        goto cleanup;
    }
    if (noptions > 0)
    {
        node->options = mdup(options, noptions * sizeof(cypher_astnode_t *));
        if (node->options == NULL)
        {
            goto cleanup;
        }
        node->noptions = noptions;
    }
    node->body = body;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void statement_free(cypher_astnode_t *self)
{
    struct statement *node = container_of(self, struct statement, _astnode);
    free(node->options);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_STATEMENT), -1);
    struct statement *node = container_of(self, struct statement, _astnode);

    size_t n = 0;

    ssize_t r;
    if (node->noptions > 0)
    {
        if (n < size)
        {
            strncpy(str + n, "options=", size - n);
            str[size-1] = '\0';
        }
        n += 8;

        r = snprint_sequence(str + 8, (size > 8)? size-8 : 0,
                node->options, node->noptions);
        if (r < 0)
        {
            return -1;
        }
        n += r;

        if (n < size)
        {
            strncpy(str + n, ", ", size - n);
            str[size-1] = '\0';
        }
        n += 2;
    }

    r = snprintf(str + n, (n < size)? size-n : 0, "body=@%u",
            node->body->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    return n;
}
