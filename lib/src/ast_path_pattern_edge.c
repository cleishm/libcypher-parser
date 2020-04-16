#include "../../config.h"
#include "astnode.h"
#include "util.h"
#include <assert.h> 

struct path_pattern_edge
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *reltype;
};

static cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_path_pattern_edge_astnode_vt =
        { .name = "path pattern edge",
          .detailstr = detailstr,
          .release = cypher_astnode_release,
          .clone = clone };


cypher_astnode_t *cypher_ast_path_pattern_edge( const cypher_astnode_t *reltype,
        cypher_astnode_t **children, unsigned int nchildren,
        struct cypher_input_range range)
{
    struct path_pattern_edge *node = calloc(1, sizeof(struct path_pattern_edge));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATH_PATTERN_EDGE,
            children, nchildren, range))
    {
        goto cleanup;
    }
    node->reltype = reltype;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}

const cypher_astnode_t *cypher_ast_path_pattern_edge_get_reltype(
        const cypher_astnode_t *astnode)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_EDGE, NULL);
    struct path_pattern_edge *node =
            container_of(astnode, struct path_pattern_edge, _astnode);
    return node->reltype;
}

cypher_astnode_t *clone(const cypher_astnode_t *self,
        cypher_astnode_t **children)
{
    REQUIRE_TYPE(self, CYPHER_AST_PATH_PATTERN_EDGE, NULL);
    struct path_pattern_edge *node = container_of(self, struct path_pattern_edge, _astnode);

    cypher_astnode_t *reltype = (node->reltype == NULL) ? NULL :
            children[child_index(self, node->reltype)];

    cypher_astnode_t *clone = cypher_ast_path_pattern_edge(reltype,
            children, self->nchildren, self->range);

    int errsv = errno;
    errno = errsv;
    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size) {
    struct path_pattern_edge *node = container_of(self, struct path_pattern_edge, _astnode);
    return snprintf(str, size, "edge label=@%u", node->reltype->ordinal);
}
