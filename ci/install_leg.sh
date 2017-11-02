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
  exit 1
fi

# install peg / leg
wget http://piumarta.com/software/peg/peg-0.1.18.tar.gz
tar -xzf peg-0.1.18.tar.gz
cd peg-0.1.18
make
sudo make install
cd ..
