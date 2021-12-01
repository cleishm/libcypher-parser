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


struct show_fulltext_indexes
{
    cypher_astnode_t _astnode;
};


static cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


static const struct cypher_astnode_vt *parents[] =
    { &cypher_schema_command_astnode_vt };

const struct cypher_astnode_vt cypher_show_fulltext_indexes_astnode_vt =
    { .parents = parents,
      .nparents = 1,
      .name = "SHOW FULLTEXT INDEXES",
      .detailstr = detailstr,
      .release = cypher_astnode_release,
      .clone = clone };


cypher_astnode_t *cypher_ast_show_fulltext_indexes(
        cypher_astnode_t **children, unsigned int nchildren,
	   	struct cypher_input_range range)
{
    struct show_fulltext_indexes *node = calloc(1, sizeof(struct show_fulltext_indexes));
            
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_SHOW_FULLTEXT_INDEXES,
            children, nchildren, range))
    {
        goto cleanup;
    }
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}


cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children)
{
    REQUIRE_TYPE(self, CYPHER_AST_SHOW_FULLTEXT_INDEXES, NULL);

    cypher_astnode_t *clone = cypher_ast_show_fulltext_indexes(children,
		   	self->nchildren, self->range);
    int errsv = errno;
    errno = errsv;
    return clone;
}


ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE_TYPE(self, CYPHER_AST_SHOW_FULLTEXT_INDEXES, -1);

    return 0;
}
