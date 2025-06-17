# Copyright 2018 Open Source Robotics Foundation, Inc.
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

import copy
import os
import random
import subprocess
import sys
import tempfile
import threading

import rclpy


def require_environment_variable(name):
    """Get environment variable or raise if it does not exist."""
    env = os.getenv(name)
    if env is None:
        raise EnvironmentError('Missing environment variable "%s"' % name)
    return env


class HelperCommand:
    """Execute a command in the background."""

    def __init__(self, command):
        self._env = dict(os.environ)
        self._command = copy.deepcopy(command)

        # Execute python files using same python used to start this test
        if command[0][-3:] == '.py':
            self._command = list(self._command)
            self._command.insert(0, sys.executable)
            self._env['PYTHONUNBUFFERED'] = '1'

    def __enter__(self):
        self._proc = subprocess.Popen(self._command, env=self._env)
        return self

    def __exit__(self, t, v, tb):
        self._proc.kill()


class TemporaryFileWithContent:
    """Create a named temporary file with content."""

    def __init__(self, content):
        self._tempdir = tempfile.TemporaryDirectory(prefix='test_cli_')
        self._content = content

    def __enter__(self):
        directory = self._tempdir.__enter__()
        name = ''.join(random.choice('abcdefghijklmnopqrstuvwxyz') for _ in range(10))
        self._filename = os.path.join(directory, name)
        self._file = open(self._filename, mode='w')
        self._file.write(self._content)
        self._file.flush()
        # close so it can be opened again on windows
        self._file.close()
        return self._file.name

    def __exit__(self, t, v, tb):
        self._tempdir.__exit__(t, v, tb)


class BackgroundExecutor:
    """Spin an executor in the background."""

    def __init__(self, node, time_between_spins=0.25):
        self._node = node
        self._time_between_spins = time_between_spins

    def __enter__(self):
        self._stop = threading.Event()
        self._thr = threading.Thread(target=self._run, daemon=True)
        self._thr.start()

    def _run(self):
        while not self._stop.is_set():
            rclpy.spin_once(self._node, timeout_sec=self._time_between_spins)

    def __exit__(self, t, v, tb):
        self._stop.set()
        self._thr.join()
