#!/usr/bin/env python
# Copyright 2017, Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import re
import os
import os.path


class BaseTransform(object):
    match_begin = None
    match = None
    match_end = None
    emit_begin = None
    emit = None
    emit_end = None
    declaration = None

    def __init__(self):
        if isinstance(self.match_begin, str):
            self.match_begin = ' '.join(self.match_begin.split())
        if isinstance(self.match, str):
            self.match = ' '.join(self.match.split())
        if isinstance(self.match_end, str):
            self.match_end = ' '.join(self.match_end.split())
        if isinstance(self.emit_begin, str):
            self.emit_begin = '  ' + ' '.join(self.emit_begin.split())
        if isinstance(self.emit, str):
            self.emit = '    ' + ' '.join(self.emit.split())
            self.emit = self.emit.replace('{', '{{')
            self.emit = self.emit.replace('}', '}}')
            self.emit = re.sub(
                r'\{([a-zA-Z_][a-zA-Z_0-9]*)\}', r'\1', self.emit
            )
        if isinstance(self.emit_end, str):
            self.emit_end = '  ' + ' '.join(self.emit_end.split())
        if isinstance(self.declaration, str):
            self.declaration = '\n'.join(
                l.strip() for l in self.declaration.strip().split('\n')
            )


    def run(self, cypher_parser_h):
        """Return an iterable of strings - lines of generated code."""
        raise NotImplementedError


class Transform(BaseTransform):
    def _prepare_regex(self, s):
        s = s.strip()
        special_tokens = ['(', ')', '*', ',', ';', '+']
        for tok in special_tokens:
            s = s.replace(tok, ' \\' + tok + ' ')
        s = r'\s*'.join(s.split())
        s = re.sub(r'\{([a-z_][a-z_0-9]*)\}', r'(?P<\1>[a-z_][a-z_0-9]*)', s)
        s = re.sub(r'\{([A-Z_][A-Z_0-9]*)\}', r'(?P<\1>[A-Z_][A-Z_0-9]*)', s)
        return s

    def generate(self, cypher_parser_h):
        regex = self._prepare_regex(self.match)
        p = re.compile(regex)
        if self.emit_begin is not None:
            yield self.emit_begin
        for match in p.finditer(cypher_parser_h):
            ctx = match.groupdict()
            for k in list(ctx.keys()):
                if k.isupper():
                    ctx[k.lower()] = ctx[k].lower()
                if k.islower():
                    ctx[k.upper()] = ctx[k].upper()
            yield self.emit.format(**ctx)
        if self.emit_end is not None:
            yield self.emit_end


class AggregateTransform(BaseTransform):
    def generate(self, cypher_parser_h):
        assert self.match_begin is None
        assert self.match is None
        assert self.match_end is None
        result = []
        if self.emit_begin is not None:
            yield self.emit_begin
        for transform in self.emit:
            for line in transform().generate(cypher_parser_h):
                yield line
        if self.emit_end is not None:
            yield self.emit_end


class Prop(Transform):
    emit_python = None

    def __init__(self):
        super(Prop, self).__init__()
        if self.emit_python is not None:
            self.emit_python = self.emit_python.rstrip()
            self.emit_python = re.sub(
                '^    ', '', self.emit_python, flags=re.MULTILINE
            )

    def generate_python(self, cypher_parser_h):
        p = re.compile(self._prepare_regex(self.match))
        for match in p.finditer(cypher_parser_h):
            ctx = match.groupdict()
            for k in list(ctx.keys()):
                if k.isupper():
                    ctx[k.lower()] = ctx[k].lower()
                if k.islower():
                    ctx[k.upper()] = ctx[k].upper()
            yield self.emit_python.format(**ctx)


class PycypherDirectionProp(Prop):
    match = '''
        __cypherlang_getter enum cypher_rel_direction
        cypher_ast_{type}_get_{prop}(const cypher_astnode_t *{_node});
    '''
    declaration = '''
        pycypher_direction_prop_t* pycypher_direction_props;
        size_t pycypher_direction_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_direction_prop_t, pycypher_direction_props, TABLE({
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_get_{prop}},
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_getter
        def get_{prop}(self):
            pass
    '''


class PycypherOperatorProp(Prop):
    match = '''
        __cypherlang_getter const cypher_operator_t *
        cypher_ast_{type}_get_{prop}(const cypher_astnode_t *{_node});
    '''
    declaration = '''
        pycypher_operator_prop_t* pycypher_operator_props;
        size_t pycypher_operator_props_len;
    '''
    emit_begin = '''
      INIT_TABLE(pycypher_operator_prop_t, pycypher_operator_props, TABLE({
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_get_{prop}},
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_getter
        def get_{prop}(self):
            pass
    '''


class PycypherOperatorListProp(Prop):
    match = '''
        __cypherlang_list_getter({length_getter}) const cypher_operator_t *
        cypher_ast_{type}_get_{prop}(
            const cypher_astnode_t *{_node}, unsigned int {_index}
        );
    '''
    declaration = '''
        pycypher_operator_list_prop_t* pycypher_operator_list_props;
        size_t pycypher_operator_list_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_operator_list_prop_t, pycypher_operator_list_props,
        TABLE({
    '''
    emit = '''
        {
            CYPHER_AST_{TYPE}, "{prop}s",
            cypher_ast_{type}_{length_getter},
            cypher_ast_{type}_get_{prop},
        },
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_list_getter('{prop}')
        def get_{prop}s(self):
            pass
    '''


class PycypherBoolProp_Get(Prop):
    match = '''
        __cypherlang_getter bool
        cypher_ast_{type}_get_{prop}(const cypher_astnode_t *{_node});
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_get_{prop}},
    '''
    emit_python = '''
        @pycypher_getter
        def get_{prop}(self):
            pass
    '''


class PycypherBoolProp_Is(Prop):
    match = '''
        __cypherlang_getter bool
        cypher_ast_{type}_is_{prop}(const cypher_astnode_t *{_node});
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_is_{prop}},
    '''
    emit_python = '''
        @pycypher_getter
        def is_{prop}(self):
            pass
    '''


class PycypherBoolProp_Has(Prop):
    match = '''
        __cypherlang_getter bool
        cypher_ast_{type}_has_{prop}(const cypher_astnode_t *{_node});
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_has_{prop}},
    '''
    emit_python = '''
        @pycypher_getter
        def has_{prop}(self):
            pass
    '''


class PycypherBoolProp(AggregateTransform):
    declaration = '''
        pycypher_bool_prop_t* pycypher_bool_props;
        size_t pycypher_bool_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_bool_prop_t, pycypher_bool_props, TABLE({
    '''
    emit = [
        PycypherBoolProp_Get,
        PycypherBoolProp_Is,
        PycypherBoolProp_Has,
    ]
    emit_end = '}))'


class PycypherStringProp(Prop):
    match = '''
        __cypherlang_getter const char *
        cypher_ast_{type}_get_{prop}(const cypher_astnode_t *{_node});
    '''
    declaration = '''
        pycypher_string_prop_t* pycypher_string_props;
        size_t pycypher_string_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_string_prop_t, pycypher_string_props, TABLE({
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_get_{prop}},
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_getter
        def get_{prop}(self):
            pass
    '''


class PycypherAstListProp(Prop):
    match = '''
        __cypherlang_list_getter({length_getter}) const cypher_astnode_t *
        cypher_ast_{type}_get_{prop}(
            const cypher_astnode_t *{_node}, unsigned int {_index}
        );
    '''
    declaration = '''
        pycypher_ast_list_prop_t* pycypher_ast_list_props;
        size_t pycypher_ast_list_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_ast_list_prop_t, pycypher_ast_list_props, TABLE({
    '''
    emit = '''
        {
            CYPHER_AST_{TYPE}, "{prop}s", "{prop}",
            cypher_ast_{type}_{length_getter},
            cypher_ast_{type}_get_{prop},
        },
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_list_getter('{prop}')
        def get_{prop}s(self):
            pass
    '''


class PycypherAstListPlusOneProp(Prop):
    match = '''
        __cypherlang_list_getter({length_getter} + 1) const cypher_astnode_t *
        cypher_ast_{type}_get_{prop}(
            const cypher_astnode_t *{_node}, unsigned int {_index}
        );
    '''
    declaration = '''
        pycypher_ast_list_plus_one_prop_t* pycypher_ast_list_plus_one_props;
        size_t pycypher_ast_list_plus_one_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(
        pycypher_ast_list_plus_one_prop_t, pycypher_ast_list_plus_one_props,
        TABLE({
    '''
    emit = '''
        {
            CYPHER_AST_{TYPE}, "{prop}s", "{prop}",
            cypher_ast_{type}_{length_getter},
            cypher_ast_{type}_get_{prop},
        },
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_list_getter('{prop}')
        def get_{prop}s(self):
            pass
    '''


class PycypherAstProp(Prop):
    match = '''
        __cypherlang_getter const cypher_astnode_t *
        cypher_ast_{type}_get_{prop}(const cypher_astnode_t *{_node});
    '''
    declaration = '''
        pycypher_ast_prop_t* pycypher_ast_props;
        size_t pycypher_ast_props_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_ast_prop_t, pycypher_ast_props, TABLE({
    '''
    emit = '''
        {CYPHER_AST_{TYPE}, "{prop}", cypher_ast_{type}_get_{prop}},
    '''
    emit_end = '}))'
    emit_python = '''
        @pycypher_getter
        def get_{prop}(self):
            pass
    '''


class PycypherOperator(Transform):
    match_begin = 'CYPHER_OPERATORS_BEGIN'
    match = '''
        extern const cypher_operator_t *CYPHER_OP_{OPERATOR};
    '''
    match_end = 'CYPHER_OPERATORS_END'
    declaration = '''
        pycypher_operator_t* pycypher_operators;
        size_t pycypher_operators_len;
    '''
    emit_begin = '''
        INIT_TABLE(pycypher_operator_t, pycypher_operators, TABLE({
    '''
    emit = '''
        {"CYPHER_OP_{OPERATOR}", CYPHER_OP_{OPERATOR}},
    '''
    emit_end = '}))'


class PycypherNodeType(Transform):
    match_begin = 'CYPHER_NODE_TYPES_BEGIN'
    match = '''
        extern const cypher_astnode_type_t CYPHER_AST_{TYPE};
    '''
    match_end = 'CYPHER_NODE_TYPES_END'
    declaration = '''
        pycypher_node_type_t* pycypher_node_types;
        size_t pycypher_node_types_len;
    '''
    emit_begin = '''
          INIT_TABLE(pycypher_node_type_t, pycypher_node_types, TABLE({
    '''
    emit = '''
        {"CYPHER_AST_{TYPE}", CYPHER_AST_{TYPE}},
    '''
    emit_end = '}))'


THIS_DIR = os.path.dirname(os.path.abspath(__file__))


class Generator(object):
    def generate_operators_c(self, cypher_parser_h):
        yield '/* THIS FILE IS AUTOGENERATED, DO NOT EDIT */'
        yield '#include "operators.h"'
        yield '#include "table_utils.h"'
        yield ''
        yield PycypherOperator().declaration
        yield ''
        yield 'void pycypher_init_operators(void) {'
        for line in PycypherOperator().generate(cypher_parser_h):
            yield line
        yield '}'

    def generate_node_types_c(self, cypher_parser_h):
        yield '/* THIS FILE IS AUTOGENERATED, DO NOT EDIT */'
        yield '#include "node_types.h"'
        yield '#include "table_utils.h"'
        yield ''
        yield PycypherNodeType().declaration
        yield ''
        yield 'void pycypher_init_node_types(void) {'
        for line in PycypherNodeType().generate(cypher_parser_h):
            yield line
        yield '}'

    def generate_props_c(self, cypher_parser_h):
        PROPS = [
            PycypherDirectionProp,
            PycypherOperatorProp,
            PycypherOperatorListProp,
            PycypherBoolProp,
            PycypherStringProp,
            PycypherAstListProp,
            PycypherAstListPlusOneProp,
            PycypherAstProp,
        ]
        yield '/* THIS FILE IS AUTOGENERATED, DO NOT EDIT */'
        yield '#include "props.h"'
        yield '#include "table_utils.h"'
        yield ''
        for prop in PROPS:
            yield prop().declaration
        yield ''
        yield 'void pycypher_init_props(void) {'
        yield ''
        for prop in PROPS:
            for line in prop().generate(cypher_parser_h):
                yield line
            yield ''
        yield '}'

    def generate_getters_py(self, cypher_parser_h):
        yield '# THIS FILE IS AUTOGENERATED, DO NOT EDIT'
        yield 'from .decorators import pycypher_getter, pycypher_list_getter'
        yield ''
        yield ''
        yield 'class GettersMixin(object):'
        PROPS = [
            PycypherDirectionProp,
            PycypherOperatorProp,
            PycypherOperatorListProp,
            PycypherBoolProp_Get,
            PycypherBoolProp_Is,
            PycypherBoolProp_Has,
            PycypherStringProp,
            PycypherAstListProp,
            PycypherAstListPlusOneProp,
            PycypherAstProp,
        ]
        for prop in PROPS:
            for line in prop().generate_python(cypher_parser_h):
                yield line

    def generate_version_py(self, configure_ac):
        match = re.search(
            r'AC_INIT\(\[libcypher-parser\],\[(\d+\.\d+\.\d+)(\~[^\]]*)?\]\)',
            configure_ac,
        )
        if match is None:
            raise ValueError(
                'Could not find version number in ../configure.ac file'
            )
        yield '# THIS FILE IS AUTOGENERATED, DO NOT EDIT'
        yield "__version__ = '" + match.group(1) + "'"

    def main(self):
        filename_cypher_parser_h = os.path.join(
            THIS_DIR, '..', 'src', 'lib', 'cypher-parser.h.in',
        )
        with open(filename_cypher_parser_h) as file_cypher_parser_h:
            cypher_parser_h = file_cypher_parser_h.read()

        filename_configure_ac = os.path.join(
            THIS_DIR, '..', 'configure.ac'
        )
        with open(filename_configure_ac) as file_configure_ac:
            configure_ac = file_configure_ac.read()

        filename_license = os.path.join(
            THIS_DIR, '..', 'LICENSE'
        )
        with open(filename_license) as file_license:
            license = file_license.read()

        os.chdir(os.path.join(THIS_DIR, 'pycypher'))

        operators_c = "\n".join(self.generate_operators_c(cypher_parser_h))
        with open('operators.c', 'w') as file_out:
            file_out.write(operators_c)

        node_types_c = "\n".join(self.generate_node_types_c(cypher_parser_h))
        with open('node_types.c', 'w') as file_out:
            file_out.write(node_types_c)

        props_c = "\n".join(self.generate_props_c(cypher_parser_h))
        with open('props.c', 'w') as file_out:
            file_out.write(props_c)

        getters_py = "\n".join(self.generate_getters_py(cypher_parser_h))
        with open('getters.py', 'w') as file_out:
            file_out.write(getters_py)

        version_py = "\n".join(self.generate_version_py(configure_ac))
        with open('version.py', 'w') as file_out:
            file_out.write(version_py)

        with open('../LICENSE', 'w') as file_out:
            file_out.write(license)


if __name__ == "__main__":
    Generator().main()
