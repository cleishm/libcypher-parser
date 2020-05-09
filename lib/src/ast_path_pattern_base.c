#include "../../config.h"
#include "astnode.h"
#include "util.h"
#include <assert.h> 

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

const cypher_astnode_t *cypher_ast_path_pattern_base_get_varlength(
        const cypher_astnode_t *astnode)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_BASE, NULL);
    struct path_pattern_base *node =
            container_of(astnode, struct path_pattern_base, _astnode);
    return node->varlength;
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
    struct path_pattern_base *node = container_of(self, struct path_pattern_base, _astnode);
    
    size_t n = 0;
    ssize_t r = snprintf(str, size, "%s",
            (node->direction == CYPHER_REL_INBOUND)? "<" : "");
    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->path_base != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, "@%u",
                node->path_base->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }

    r = snprintf(str+n, (n < size)? size-n : 0, "%s",
            (node->direction == CYPHER_REL_OUTBOUND)? ">" : "");

    if (r < 0)
    {
        return -1;
    }
    n += r;

    if (node->varlength != NULL)
    {
        r = snprintf(str+n, (n < size)? size-n : 0, " range=@%u",
                node->varlength->ordinal);
        if (r < 0)
        {
            return -1;
        }
        n += r;
    }
    return n;
}