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


struct case_expression
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *expression;
    const cypher_astnode_t **alternatives;
    unsigned int nalternatives;
    const cypher_astnode_t *deflt;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);
static void case_expression_free(cypher_astnode_t *self);


const struct cypher_astnode_vt cypher_case_astnode_vt =
    { .name = "case",
      .detailstr = detailstr,
      .free = case_expression_free };


cypher_astnode_t *cypher_ast_case(const cypher_astnode_t *expression,
        cypher_astnode_t * const *alternatives, unsigned int nalternatives,
        const cypher_astnode_t *deflt, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(nalternatives > 0, NULL);
    REQUIRE(nalternatives % 2 == 0, NULL);
    REQUIRE(alternatives != NULL, NULL);

    struct case_expression *node = calloc(1, sizeof(struct case_expression));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_CASE,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->expression = expression;
    node->alternatives = mdup(alternatives,
            nalternatives * sizeof(cypher_astnode_t *));
    if (node->alternatives == NULL)
    {
        goto cleanup;
    }
    node->nalternatives = nalternatives;
    node->deflt = deflt;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


void case_expression_free(cypher_astnode_t *self)
{
    struct case_expression *node =
            container_of(self, struct case_expression, _astnode);
    free(node->alternatives);
    cypher_astnode_free(self);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_CASE), -1);
    struct case_expression *node =
            container_of(self, struct case_expression, _astnode);

    ssize_t r;
    size_t n = 0;
    if (node->expression != NULL)
    {
        r = snprintf(str, size, "expression=@%u, ",
                node->expression->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    r = snprintf(str+n, (n < size)? size-n : 0, "alternatives=");
    if (r < 0)
    {
        return -1;
    }
    n += r;
    if (n < size)
    {
        str[n] = '[';
    }
    n++;
    for (unsigned int i = 0; i < node->nalternatives; )
    {
        ssize_t r = snprintf(str+n, (n < size)? size-n : 0,
                "(@%u:@%u)", node->alternatives[i]->ordinal,
                node->alternatives[i+1]->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
        i += 2;
        if (i < node->nalternatives)
        {
            if (n < size)
            {
                str[n] = ',';
            }
            n++;
            if (n < size)
            {
                str[n] = ' ';
            }
            n++;
        }
    }
    if (n < size)
    {
        str[n] = ']';
    }
    n++;

    if (node->deflt != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, ", default=@%u",
                node->deflt->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
