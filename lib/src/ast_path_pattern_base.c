#include "astnode.h"

struct path_pattern_base {
    cypher_astnode_t _astnode;
    enum cypher_rel_direction direction;
    const cypher_astnode_t *varlength;
    const cypher_astnode_t *path_base;
};

static cypher_astnode_t *clone(const cypher_astnode_t *self,
                               cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);

const struct cypher_astnode_vt cypher_path_pattern_base_astnode_vt =
    { .name = "path base",
      .detailstr = detailstr,
      .release = cypher_astnode_release,
      .clone = clone };

cypher_astnode_t *cypher_ast_path_pattern_base(enum cypher_rel_direction direction, const cypher_astnode_t *varlength,
        const cypher_astnode_t *path_base, cypher_astnode_t **children,
        unsigned int nchildren, struct cypher_input_range range)
{
    struct path_pattern_base *node = calloc(1, sizeof(struct path_pattern_base));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATH_PATTERN_BASE,
            children, nchildren, range))
    {
        goto cleanup;
    }

    node->direction = direction;
    node->varlength = varlength;
    node->path_base = path_base;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}

enum cypher_rel_direction cypher_ast_path_pattern_base_get_direction(
        const cypher_astnode_t *astnode)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_BASE, NULL);
    struct path_pattern_base *node =
            container_of(astnode, struct path_pattern_base, _astnode);
    return node->direction;
}

const cypher_astnode_t *cypher_ast_path_pattern_base_get_child(
        const cypher_astnode_t *astnode)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_BASE, NULL);
    struct path_pattern_base *node =
            container_of(astnode, struct path_pattern_base, _astnode);
    return node->path_base;
}

cypher_astnode_t *clone(const cypher_astnode_t *self,
                        cypher_astnode_t **children)
{
    REQUIRE_TYPE(self, CYPHER_AST_PATH_PATTERN_BASE, NULL);
    struct path_pattern_base *node = container_of(self, struct path_pattern_base, _astnode);

    cypher_astnode_t *varlength = (node->varlength == NULL) ? NULL :
                                  children[child_index(self, node->varlength)];

    cypher_astnode_t *path_base = (node->path_base == NULL) ? NULL :
                                  children[child_index(self, node->path_base)];

    cypher_astnode_t *clone = cypher_ast_path_pattern_base(node->direction,
            varlength, path_base, children, self->nchildren, self->range);

    int errsv = errno;
    errno = errsv;
    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size) {
    const struct path_pattern_base *base = (const struct path_pattern_base *) self;
    const char *dir = (base->direction == CYPHER_REL_OUTBOUND) ? "outbound" :
            (base->direction == CYPHER_REL_INBOUND ? "inbound" : "bidirectional");

    const char *child = NULL;

    if (base->path_base->type == CYPHER_AST_LABEL) {
        child = "label";
    }

    return snprintf(str, size, "dir: %s, child: %s", dir, child);
}