#!/bin/bash
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
set -e -x

if [ "$TRAVIS" != "true" ] || [ "$CI" != "true" ]; then
  echo 'This script is not meant to run outside of Travis CI,'
  echo 'it requires sudo and installs some packages globally.'
  echo 'Use ./local_tests.sh for compiling and testing the extension module on OS X.'
  exit 1
fi

if [ "$TRAVIS_OS_NAME" != "osx" ]; then
  echo 'This script runs only on Mac OS X.'
  exit 1
fi

# install peg / leg
wget http://piumarta.com/software/peg/peg-0.1.18.tar.gz
tar -xzf peg-0.1.18.tar.gz
cd peg-0.1.18
make
make install
cd ..

# install libcypher-parser
autoreconf -isvf
./configure
make
make install

# generate the bindings
./pycypher/generate.py

# build and test wheels
export CIBW_PLATFORM=macos
export CIBW_TEST_REQUIRES='nose==1.3.7'
export CIBW_TEST_COMMAND='nosetests pycypher'
cd pycypher
pip install cibuildwheel==0.5.1
cibuildwheel --output-dir dist

# add build tag to wheels
/Library/Frameworks/Python.framework/Versions/2.7/bin/python \
  build_utils/add_build_tag_to_wheels.py dist $TRAVIS_BUILD_NUMBER
