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

import sys
import os
import os.path
import subprocess

from wheel.install import WheelFile


def main():
    os.chdir(sys.argv[1])
    build_tag = sys.argv[2]

    for wheel in os.listdir('.'):
        wheelfile = WheelFile(wheel)
        wheeldict = wheelfile.parsed_filename.groupdict()
        if wheeldict['build'] is not None:
            continue
        wheeldict['build'] = build_tag
        new_wheel = \
            '%(name)s-%(ver)s-%(build)s-%(pyver)s-%(abi)s-%(plat)s.whl' \
            % wheeldict
        subprocess.check_call(['mv', wheel, new_wheel])


if __name__ == '__main__':
    main()
