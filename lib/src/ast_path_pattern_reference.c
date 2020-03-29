#include "astnode.h"

struct path_pattern_reference
{
    cypher_astnode_t _astnode;
    const cypher_astnode_t *identifier;
};

static cypher_astnode_t *clone(const cypher_astnode_t *self,
                               cypher_astnode_t **children);
static ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size);


const struct cypher_astnode_vt cypher_path_pattern_reference_astnode_vt =
        { .name = "path pattern reference",
                .detailstr = detailstr,
                .release = cypher_astnode_release,
                .clone = clone };


cypher_astnode_t *cypher_ast_path_pattern_reference( const cypher_astnode_t *identifier,
                                                    cypher_astnode_t **children, unsigned int nchildren,
                                                    struct cypher_input_range range)
{
    struct path_pattern_reference *node = calloc(1, sizeof(struct path_pattern_reference));
    if (node == NULL)
    {
        return NULL;
    }
    if (cypher_astnode_init(&(node->_astnode), CYPHER_AST_PATH_PATTERN_REFERENCE,
                            children, nchildren, range))
    {
        goto cleanup;
    }
    node->identifier = identifier;
    return &(node->_astnode);

    int errsv;
cleanup:
    errsv = errno;
    free(node);
    errno = errsv;
    return NULL;
}

const cypher_astnode_t *cypher_ast_path_pattern_reference_get_identifier(
        const cypher_astnode_t *astnode)
{
    REQUIRE_TYPE(astnode, CYPHER_AST_PATH_PATTERN_REFERENCE, NULL);
    struct path_pattern_reference *node =
    container_of(astnode, struct path_pattern_reference, _astnode);
    return node->identifier;
}

cypher_astnode_t *clone(const cypher_astnode_t *self,
                        cypher_astnode_t **children)
{
    REQUIRE_TYPE(self, CYPHER_AST_PATH_PATTERN_REFERENCE, NULL);
    struct path_pattern_reference *node = container_of(self, struct path_pattern_reference, _astnode);

    cypher_astnode_t *identifier = (node->identifier == NULL) ? NULL :
            children[child_index(self, node->identifier)];

    cypher_astnode_t *clone = cypher_ast_path_pattern_reference(identifier,
            children, self->nchildren, self->range);

    return clone;
}

ssize_t detailstr(const cypher_astnode_t *self, char *str, size_t size) {
    struct path_pattern_reference *node = container_of(self, struct path_pattern_reference, _astnode);
    return snprintf(str, size, "~@%u", node->identifier->ordinal);
}
