/* Copyright 2017, Google Inc. All rights reserved.
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
#include "parser.h"

#ifdef __APPLE__
#include "third_party/fmemopen.c"
#endif

const cypher_parse_result_t* pycypher_invoke_parser(const char* query) {
  const cypher_parse_result_t* parse_result;
  cypher_parser_config_t* parser_config = cypher_parser_new_config();
  if(parser_config == NULL)
    return PyErr_SetFromErrno(PyExc_OSError);
  FILE *query_file = fmemopen((char*)query, strlen(query), "r");
  if(query_file == NULL)
    return PyErr_SetFromErrno(PyExc_OSError);

  parse_result = cypher_fparse(
    query_file, NULL, parser_config, /*flags*/0
  );

  free(parser_config);
  if(fclose(query_file) != 0)
    return PyErr_SetFromErrno(PyExc_OSError);
  return parse_result;
}

const char* pycypher_build_ast_type(const cypher_astnode_t* src_ast) {
  int i;
  for(i=0; i<pycypher_node_types_len; ++i)
    if(pycypher_node_types[i].node_type == cypher_astnode_type(src_ast))
      return pycypher_node_types[i].name;
  return "CYPHER_AST_UNKNOWN";
}

PyObject* pycypher_build_ast_instanceof(const cypher_astnode_t* src_ast) {
  PyObject* result = PyList_New(0);
  int i;
  for(i=0; i<pycypher_node_types_len; ++i)
    if(cypher_astnode_instanceof(src_ast, pycypher_node_types[i].node_type)) {
      PyObject* tmp = Py_BuildValue("s", pycypher_node_types[i].name);
      PyList_Append(result, tmp);
      Py_DECREF(tmp);
    }
  return result;
}

PyObject* pycypher_build_ast_children(PyObject* cls, const cypher_astnode_t* src_ast) {
  int nchildren = cypher_astnode_nchildren(src_ast);
  PyObject* result = PyList_New(nchildren);
  int i;
  for(i=0; i<nchildren; ++i) {
    PyObject* ast = pycypher_build_ast(cls, cypher_astnode_get_child(src_ast, i));
    if(ast == NULL)
      return NULL;
    // PyList_SetItem consumes a reference so no need to call Py_DECREF(ast)
    PyList_SetItem(result, i, ast);
  }
  return result;
}

PyObject* pycypher_build_ast(PyObject* cls, const cypher_astnode_t* src_ast) {
  PyObject* arglist = Py_BuildValue(
    "(isNNNii)", src_ast,
    pycypher_build_ast_type(src_ast),
    pycypher_build_ast_instanceof(src_ast),
    pycypher_build_ast_children(cls, src_ast),
    pycypher_extract_props(src_ast),
    cypher_astnode_range(src_ast).start.offset,
    cypher_astnode_range(src_ast).end.offset
  );
  if(arglist == NULL)
    return NULL;
  PyObject* result = PyEval_CallObject(cls, arglist);
  Py_DECREF(arglist);
  return result;
}

PyObject* pycypher_build_ast_list(
  PyObject* cls, const cypher_parse_result_t* parse_result
) {
  int nroots = cypher_parse_result_nroots(parse_result);
  PyObject* result = PyList_New(nroots);
  int i;
  for(i=0; i<nroots; ++i) {
    PyObject* ast = pycypher_build_ast(cls, cypher_parse_result_get_root(
      parse_result, i
    ));
    if(ast == NULL)
      return NULL;
    // PyList_SetItem consumes a reference so no need to call Py_DECREF(ast)
    PyList_SetItem(result, i, ast);
  }
  return result;
}

PyObject* pycypher_build_exn(PyObject* cls, const cypher_parse_error_t* err) {
  PyObject* arglist = Py_BuildValue(
    "(ssii)",
    cypher_parse_error_message(err),
    cypher_parse_error_context(err),
    cypher_parse_error_position(err).offset,
    cypher_parse_error_context_offset(err)
  );
  if(arglist == NULL)
    return NULL;
  PyObject* result = PyEval_CallObject(cls, arglist);
  Py_DECREF(arglist);
  return result;
}

PyObject* pycypher_build_exn_list(
  PyObject* cls, const cypher_parse_result_t* parse_result
) {
  int nerrors = cypher_parse_result_nerrors(parse_result);
  PyObject* result = PyList_New(nerrors);
  int i;
  for(i=0; i<nerrors; ++i) {
    PyObject* exn = pycypher_build_exn(cls, cypher_parse_result_get_error(
      parse_result, i
    ));
    if(exn == NULL)
      return NULL;
    // PyList_SetItem consumes a reference so no need to call Py_DECREF(exn)
    PyList_SetItem(result, i, exn);
  }
  return result;
}

PyObject* pycypher_parse_query(PyObject* self, PyObject* args) {
  char* query;
  PyObject* ast_class;
  PyObject* exn_class;
  if (!PyArg_ParseTuple(args, "OOs:parse", &ast_class, &exn_class, &query))
    return NULL;
  const cypher_parse_result_t* parse_result = pycypher_invoke_parser(query);
  if(parse_result == NULL)
    return PyErr_SetFromErrno(PyExc_OSError);
  PyObject* ast_list = pycypher_build_ast_list(ast_class, parse_result);
  PyObject* exn_list = pycypher_build_exn_list(exn_class, parse_result);
  cypher_parse_result_free((cypher_parse_result_t*) parse_result);
  return Py_BuildValue("(NN)", ast_list, exn_list);
}
