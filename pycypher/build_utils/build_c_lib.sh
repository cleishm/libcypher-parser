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

# fix libtool and autotools path mismatch
ln -s /usr/share/aclocal/* /usr/local/share/aclocal/

cd /project/

cp configure.ac configure.ac.original
perl -0777 -i -pe 's/LT_INIT/AC_PROG_LIBTOOL\nAC_SUBST(LIBTOOL_DEPS)/g' configure.ac
perl -0777 -i -pe 's/(GCC_CFLAGS=")/\1-lm /gm' configure.ac

function cleanup {
  cd /project/
  mv configure.ac.original configure.ac
}
trap cleanup EXIT

autoreconf -isvf 2>&1 \
  | grep -v 'is expanded from' \
  | grep -v 'suspicious cache-id' \
  | grep -v 'call detected in body' \
  | grep -v 'the top level'

./configure
make
make install
