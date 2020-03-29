#include "astnode.h"

struct path_pattern_any
{
    cypher_astnode_t _astnode;
};

static cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_path_pattern_any_astnode_vt =
        { .name = "path pattern any",
          .detailstr = detailstr,
          .release = cypher_astnode_release,
          .clone = clone };


cypher_astnode_t *cypher_ast_path_pattern_any(cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    struct path_pattern_any *node = calloc(1, sizeof(struct path_pattern_any));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATH_PATTERN_ANY,
            NULL, 0, range))
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
    REQUIRE_TYPE(self, CYPHER_AST_PATH_PATTERN_ANY, NULL);
    cypher_astnode_t *clone = cypher_ast_path_pattern_any(children, self->nchildren, self->range);

    int errsv = errno;
    errno = errsv;
    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size) {
    return snprintf(str, size, "-");
}
