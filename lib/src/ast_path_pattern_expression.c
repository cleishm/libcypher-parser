#include "astnode.h"

struct path_pattern_expression
{
    cypher_astnode_t _astnode;
    size_t nelements;
    const cypher_astnode_t *elements[];
};

static cypher_astnode_t *clone(const cypher_astnode_t *self,
                               cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);

const struct cypher_astnode_vt cypher_path_pattern_expression_astnode_vt =
    {
      .name = "expression",
      .detailstr = detailstr,
      .release = cypher_astnode_release,
      .clone = clone };

cypher_astnode_t *cypher_ast_path_pattern_expression(cypher_astnode_t * const *elements, unsigned int nelements,
        cypher_astnode_t **children, unsigned int nchildren, struct cypher_input_range range)
{
    struct path_pattern_expression *node = calloc(1, sizeof(struct path_pattern_expression) +
                                          nelements * sizeof(cypher_astnode_t *));
    if (node == NULL)
    {
        return NULL;
    }

    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATH_PATTERN_EXPRESSION,
                            children, nchildren, range))
    {
        goto cleanup;
    }

    memcpy(node->elements, elements, nelements * sizeof(cypher_astnode_t *));
    node->nelements = nelements;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}

unsigned int cypher_ast_path_pattern_expression_get_nelements(const cypher_astnode_t *astnode) {
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_EXPRESSION, NULL);
    struct path_pattern_expression *node =
            container_of(astnode, struct path_pattern_expression, _astnode);
    return node->nelements;
}

const cypher_astnode_t *cypher_ast_path_pattern_expression_get_element(
        const cypher_astnode_t *astnode, unsigned int index)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_EXPRESSION, NULL);
    struct path_pattern_expression *node =
            container_of(astnode, struct path_pattern_expression, _astnode);
    return node->elements[index];
}

cypher_astnode_t *clone(const cypher_astnode_t *self, cypher_astnode_t **children)
{
    struct path_pattern_expression *node = container_of(self, struct path_pattern_expression, _astnode);

    cypher_astnode_t **elements =
            calloc(node->nelements, sizeof(cypher_astnode_t *));
    if (elements == NULL)
    {
        return NULL;
    }
    for (unsigned int i = 0; i < node->nelements; ++i)
    {
        elements[i] = children[child_index(self, node->elements[i])];
    }

    cypher_astnode_t *clone = cypher_ast_path_pattern_expression(elements, node->nelements, children, self->nchildren,
                                                      self->range);
    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size) {
    const struct path_pattern_expression *expression = (const struct path_pattern_expression *) self;
    return snprintf(str, size, "Expression, elems: %ld", expression->nelements);
}