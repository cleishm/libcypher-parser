# User guide

## Basics
Use `parse_query` to obtain a list of `CypherAstNode` instances:
```python
>>> from pycypher import parse_query
>>>
>>> type(parse_query("MATCH (a) RETURN (A)"))
<type 'list'>
>>>
>>> type(parse_query("MATCH (a) RETURN (A)")[0])
<class 'pycypher.ast.CypherAstNode'>
>>>
>>> parse_query("MATCH (a) RETURN (A)")
[<CypherAstNode.CYPHER_AST_STATEMENT>]
>>>
>>> parse_query("""
...   /* global comment */
...   MATCH (a) RETURN (A);
...   MERGE (a /* local comment */)->(b);
...   /* global comment 2 */
...   RETURN 1;
... """)
[<CypherAstNode.CYPHER_AST_BLOCK_COMMENT>,
<CypherAstNode.CYPHER_AST_STATEMENT>,
<CypherAstNode.CYPHER_AST_STATEMENT>,
<CypherAstNode.CYPHER_AST_BLOCK_COMMENT>,
<CypherAstNode.CYPHER_AST_STATEMENT>]
```

Every `CypherAstNode` object has some basic attributes:
* `type` is a string
identifier of a node type. For a complete list of node types, see
[here](https://cleishm.github.io/libcypher-parser/doc/latest/cypher-parser_8h.html)
* `children` is a list of children of that node.
* `props` is a dictionary of properties of that node. However, properties that
  are other nodes or are lists of other nodes are not present in this
  dictionary and they are only accessible by getter methods.
* `start` and `end` are character offsets in the original query
and denote the substring corresponding to the ast node.

*Example:*
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>>
>>> q.type
'CYPHER_AST_STATEMENT'
>>>
>>> q.children
[<CypherAstNode.CYPHER_AST_QUERY>]
>>>
>>> q.props
{}
>>> q.start
0
>>> q.end
20
>>> ident, _ = q.find_nodes(type='CYPHER_AST_IDENTIFIER')
>>> ident.props
{'name': 'a'}
```

## Getter methods
They correspond to getter functions from the
[C library](https://cleishm.github.io/libcypher-parser/doc/latest/cypher-parser_8h.html).
For detailed information on how they are generated, see sections
**properties of ast nodes** and **list-valued properties of ast nodes** of
[Developer guide](DeveloperGuide.md).

Getter methods return `None` or an empty list `[]` if they are called on a node that
does not have given property.

*Example:*
```python
from pycypher import parse_query
>>> q, = parse_query("MATCH (a)")
>>>
>>> q.get_name()
None
>>> body = q.get_body()
>>> body
<CypherAstNode.CYPHER_AST_QUERY>
>>> clauses = body.get_clauses()
>>> clauses
[<CypherAstNode.CYPHER_AST_MATCH>]
>>> match = clauses[0]
>>> pattern = match.get_pattern()
>>> pattern
<CypherAstNode.CYPHER_AST_PATTERN>
>>> path, = pattern.get_paths()
>>> path
<CypherAstNode.CYPHER_AST_PATTERN_PATH>
>>> element, = path.get_elements()
>>> element
<CypherAstNode.CYPHER_AST_NODE_PATTERN>
>>> ident = element.get_identifier()
>>> ident
<CypherAstNode.CYPHER_AST_IDENTIFIER>
>>> ident.get_name()
'a'
>>> ident.get_clauses()
[]
>>> ident.get_body()
None
```

## Searching descendant AST nodes
`CypherAstNode.find_nodes` returns an iterable of descendant nodes (including
node the method was called on) that match given criteria.

Use `CypherAstNode.find_nodes(type=...)` to find nodes by exact type:
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> list(q.find_nodes(type='CYPHER_AST_IDENTIFIER'))
[<CypherAstNode.CYPHER_AST_IDENTIFIER>, <CypherAstNode.CYPHER_AST_IDENTIFIER>]
>>> list(q.find_nodes(type='CYPHER_AST_PATTERN'))
[<CypherAstNode.CYPHER_AST_PATTERN>]
```

Use `CypherAstNode.find_nodes(instanceof=...)` to find nodes by type including
subtypes:
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> list(q.find_nodes(type='CYPHER_AST_EXPRESSION'))
[]
>>> list(q.find_nodes(instanceof='CYPHER_AST_EXPRESSION'))
[<CypherAstNode.CYPHER_AST_PATTERN_PATH>,
<CypherAstNode.CYPHER_AST_IDENTIFIER>,
<CypherAstNode.CYPHER_AST_IDENTIFIER>]
```

Use `CypherAstNode.find_nodes(role=...)` to find nodes by name of attribute
they are accessible from parent node
(if the name is plural, use singular form instead):
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>>
>>> q.get_body().get_clauses()
[<CypherAstNode.CYPHER_AST_MATCH>, <CypherAstNode.CYPHER_AST_RETURN>]
>>> list(q.find_nodes(role='clause'))
[<CypherAstNode.CYPHER_AST_MATCH>, <CypherAstNode.CYPHER_AST_RETURN>]
>>>
>>> q.get_body().get_clauses()[0].get_pattern()
<CypherAstNode.CYPHER_AST_PATTERN>
>>> list(q.find_nodes(role='pattern'))
[<CypherAstNode.CYPHER_AST_PATTERN>]
```

Use `CypherAstNode.find_nodes(start=..., end=...)` to find nodes by
position in the original query text.
```python
>>> from pycypher import parse_query
>>> q, = parse_query("MATCH (a) RETURN (a)")
>>> list(q.find_nodes(start=6, end=8))
[<CypherAstNode.CYPHER_AST_IDENTIFIER>]
```

## Handling parse errors
When there are any parse errors in a query, an exception
`pycypher.CypherParseError` is raised. The exception has following attributes:
* `message` - the error message
* `context` - section of the input around where the error occurred, that is limited
in length and suitable for presentation to a user.
* `offset` - character offset of the error in the original query
* `context_offset` - character offset of the error in the context
* `all_errors` - list of `CypherParseError` objects - all errors that are
in the query - there can be more than one error, this way all of them can
be accessed. Note that the attributes `parse_result` and `all_errors` are
available only on the first element of this list.
* `parse_result` - this is what normally would be returned from `parse_query`,
with the difference that some nodes in the AST will have type `CYPHER_AST_ERROR`.

```python
>>> from pycypher import parse_query, CypherParseError
>>> parse_query("MATCH (a) RETURN (*$@!); RETURN 1;")
Traceback (most recent call last):
  File "<input>", line 1, in <module>
    parse_query("MATCH (a) RETURN (*$@!); RETURN 1;")
  File "/.../site-packages/pycypher/__init__.py", line 39, in parse_query
    raise e
CypherParseError: Invalid input '*': expected an identifier, a label, '{', a parameter, ')', NOT, '+', '-', TRUE, FALSE, NULL, "...string...", a float, an integer, '[', CASE, FILTER, EXTRACT, REDUCE, ALL, ANY, NONE, SINGLE, shortestPa
th, allShortestPaths, '(' or a function name
>>>
>>> try:
...   parse_query("MATCH (a) RETURN (*$@!); RETURN 1;")
... except CypherParseError as exn:
...   e = exn
...
>>> e.message
'Invalid input \'*\': expected an identifier, a label, \'{\', a parameter, \')\', NOT, \'+\', \'-\', TRUE, FALSE, NULL, "...string...", a float, an integer, \'[\', CASE, FILTER, EXTRACT, REDUCE, ALL, ANY, NONE, SINGLE, shortestPath, a llShortestPaths, \'(\' or a function name'
>>> e.context
'MATCH (a) RETURN (*$@!);'
>>> e.offset
18
>>> e.context_offset
18
>>> e.all_errors
[CypherParseError('Invalid input \'*\': expected an identifier, a label, \'{\', a parameter, \')\', NOT, \'+\', \'-\', TRUE, FALSE, NULL, "...string...", a float, an integer, \'[\', CASE, FILTER, EXTRACT, REDUCE, ALL, ANY, NONE, SINGLE, shortestPath, allShortestPaths, \'(\' or a function name',)]
>>> e.parse_result
[<CypherAstNode.CYPHER_AST_STATEMENT>, <CypherAstNode.CYPHER_AST_STATEMENT>]
>>> list(e.parse_result[0].find_nodes(type='CYPHER_AST_ERROR'))
[<CypherAstNode.CYPHER_AST_ERROR>]
```

## Serializing to JSON
`CypherAstNode.to_json()` returns a json-serializable nested representation
made from Python dictionaries, lists and primitive types.

*Example:*
```python
>>> import json
>>> from pycypher import parse_query
>>>
>>> q, = parse_query("RETURN 1")
>>> print json.dumps(q.to_json(), indent=2, separators=(', ', ': '))
{
  "instanceof": [
    "CYPHER_AST_STATEMENT"
  ],
  "end": 8,
  "roles": [],
  "start": 0,
  "props": {},
  "type": "CYPHER_AST_STATEMENT",
  "children": [
    {
      "instanceof": [
        "CYPHER_AST_QUERY"
      ],
      "end": 8,
      "roles": [
        "body"
      ],
      "start": 0,
      "props": {},
      "type": "CYPHER_AST_QUERY",
      "children": [
        {
          "instanceof": [
            "CYPHER_AST_QUERY_CLAUSE",
            "CYPHER_AST_RETURN"
          ],
          "end": 8,
          "roles": [
            "clause"
          ],
          "start": 0,
          "props": {
            "distinct": false,
            "include_existing": false
          },
          "type": "CYPHER_AST_RETURN",
          "children": [
            {
              "instanceof": [
                "CYPHER_AST_PROJECTION"
              ],
              "end": 8,
              "roles": [
                "projection"
              ],
              "start": 7,
              "props": {},
              "type": "CYPHER_AST_PROJECTION",
              "children": [
                {
                  "instanceof": [
                    "CYPHER_AST_EXPRESSION",
                    "CYPHER_AST_INTEGER"
                  ],
                  "end": 8,
                  "roles": [
                    "expression"
                  ],
                  "start": 7,
                  "props": {
                    "valuestr": "1"
                  },
                  "type": "CYPHER_AST_INTEGER",
                  "children": []
                },
                {
                  "instanceof": [
                    "CYPHER_AST_EXPRESSION",
                    "CYPHER_AST_IDENTIFIER"
                  ],
                  "end": 8,
                  "roles": [
                    "alias"
                  ],
                  "start": 7,
                  "props": {
                    "name": "1"
                  },
                  "type": "CYPHER_AST_IDENTIFIER",
                  "children": []
                }
              ]
            }
          ]
        }
      ]
    }
  ]
}
```
