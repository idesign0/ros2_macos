#! /usr/bin/env python3
"""Hard Drive (or any other memory) monitor. Contains a the monitor node and its main function."""
# -*- coding: utf-8 -*-
#
# Software License Agreement (BSD License)
#
# Copyright (c) 2009, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the Willow Garage nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# \author Kevin Watts
# \author Antoine Lima

from pathlib import Path
from shutil import disk_usage
from socket import gethostname
from typing import List

from diagnostic_msgs.msg import DiagnosticStatus, KeyValue
from diagnostic_updater import Updater
from rcl_interfaces.msg import ParameterDescriptor, SetParametersResult
import rclpy
from rclpy.node import Node


FREE_PERCENT_LOW = 5
FREE_PERCENT_CRIT = 1
DICT_STATUS = {
    DiagnosticStatus.OK: 'OK',
    DiagnosticStatus.WARN: 'Warning',
    DiagnosticStatus.ERROR: 'Error',
}
DICT_USAGE = {
    DiagnosticStatus.OK: 'OK',
    DiagnosticStatus.WARN: 'Low Disk Space',
    DiagnosticStatus.ERROR: 'Very Low Disk Space',
}


class HDMonitor(Node):
    """
    Diagnostic node checking the remaining space on the specified hard drive.

    Three ROS parameters:
    - path: Path on the filesystem to check (string, default: home directory)
    - free_percent_low: Percentage at which to consider the space left as low
    - free_percent_crit: Percentage at which to consider the space left as critical
    """

    def __init__(self):
        hostname = gethostname()
        # Every invalid symbol is replaced by underscore.
        # isalnum() alone also allows invalid symbols depending on the locale
        cleaned_hostname = ''.join(
            c if (c.isascii() and c.isalnum()) else '_' for c in hostname)
        super().__init__(f'hd_monitor_{cleaned_hostname}')

        self._path = '~'
        self._free_percent_low = FREE_PERCENT_LOW
        self._free_percent_crit = FREE_PERCENT_CRIT

        self.add_on_set_parameters_callback(self.callback_config)
        self.declare_parameter('path', self._path,  ParameterDescriptor(
            description='Path in which to check remaining space.'))
        self.declare_parameter(
            'free_percent_low', self._free_percent_low,  ParameterDescriptor(
                description='Warning threshold.', type=int()))
        self.declare_parameter(
            'free_percent_crit', self._free_percent_crit,  ParameterDescriptor(
                description='Error threshold.', type=int()))

        self._updater = Updater(self)
        self._updater.setHardwareID(hostname)
        self._updater.add(f'{hostname} HD Usage', self.check_disk_usage)

    def callback_config(self, params: List[rclpy.Parameter]):
        """
        Retrieve ROS parameters.

        see the class documentation for declared parameters.
        """
        for param in params:
            if param.name == 'path':
                self._path = str(
                    Path(param.value).expanduser().resolve(strict=True)
                )
            elif param.name == 'free_percent_low':
                self._free_percent_low = param.value
            elif param.name == 'free_percent_crit':
                self._free_percent_crit = param.value

        return SetParametersResult(successful=True)

    def check_disk_usage(self, diag: DiagnosticStatus) -> DiagnosticStatus:
        """
        Compute the disk usage and derive a status from it.

        Task periodically ran by the diagnostic updater.
        """
        diag.level = DiagnosticStatus.OK

        total, _, free = disk_usage(self._path)
        percent = free / total * 100.0

        if percent > self._free_percent_low:
            diag.level = DiagnosticStatus.OK
        elif percent > self._free_percent_crit:
            diag.level = DiagnosticStatus.WARN
        else:
            diag.level = DiagnosticStatus.ERROR

        total_go = total // (1024 * 1024)
        diag.values.extend(
            [
                KeyValue(key='Name', value=self._path),
                KeyValue(key='Status', value=DICT_STATUS[diag.level]),
                KeyValue(key='Total (Go)', value=str(total_go)),
                KeyValue(key='Available (%)', value=str(round(percent, 1))),
            ]
        )

        diag.message = DICT_USAGE[diag.level]
        return diag


def main(args=None):
    """Run the HDMonitor class."""
    rclpy.init(args=args)

    node = HDMonitor()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
