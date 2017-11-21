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
#include "node_types.h"
#include "operators.h"
#include "props.h"

static PyMethodDef pycypher_methods[] = {
    {
      "parse_query", pycypher_parse_query, METH_VARARGS,
      "Return a list of CypherAst instances corresponding to parsed query."
    },
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3

  static int pycypher_traverse(PyObject *m, visitproc visit, void *arg) {
    return 0;
  }
  static int pycypher_clear(PyObject *m) {
    return 0;
  }
  static struct PyModuleDef pycypher_module = {
    PyModuleDef_HEAD_INIT,
    "pycypher.bindings",
    NULL,
    0,
    pycypher_methods,
    NULL,
    pycypher_traverse,
    pycypher_clear,
    NULL
  };
  PyMODINIT_FUNC PyInit_bindings(void)
  {
    PyObject *module = PyModule_Create(&pycypher_module);
    if (module == NULL)
      return NULL;
    pycypher_init_node_types();
    pycypher_init_operators();
    pycypher_init_props();
    return module;
  }

#else

  PyMODINIT_FUNC initbindings(void)
  {
    PyObject *module = Py_InitModule("pycypher.bindings", pycypher_methods);
    if (module == NULL)
      return;
    pycypher_init_node_types();
    pycypher_init_operators();
    pycypher_init_props();
  }

#endif
