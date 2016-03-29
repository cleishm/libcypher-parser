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


struct return_clause
{
    cypher_astnode_t _astnode;
    bool distinct;
    bool include_existing;
    const cypher_astnode_t **items;
    unsigned int nitems;
    const cypher_astnode_t *order_by;
    const cypher_astnode_t *skip;
    const cypher_astnode_t *limit;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void return_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_return_astnode_vt =
    { .name = "RETURN",
      .detailstr = detailstr,
      .free = return_free };


cypher_astnode_t *cypher_ast_return(bool distinct, bool include_existing,
        cypher_astnode_t * const *items, unsigned int nitems,
        const cypher_astnode_t *order_by, const cypher_astnode_t *skip,
        const cypher_astnode_t *limit, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(nitems == 0 || items != NULL, NULL);

    struct return_clause *node = calloc(1, sizeof(struct return_clause));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_RETURN,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->distinct = distinct;
    node->include_existing = include_existing;
    if (nitems > 0)
    {
        node->items = mdup(items, nitems * sizeof(cypher_astnode_t *));
        if (node->items == NULL)
        {
            goto cleanup;
        }
        node->nitems = nitems;
    }
    node->order_by = order_by;
    node->skip = skip;
    node->limit = limit;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void return_free(cypher_astnode_t *self)
{
    struct return_clause *node =
            container_of(self, struct return_clause, _astnode);
    free(node->items);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_RETURN), -1);
    struct return_clause *node =
            container_of(self, struct return_clause, _astnode);

    ssize_t r = snprintf(str, size, "%s%s%s%sitems=",
            node->distinct? "DISTINCT":"",
            (node->distinct && node->include_existing)? ", ":"",
            node->include_existing? "*":"",
            (node->distinct || node->include_existing)? ", ":"");
    if (r < 0)
    {
        return -1;
    }
    size_t n = r;
    r = snprint_sequence(str + n, (n < size)? size-n : 0,
            node->items, node->nitems);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->order_by != NULL)
    {
        r = snprintf(str + n, (n < size)? size-n : 0, ", ORDER BY=@%u",
                node->order_by->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->skip != NULL)
    {
        r = snprintf(str + n, (n < size)? size-n : 0, ", SKIP=@%u",
                node->skip->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->limit != NULL)
    {
        r = snprintf(str + n, (n < size)? size-n : 0, ", LIMIT=@%u",
                node->limit->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    return n;
}
