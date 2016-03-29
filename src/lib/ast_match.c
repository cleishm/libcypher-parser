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


struct match
{
    cypher_astnode_t _astnode;
    bool optional;
    const cypher_astnode_t *pattern;
    const cypher_astnode_t **hints;
    unsigned int nhints;
    const cypher_astnode_t *predicate;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void match_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_match_astnode_vt =
    { .name = "MATCH",
      .detailstr = detailstr,
      .free = match_free };


cypher_astnode_t *cypher_ast_match(bool optional,
        const cypher_astnode_t *pattern, cypher_astnode_t * const *hints,
        unsigned int nhints, const cypher_astnode_t *predicate,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    REQUIRE(cypher_astnode_instanceof(pattern, CYPHER_AST_PATTERN), NULL);
    REQUIRE(nhints == 0 || hints != NULL, NULL);

    struct match *node = calloc(1, sizeof(struct match));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_MATCH,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->optional = optional;
    node->pattern = pattern;
    if (nhints > 0)
    {
        node->hints = mdup(hints, nhints * sizeof(cypher_astnode_t *));
        if (node->hints == NULL)
        {
            goto cleanup;
        }
        node->nhints = nhints;
    }
    node->predicate = predicate;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void match_free(cypher_astnode_t *self)
{
    struct match *node = container_of(self, struct match, _astnode);
    free(node->hints);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_MATCH), -1);
    struct match *node = container_of(self, struct match, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "%spattern=@%d",
            node->optional? "OPTIONAL, " : "", node->pattern->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->nhints > 0)
    {
        strncpy(str + n, ", hints=", (n < size)? size-n : 0);
        if (size > 0)
        {
            str[size-1] = '\0';
        }
        n += 8;

        r = snprint_sequence(str+n, (n < size)? size-n : 0,
                node->hints, node->nhints);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    if (node->predicate != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, ", where=@%u",
                node->predicate->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
