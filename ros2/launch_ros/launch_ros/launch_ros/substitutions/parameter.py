# Copyright 2022 Open Source Robotics Foundation, Inc.
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

"""Module for the Parameter substitution."""

from typing import Iterable
from typing import List
from typing import Text

from launch.frontend import expose_substitution
from launch.launch_context import LaunchContext
from launch.some_substitutions_type import SomeSubstitutionsType
from launch.substitution import Substitution
from launch.substitutions.substitution_failure import SubstitutionFailure
from launch.utilities import normalize_to_list_of_substitutions
from launch.utilities import perform_substitutions


@expose_substitution('param')
class Parameter(Substitution):
    """
    Substitution that tries to get a parameter that was set by SetParameter.

    :raise: SubstitutionFailure when param is not found
    """

    def __init__(
        self,
        name: SomeSubstitutionsType,
    ) -> None:
        """Create a Parameter substitution."""
        super().__init__()
        self.__name = normalize_to_list_of_substitutions(name)

    @classmethod
    def parse(cls, data: Iterable[SomeSubstitutionsType]):
        """Parse a Parameter substitution."""
        if not data or len(data) != 1:
            raise AttributeError('param substitutions expect 1 argument')
        kwargs = {'name': data[0]}
        return cls, kwargs

    @property
    def name(self) -> List[Substitution]:
        """Getter for name."""
        return self.__name

    def describe(self) -> Text:
        """Return a description of this substitution as a string."""
        name_str = ' + '.join([sub.describe() for sub in self.name])
        return '{}(name={})'.format(self.__class__.__name__, name_str)

    def perform(self, context: LaunchContext) -> Text:
        """Perform the substitution."""
        name = perform_substitutions(context, self.name)
        params_container = context.launch_configurations.get('global_params', None)
        for param in params_container:
            if isinstance(param, tuple):
                if param[0] == name:
                    return str(param[1])
        raise SubstitutionFailure("parameter '{}' not found".format(name))
