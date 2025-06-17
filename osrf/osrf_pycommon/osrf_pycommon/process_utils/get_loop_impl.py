# Copyright 2014 Open Source Robotics Foundation, Inc.
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

import os
import threading
import warnings

_thread_local = threading.local()


def get_loop_impl(asyncio):
    # See the note in
    # https://docs.python.org/3/library/asyncio-eventloop.html#asyncio.get_event_loop # noqa
    # In short, between Python 3.10.0 and 3.10.8, this unconditionally raises a
    # DeprecationWarning.  But after 3.10.8, it only raises the warning if
    # ther eis no current loop set in the policy.  Since we are setting a loop
    # in the policy, this warning is spurious, and will go away once we get
    # away from Python 3.10.6 (verified with Python 3.11.3).
    with warnings.catch_warnings():
        warnings.filterwarnings(
            'ignore',
            'There is no current event loop',
            DeprecationWarning)

        global _thread_local
        if getattr(_thread_local, 'loop_has_been_setup', False):
            return asyncio.get_event_loop()
        # Setup this thread's loop and return it
        if os.name == 'nt':
            try:
                loop = asyncio.get_event_loop()
                if not isinstance(loop, asyncio.ProactorEventLoop):
                    # Before replacing the existing loop, explicitly
                    # close it to prevent an implicit close during
                    # garbage collection, which may or may not be a
                    # problem depending on the loop implementation.
                    loop.close()
                    loop = asyncio.ProactorEventLoop()
                    asyncio.set_event_loop(loop)
            except (RuntimeError, AssertionError):
                loop = asyncio.ProactorEventLoop()
                asyncio.set_event_loop(loop)
        else:
            try:
                loop = asyncio.get_event_loop()
            except (RuntimeError, AssertionError):
                loop = asyncio.new_event_loop()
                asyncio.set_event_loop(loop)
        _thread_local.loop_has_been_setup = True

    return loop
