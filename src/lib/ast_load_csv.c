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


struct loadcsv
{
    cypher_astnode_t _astnode;
    bool with_headers;
    const cypher_astnode_t *url;
    const cypher_astnode_t *identifier;
    const cypher_astnode_t *field_terminator;
};


static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_load_csv_astnode_vt =
    { .name = "LOAD CSV",
      .detailstr = detailstr,
      .free = cypher_astnode_free };


cypher_astnode_t *cypher_ast_load_csv(bool with_headers,
        const cypher_astnode_t *url, const cypher_astnode_t *identifier,
        const cypher_astnode_t *field_terminator, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    REQUIRE(url != NULL, NULL);
    REQUIRE(cypher_astnode_instanceof(identifier, CYPHER_AST_IDENTIFIER), NULL);
    REQUIRE(nchildren == 0 || children != NULL, NULL);

    struct loadcsv *node = calloc(1, sizeof(struct loadcsv));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_LOAD_CSV,
            children, nchildren, range))
    {
        free(node);
        return NULL;
    }
    node->with_headers = with_headers;
    node->url = url;
    node->identifier = identifier;
    node->field_terminator = field_terminator;
    return &(node->_astnode);
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE(cypher_astnode_instanceof(self, CYPHER_AST_LOAD_CSV), -1);
    struct loadcsv *node = container_of(self, struct loadcsv, _astnode);

    size_t n = 0;
    ssize_t r = snprintf(str, size, "%surl=@%u, identifier=@%u",
                node->with_headers? "WITH HEADERS, " : "",
                node->url->ordinal, node->identifier->ordinal);
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->field_terminator != NULL)
    {
        r = snprintf(str + n, (n < size)? size-n : 0,
                ", field_terminator=@%u", node->field_terminator->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}
