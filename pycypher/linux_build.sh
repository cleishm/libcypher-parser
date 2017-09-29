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
cd "$(dirname -- "$0")"
cd ..

# install dependencies
docker build . -f pycypher/utils/pycypher_x86_64.dockerfile -t pycypher_x86_64
docker build . -f pycypher/utils/pycypher_i686.dockerfile -t pycypher_i686

rm -rf ./pycypher/dist

# generate bindings
./pycypher/generate.py

# build libcypher-parser and wheels
docker run --rm -it \
  -v `pwd`/pycypher:/project/pycypher \
  pycypher_x86_64 \
  bash -c '/project/pycypher/utils/build_wheels.sh'
docker run --rm -it \
  -v `pwd`/pycypher:/project/pycypher \
  pycypher_i686 \
  linux32 bash -c '/project/pycypher/utils/build_wheels.sh'

# if running on Travis CI, add a build tag to wheels so that it is possible
# to re-run the build and it will upload wheels with new tag so that fixes
# can be deployed to already-uploaded versions
if [ "$TRAVIS" == "true" ] && [ "$CI" == "true" ]; then
  docker run --rm -it \
    -v `pwd`/pycypher:/project/pycypher \
    pycypher_x86_64 \
    /opt/python/cp27-cp27m/bin/python \
      /project/pycypher/utils/add_build_tag_to_wheels.py \
      /project/pycypher/dist $TRAVIS_BUILD_NUMBER
  docker run --rm -it \
    -v `pwd`/pycypher:/project/pycypher \
    pycypher_i686 \
    linux32 /opt/python/cp27-cp27m/bin/python \
      /project/pycypher/utils/add_build_tag_to_wheels.py \
      /project/pycypher/dist $TRAVIS_BUILD_NUMBER
fi

# test
docker run --rm -it \
  -e 'TRAVIS' \
  -v `pwd`/pycypher/dist:/project/pycypher/dist \
  -v `pwd`/pycypher/utils:/project/pycypher/utils \
  quay.io/pypa/manylinux1_x86_64 \
  bash -c '/project/pycypher/utils/test_wheels.sh'
docker run --rm -it \
  -e 'TRAVIS' \
  -v `pwd`/pycypher/dist:/project/pycypher/dist \
  -v `pwd`/pycypher/utils:/project/pycypher/utils \
  quay.io/pypa/manylinux1_i686 \
  linux32 bash -c '/project/pycypher/utils/test_wheels.sh'
