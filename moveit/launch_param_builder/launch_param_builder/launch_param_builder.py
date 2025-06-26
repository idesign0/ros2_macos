# Copyright 2021 PickNik Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the PickNik Inc. nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


from pathlib import Path

from .utils import load_file, load_yaml, load_xacro, ParameterValueType

from ament_index_python.packages import get_package_share_directory


class ParameterBuilder(object):
    _package_path = None
    _parameters = {}

    def __init__(self, package_name: str):
        """
        @param package_name: Using this value, the path to the package's 'share'
            directory will be set to 'self._package_path'.
        """
        self._package_path = Path(get_package_share_directory(package_name))
        self._parameters = {}

    def yaml(self, file_path: str, parameter_namespace: str = None):
        """
        @param file_path: Path of the yaml file UNDER the top level directory
            of 'package_name' that is passed to the 'ParameterBuilder's
            constructor.
            E.g. If the yaml file is at %TOPDIR_PKG%/conf/foo.yaml,
                pass file_path = "conf/foo.yaml"
        @return The instance itself.
        """
        if parameter_namespace:
            if parameter_namespace in self._parameters:
                self._parameters[parameter_namespace].update(
                    load_yaml(self._package_path / file_path)
                )
            else:
                self._parameters[parameter_namespace] = load_yaml(
                    self._package_path / file_path
                )
        else:
            self._parameters.update(load_yaml(self._package_path / file_path))
        return self

    def file_parameter(self, parameter_name: str, file_path: str):
        """
        @deprecated: Use 'yaml' method instead.
        """
        self._parameters[parameter_name] = load_file(self._package_path / file_path)
        return self

    def xacro_parameter(
        self, parameter_name: str, file_path: str, mappings: dict = None
    ):
        self._parameters[parameter_name] = load_xacro(
            self._package_path / file_path, mappings=mappings
        )
        return self

    def parameter(self, parameter_name: str, parameter_value: ParameterValueType):
        """
        @summary: Set 'parameter_name' = 'parameter_value'.
        @return The instance itself.
        """
        self._parameters[parameter_name] = parameter_value
        return self

    def path_parameter(self, parameter_name: str, file_path: str):
        self._parameters[parameter_name] = str(self._package_path / file_path)
        return self

    def to_dict(self):
        return self._parameters
