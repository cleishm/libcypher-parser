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

import os
from setuptools import setup, Extension

bindings = Extension(
    'pycypher.bindings',
    sources=[
        'pycypher/bindings.c',
        'pycypher/node_types.c',
        'pycypher/operators.c',
        'pycypher/props.c',
        'pycypher/extract_props.c',
        'pycypher/parser.c',
    ],
    libraries=['cypher-parser'],
)

description = u"""
Python bindings for libcypher-parser.
""".strip()

long_description = u"""
Homepage: https://github.com/cleishm/libcypher-parser/tree/master/pycypher
""".strip()

setup(
    name=u'pycypher',
    version='0.0.4',
    description=description,
    long_description=long_description,
    license='Apache License 2.0',
    url=u'https://github.com/cleishm/libcypher-parser/tree/master/pycypher',
    author='Franciszek Piszcz, Google Inc.',
    author_email='franek@google.com',
    maintainer=u'Chris Leishman (http://github.com/cleishm)',
    maintainer_email=u'chris@leishman.org',
    classifiers=[
        u'Development Status :: 3 - Alpha',
        u'Intended Audience :: Developers',
        u'Programming Language :: Python :: 2',
        u'Programming Language :: Python :: 2.6',
        u'Programming Language :: Python :: 2.7',
        u'Programming Language :: Python :: 3',
        u'Programming Language :: Python :: 3.3',
        u'Programming Language :: Python :: 3.4',
        u'Programming Language :: Python :: 3.5',
        u'Programming Language :: Python :: 3.6',
        u'Programming Language :: Python :: Implementation :: CPython',
        u'Programming Language :: C',
        u'Operating System :: POSIX :: Linux',
        u'Operating System :: MacOS :: MacOS X',
        u'Topic :: Software Development :: Libraries',
    ],
    ext_modules=[bindings],
    packages=['pycypher', 'pycypher.tests'],
    include_package_data=True,
    zip_safe=False,
    install_requires=[],
)
