# Copyright 2020 Open Source Robotics Foundation, Inc.
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

from contextlib import contextmanager
import os
import sys


@contextmanager
def add_dll_directories_from_env(env_name: str):
    """
    Add a list of directories from an environment variable to the DLL search path on Windows.

    Each directory in the environment variable that exists is passed to
    :func:`os.add_dll_directory`.

    If this function is called on a system other than Windows, then nothing happens and
    an empty list is returned.
    If this function is called with a version of Python less than 3.8, then nothing happens and
    an empty list is returned.

    Example usage::

        with add_dll_directories_from_env('PATH'):
            importlib.import_module('foo', package='bar')

    :param env_name: The name of the environment variable with DLL search paths.
    :return: A list of handles to directories.
    """
    dll_dir_handles = []
    # This function only makes sense on Windows and if the function 'add_dll_directory' exists
    if sys.platform == 'win32' and hasattr(os, 'add_dll_directory'):
        env_value = os.environ.get(env_name)
        path_list = env_value.split(os.pathsep) if env_value is not None else []
        for prefix_path in path_list:
            # Only add directories that exist
            if os.path.isdir(prefix_path):
                dll_dir_handles.append(os.add_dll_directory(os.path.abspath(prefix_path)))

    try:
        yield dll_dir_handles
    finally:
        for handle in dll_dir_handles:
            handle.close()
