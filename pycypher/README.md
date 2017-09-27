# pycypher
[![Build Status](https://travis-ci.org/cleishm/libcypher-parser.svg?branch=master)](https://travis-ci.org/cleishm/libcypher-parser)
[![Version](https://img.shields.io/pypi/v/pycypher.svg)](https://pypi.python.org/pypi/pycypher)

Python bindings for libcypher-parser.

### Getting started
You don't need to have libcypher-parser installed to use it.
The distribution contains a statically linked copy of libcypher-parser. You
only need a CPython installation on a Linux or a Mac OS X system.

```console
$ pip install pycypher
```
```python
from pycypher import parse_query
(q,) = parse_query("MATCH (a) RETURN (a)")
print q.to_json()
```

If pip does not find pycypher package, please try:

    $ pip install --upgrade pip
    $ pip install wheel

If you encounter installation problems on OS X, please check ABI tag of your
python installation:
```console
python -c 'from wheel.pep425tags import get_abi_tag; print get_abi_tag()'
```
If the output contains the letter `u` (for example `cp27mu`), please install Python
from http://python.org. Currently there are only OS X packages compatible with python
compiled without the "wide unicode" flag.

### Documentation
* [User guide](docs/UserGuide.md)
* [Developer guide](docs/DeveloperGuide.md)

### License
PyCypher is licensed under the [Apache License, Version 2.0](
http://www.apache.org/licenses/LICENSE-2.0).

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.
