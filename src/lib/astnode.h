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
#ifndef CYPHER_PARSER_ASTNODE_H
#define CYPHER_PARSER_ASTNODE_H

#include "cypher-parser.h"
#include "ast.h"
#include "util.h"


struct cypher_astnode_vt
{
    const struct cypher_astnode_vt **parents;
    unsigned int nparents;
    const char *name;
    ssize_t (*detailstr)(const cypher_astnode_t *self, char *str, size_t size);
    void (*free)(cypher_astnode_t *self);
};


struct cypher_astnode
{
    cypher_astnode_type_t type;
    cypher_astnode_t **children;
    unsigned int nchildren;
    struct cypher_input_range range;
    unsigned int ordinal;
};


int cypher_astnode_init(cypher_astnode_t *node, cypher_astnode_type_t type,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range);

void cypher_astnode_free(cypher_astnode_t *node);

ssize_t cypher_astnode_detailstr(const cypher_astnode_t *node, char *str,
        size_t size);


#define REQUIRE_TYPE(ptr, type, res) \
        REQUIRE(cypher_astnode_instanceof(ptr, type), res)

#define REQUIRE_TYPE_OPTIONAL(ptr, type, res) \
        REQUIRE(ptr == NULL || cypher_astnode_instanceof(ptr, type), res)

#define REQUIRE_TYPE_ALL(collection, size, type, res) \
    do { \
        REQUIRE((size == 0) || (collection != NULL), res); \
        unsigned int i = size; \
        while (i-- > 0) { \
            REQUIRE(cypher_astnode_instanceof(collection[i], type), res); \
        } \
    } while(0)


#endif/*CYPHER_PARSER_ASTNODE_H*/
