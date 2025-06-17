# Copyright 2016 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

try:
    try:
        import asyncio
    except ImportError:
        pass
    else:
        from .impl import async_execute_process
        from .impl import get_loop

        __all__ = [
            'async_execute_process',
            'asyncio',
            'get_loop',
        ]
except SyntaxError:
    pass

"""
This exists as a package rather than a module so that it can be excluded from
install in the setup.py when installing for Python2.
"""
