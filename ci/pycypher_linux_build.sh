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
set -e -u
cd "$(dirname -- "$0")"
cd ..

# install dependencies
docker build . -f ci/pycypher_x86_64.dockerfile -t pycypher_x86_64
docker build . -f ci/pycypher_i686.dockerfile -t pycypher_i686

rm -rf ./pycypher/dist

# generate bindings
./pycypher/generate.py

BUILD_WHEELS='
  set -e -u

  cp -r /project/pycypher /pycypher
  rm -rf /pycypher/build
  rm -rf /pycypher/dist
  rm -rf /pycypher/*.pyc
  rm -rf /pycypher/*.so

  # Compile wheels
  for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" wheel /pycypher/ -w /wheelhouse/
  done

  # Bundle external shared libraries into the wheels
  for whl in /wheelhouse/*.whl; do
    auditwheel repair "$whl" -w /project/pycypher/dist/
  done
  chown -R $(stat -c "%u:%g" /project/pycypher) /project/pycypher
'

# build libcypher-parser and wheels
docker run --rm -it \
  -v `pwd`/pycypher:/project/pycypher \
  pycypher_x86_64 \
  bash -c "$BUILD_WHEELS"
docker run --rm -it \
  -v `pwd`/pycypher:/project/pycypher \
  pycypher_i686 \
  linux32 bash -c "$BUILD_WHEELS"

ADD_BUILD_TAG_TO_WHEELS='
import sys, os, os.path, subprocess
from wheel.install import WheelFile

os.chdir(sys.argv[1])
build_tag = sys.argv[2]

for wheel in os.listdir("."):
    wheelfile = WheelFile(wheel)
    wheeldict = wheelfile.parsed_filename.groupdict()
    if wheeldict["build"] is not None:
        continue
    wheeldict["build"] = build_tag
    new_wheel = "%(name)s-%(ver)s-%(build)s-%(pyver)s-%(abi)s-%(plat)s.whl" % wheeldict
    subprocess.check_call(["mv", wheel, new_wheel])
'

# if running on Travis CI, add a build tag to wheels so that it is possible
# to re-run the build and it will upload wheels with new tag so that fixes
# can be deployed to already-uploaded versions
if [ "${TRAVIS:-false}" == "true" ] && [ "${CI:-false}" == "true" ]; then
  docker run --rm -it \
    -v `pwd`/pycypher:/project/pycypher \
    pycypher_x86_64 \
    /opt/python/cp27-cp27m/bin/python \
      -c "$ADD_BUILD_TAG_TO_WHEELS" \
      /project/pycypher/dist $TRAVIS_BUILD_NUMBER
  docker run --rm -it \
    -v `pwd`/pycypher:/project/pycypher \
    pycypher_i686 \
    linux32 /opt/python/cp27-cp27m/bin/python \
      -c "$ADD_BUILD_TAG_TO_WHEELS" \
      /project/pycypher/dist $TRAVIS_BUILD_NUMBER
fi

TEST_WHEELS='
  set -e -u

  for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/pip" install nose
    "${PYBIN}/pip" install pycypher --no-index -f /project/pycypher/dist/
    "${PYBIN}/nosetests" pycypher
  done
'

# test
docker run --rm -it \
  -e 'TRAVIS' \
  -v `pwd`/pycypher/dist:/project/pycypher/dist \
  -v `pwd`/pycypher/build_utils:/project/pycypher/build_utils \
  quay.io/pypa/manylinux1_x86_64 \
  bash -c "$TEST_WHEELS"
docker run --rm -it \
  -e 'TRAVIS' \
  -v `pwd`/pycypher/dist:/project/pycypher/dist \
  -v `pwd`/pycypher/build_utils:/project/pycypher/build_utils \
  quay.io/pypa/manylinux1_i686 \
  linux32 bash -c "$TEST_WHEELS"
