# Copyright 2024 R. Kent James
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

# -- Path setup --------------------------------------------------------------

import sys

from sphinx.ext.autodoc.mock import mock

# -- Project information -----------------------------------------------------

copyright = 'The <launch_ros> Contributors. License: Apache License 2.0'  # noqa A001

# -- General configuration ---------------------------------------------------

master_doc = 'index'

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
    '.markdown': 'markdown',
}

# Add mocks for decorators to prevent rosdoc2 hangs

with mock(['launch']):
    import launch  # noqa F401
    module = sys.modules['launch']
sys.modules['launch'] = module

with mock(['launch.frontend']):
    import launch.frontend  # noqa F401
    module = sys.modules['launch.frontend']
    # These are dummy decorators
    module.expose_action = lambda _: lambda _: _
    module.expose_substitution = lambda _: lambda _: _
sys.modules['launch.frontend'] = module

with mock(['launch.some_substitutions_type']):
    import launch.some_substitutions_type  # noqa F401
    module = sys.modules['launch.some_substitutions_type']
    module.SomeSubstitutionsType_types_tuple = ()
sys.modules['launch.some_substitutions_type'] = module

# We still want autodoc to mock the base 'launch' module
sys.modules.pop('launch')
