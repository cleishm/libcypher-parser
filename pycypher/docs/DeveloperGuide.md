# Developer guide
Some familiarity with the
[Python C API](https://docs.python.org/2/c-api/index.html) may be helpful.
All file paths in this file are relative to libcypher-parser repository root.

## Building

##### make docker-pycypher (aka ci/pycypher_linux_build.sh)
This is the preferred way. Requires docker to be available.
Builds packages for both 32-bit and 64-bit and for various Python versions.
Run `./configure && make docker-pycypher` in project root.
It will first build a container image and install libcypher-parser tarball into it.
Then it will use this image to build pycypher and bundle libcypher-parser with it.
Built packages will be tested on image that does not have libcypher-parser installed.
Afterwards, compiled wheels containing statically-linked copy of
libcypher-parser will be available in `pycypher/dist/`.

##### tests/check_pycypher.sh
This script is automatically executed when doing `make check`.
If `pycypher/` directory does not exist the test is skipped.
It relies on `src/lib/cypher-parser.h` and
`src/lib/.libs/libcypher-parser.so`
to be present. It builds python extension module in-place and tests it, it does
not create distributable packages.

##### ci/pycypher_osx_build.sh
Unfortunately, this is only meant to be used in a CI build.
Like `ci/pycypher_linux_build.sh`, builds packages for various versions of Python
and bundles libcypher-parser with them.
Afterwards runs tests against created wheels.

## PyPI deployment
Travis build is configured to deploy to PyPI whenever a git tag is pushed.
It will first build the packages for Linux and Mac OS and run the tests.
Packages will not be uploaded if the tests fail.

The uploading code reads PyPI credentials from `$PYPI_USER` and `$PYPI_PASSWORD`
environment variables.
Package version is taken from `AC_INIT(...)` line in `configure.ac`.
Additions after `~` sign are ignored, for example both
`AC_INIT([libcypher-parser],[0.5.5~devel])` and `AC_INIT([libcypher-parser],[0.5.5])`
will cause version of Python package to be `0.5.5`.
Travis build number is used as a
["build tag"](https://www.python.org/dev/peps/pep-0427/#file-name-convention)
in uploaded wheels.

Should anything go wrong, commiting a fix (or a revert)
and pushing another git tag
(tag name can be anything, it is only to trigger deploy in Travis)
with the same version specified in `configure.ac`
will upload new builds for this version.
This way any subsequent downloads (even if users specify version explicitly)
will use the new builds.
PyPI will accept the upload even if package with this version already exists
because the build tag (= Travis build number) will be different.
And [don't](
  https://doughellmann.com/blog/2016/02/25/so-youve-released-a-broken-package-to-pypi-what-do-you-do-now/)
remove files from PyPI.

## How generation works
`pycypher/generate.py` is a self-contained (no dependencies necessary) python
script that parses `src/lib/cypher-parser.h.in` and `configure.ac`.
It produces `pycypher/pycypher/operators.c`, `pycypher/pycypher/node_types.c`,
`pycypher/pycypher/props.c`, `pycypher/pycypher/getters.py`, `pycypher/pycypher/version.py`,
`pycypher/LICENSE` and `pycypher/setup.cfg`.

It uses regular expressions to look for annotated declarations in the header
file and for the `AC_INIT` in `configure.ac`.

## Annotations

### types of ast nodes
Declarations of the form
```c
CYPHER_NODE_TYPES_BEGIN
...
extern const cypher_astnode_type_t CYPHER_AST_{TYPE};
...
CYPHER_NODE_TYPES_END
```
will be available in the bindings. A `cypher_astnode_t` value will be
translated to string of the form `'CYPHER_AST_{TYPE}'` on the Python side.

*Example:*
```c
extern const cypher_astnode_type_t CYPHER_AST_STATEMENT;
```
```Python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> q.type
'CYPHER_AST_STATEMENT'
```
`'CYPHER_AST_UNKNOWN'` will be returned for any `cypher_astnode_type_t` value
that does not have a corresponding declaration between `CYPHER_NODE_TYPES_BEGIN`
and `CYPHER_NODE_TYPES_END`.

### operators
Declarations of the form
```c
CYPHER_OPERATORS_BEGIN
...
extern const cypher_operator_t *CYPHER_OP_{TYPE};
...
CYPHER_OPERATORS_END
```
will be available in the bindings. A `cypher_operator_t *` value will be
translated to string of the form `'CYPHER_OP_{TYPE}'` on the Python side.

*Example:*
```c
extern const cypher_operator_t *CYPHER_OP_OR;
```
```Python
>>> from pycypher import parse_query
>>> q, = parse_query('MATCH (a) WHERE a.x OR a.y')
>>> node, = q.find_nodes(type='CYPHER_AST_BINARY_OPERATOR')
>>> node.get_operator()
'CYPHER_OP_OR'
```
`'CYPHER_OP_UNKNOWN'` will be returned for any `cypher_operator_t *` value
that does not have a corresponding declaration between `CYPHER_OPERATORS_BEGIN`
and `CYPHER_OPERATORS_END`.

### properties of ast nodes
Function declarations of the form:
```c
__cypherlang_getter enum cypher_rel_direction cypher_ast_{type}_get_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter const cypher_operator_t * cypher_ast_{type}_get_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter bool cypher_ast_{type}_get_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter bool cypher_ast_{type}_is_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter bool cypher_ast_{type}_has_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter const char * cypher_ast_{type}_get_{prop}(const cypher_astnode_t * {_node});
__cypherlang_getter const cypher_astnode_t * cypher_ast_{type}_get_{prop}(const cypher_astnode_t * {_node});
```

are interpreted as getters for properties on ast nodes. On the Python side,
they appear as getter methods (`get_{prop}()`, `is_{prop}()` or `has_{prop}()`) on
the CypherAstNode class.

*Example 1:*
```c
__cypherlang_getter
const cypher_astnode_t *cypher_ast_statement_get_body(
        const cypher_astnode_t *node);
```
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> q
<CypherAstNode.CYPHER_AST_STATEMENT>
>>> q.get_body()
<CypherAstNode.CYPHER_AST_QUERY>
```

*Example 2:*
```c
__cypherlang_getter
const char *cypher_ast_identifier_get_name(const cypher_astnode_t *node);
```
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a)")
>>> match = q.get_body().get_clauses()[0]
>>> node_pattern, = match.get_pattern().get_paths()[0].get_elements()
>>> ident = node_pattern.get_identifier()
>>> ident.get_name()
'a'
```

*Example 3:*
```c
__cypherlang_getter
bool cypher_ast_create_node_prop_constraint_is_unique(
       const cypher_astnode_t *node);
```
```python
>>> q1, q2 = parse_query("""
  CREATE CONSTRAINT ON (f:Foo) ASSERT exists(f.bar);
  CREATE CONSTRAINT ON (f:Foo) ASSERT f.bar IS UNIQUE;
""")
>>> q1.get_body().is_unique()
False
>>> q2.get_body().is_unique()
True
```

### list-valued properties of ast nodes
Function declarations of the form:
```c
__cypherlang_list_getter({length_getter})
const cypher_operator_t * cypher_ast_{type}_get_{prop}(
  const cypher_astnode_t * {_node}, unsigned int {_index});
__cypherlang_list_getter({length_getter})
const cypher_astnode_t * cypher_ast_{type}_get_{prop}(
  const cypher_astnode_t * {_node}, unsigned int {_index});
__cypherlang_list_getter({length_getter} + 1)
const cypher_astnode_t * cypher_ast_{type}_get_{prop}(
  const cypher_astnode_t * {_node}, unsigned int {_index});
```

are interpreted as getters for list-valued properties of ast nodes.
`cypher_ast_{type}_{length_getter}` is used to determine length of
the list. `cypher_ast_{type}_get_{prop}` is used to fetch elements to fill the
list.

Edge case: `__cypherlang_list_getter({length_getter} + 1)` syntax
increases length of the list by one - it is necessary for
`cypher_ast_comparison_get_argument` to be correctly exposed to Python
as `get_arguments()`.

On the Python side, they appear as getter methods `get_{prop}s()`
on the CypherAstNode class.  These methods return a Python
list.

*Example 1:*
```c
unsigned int cypher_ast_query_nclauses(const cypher_astnode_t *node);

__cypherlang_list_getter(nclauses)
const cypher_astnode_t *cypher_ast_query_get_clause(
        const cypher_astnode_t *node, unsigned int index);
```
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> body = q.get_body()
>>> body.get_clauses()
[<CypherAstNode.CYPHER_AST_MATCH>, <CypherAstNode.CYPHER_AST_RETURN>]
```

*Example 2:*
```c
unsigned int cypher_ast_comparison_get_length(const cypher_astnode_t *node);

__cypherlang_list_getter(get_length)
const cypher_operator_t *cypher_ast_comparison_get_operator(
        const cypher_astnode_t *node, unsigned int pos);
```
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) WHERE a.x < a.y <= a.z")
>>> where = q.get_body().get_clauses()[0].get_predicate()
>>> where.get_operators()
['CYPHER_OP_LT', 'CYPHER_OP_LTE']
```

*Example 3:*
```c
unsigned int cypher_ast_comparison_get_length(const cypher_astnode_t *node);

__cypherlang_list_getter(get_length + 1)
const cypher_astnode_t *cypher_ast_comparison_get_argument(
        const cypher_astnode_t *node, unsigned int pos);
```
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) WHERE a.x < a.y <= a.z")
>>> where = q.get_body().get_clauses()[0].get_predicate()
>>> where.get_arguments()
[<CypherAstNode.CYPHER_AST_PROPERTY_OPERATOR>,
<CypherAstNode.CYPHER_AST_PROPERTY_OPERATOR>,
<CypherAstNode.CYPHER_AST_PROPERTY_OPERATOR>,]
```
