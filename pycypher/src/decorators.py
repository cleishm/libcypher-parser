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

import functools


def get_prop_name_from_method(method):
    name = method.__name__
    if name.startswith('get_'):
        return name[4:]
    elif name.startswith('is_'):
        return name[3:]
    elif name.startswith('has_'):
        return name[4:]
    else:
        raise TypeError(
            'PyCypher getter method has to start with one of'
            ' "get_", "is_" or "has_".'
        )


def pycypher_getter(method):
    name = get_prop_name_from_method(method)
    @functools.wraps(method)
    def wrapper(self):
        if name in self._props:
            return self._props[name]
        if name in self._indirect_props:
            children = self._all_descendants()
        else:
            children = self._children
        children = [child for child in children if name in child._roles]
        if not children:
            return None
        elif len(children) == 1:
            return children[0]
        else:
            raise ValueError(
                'Multiple children with singleton role "%s".' % name
            )
    return wrapper


def pycypher_list_getter(role):
    def inner(method):
        name = get_prop_name_from_method(method)
        @functools.wraps(method)
        def wrapper(self):
            if name in self._props:
                return self._props[name]
            if role in self._indirect_props:
                children = self._all_descendants()
            else:
                children = self._children
            return [child for child in children if role in child._roles]
        return wrapper
    return inner
