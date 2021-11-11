#include "../../config.h"
#include "astnode.h"
#include "operators.h"
#include "util.h"
#include <assert.h>


struct range_optional
{
    cypher_astnode_t _astnode;
};


static cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_range_optional_astnode_vt =
    { .name = "optional",
      .detailstr = detailstr,
      .release = cypher_astnode_release,
      .clone = clone };


cypher_astnode_t *cypher_ast_range_optional(cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    struct range_optional *node = calloc(1, sizeof(struct range_optional));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_RANGE_OPTIONAL,
            NULL, 0, range))
    {
        free(node);
        return NULL;
    }
    return &(node->_astnode);
}


cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children)
{
    REQUIRE_TYPE(self, CYPHER_AST_RANGE_OPTIONAL, NULL);
    cypher_astnode_t *clone = cypher_ast_range_optional(children, self->nchildren, self->range);

    int errsv = errno;
    errno = errsv;
    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size)
{
    REQUIRE_TYPE(self, CYPHER_AST_RANGE_OPTIONAL, -1);
    return snprintf(str, size, "?");
}
