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


struct start
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t **points;
    unsigned int npoints;
    const cypher_astnode_t *predicate;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void start_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_start_astnode_vt =
    { .name = "START",
      .detailstr = detailstr,
      .free = start_free };


cypher_astnode_t *cypher_ast_start(cypher_astnode_t * const *points,
        unsigned int npoints, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(npoints > 0, NULL);
    REQUIRE(points != NULL, NULL);

    struct start *node = calloc(1, sizeof(struct start));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_START,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->points = mdup(points, npoints * sizeof(cypher_astnode_t *));
    if (node->points == NULL)
    {
        goto cleanup;
    }
    node->npoints = npoints;
    node->predicate = predicate;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void start_free(cypher_astnode_t *self)
{
    struct start *node = container_of(self, struct start, _astnode);
    free(node->points);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_START), -1);
    struct start *node = container_of(self, struct start, _astnode);

    size_t n = 0;
    if (n < size)
    {
        strncpy(str+n, "points=", size-n);
        str[size-1] = '\0';
    }
    n += 7;

    ssize_t r = snprint_sequence(str+n, (n < size)? size-n : 0,
            node->points, node->npoints);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->predicate != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "WHERE=@%u",
                node->predicate->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    return n;
}
