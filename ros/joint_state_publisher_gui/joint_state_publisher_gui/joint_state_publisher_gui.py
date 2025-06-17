# Software License Agreement (BSD License)
#
# Copyright (c) 2010, Willow Garage, Inc.
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
#  * Neither the name of Willow Garage, Inc. nor the names of its
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

import argparse
import random
import signal
import sys
import threading

import rclpy

from python_qt_binding.QtCore import pyqtSlot
from python_qt_binding.QtCore import Qt
from python_qt_binding.QtCore import Signal
from python_qt_binding.QtGui import QFont
from python_qt_binding.QtWidgets import QApplication
from python_qt_binding.QtWidgets import QFormLayout
from python_qt_binding.QtWidgets import QGridLayout
from python_qt_binding.QtWidgets import QHBoxLayout
from python_qt_binding.QtWidgets import QLabel
from python_qt_binding.QtWidgets import QLineEdit
from python_qt_binding.QtWidgets import QMainWindow
from python_qt_binding.QtWidgets import QPushButton
from python_qt_binding.QtWidgets import QSlider
from python_qt_binding.QtWidgets import QScrollArea
from python_qt_binding.QtWidgets import QVBoxLayout
from python_qt_binding.QtWidgets import QWidget

from joint_state_publisher.joint_state_publisher import JointStatePublisher

from joint_state_publisher_gui.flow_layout import FlowLayout

RANGE = 10000
LINE_EDIT_WIDTH = 45
SLIDER_WIDTH = 200
INIT_NUM_SLIDERS = 7  # Initial number of sliders to show in window

# Defined by style - currently using the default style
DEFAULT_WINDOW_MARGIN = 11
DEFAULT_CHILD_MARGIN = 9
DEFAULT_BTN_HEIGHT = 25
DEFAULT_SLIDER_HEIGHT = 64  # Is the combination of default heights in Slider

# Calculate default minimums for window sizing
MIN_WIDTH = SLIDER_WIDTH + DEFAULT_CHILD_MARGIN * 4 + DEFAULT_WINDOW_MARGIN * 2
MIN_HEIGHT = DEFAULT_BTN_HEIGHT * 2 + DEFAULT_WINDOW_MARGIN * 2 + DEFAULT_CHILD_MARGIN * 2

class Slider(QWidget):
    def __init__(self, name):
        super().__init__()

        self.joint_layout = QVBoxLayout()
        self.row_layout = QHBoxLayout()

        font = QFont("Helvetica", 9, QFont.Bold)
        self.label = QLabel(name)
        self.label.setFont(font)
        self.row_layout.addWidget(self.label)

        self.display = QLineEdit("0.00")
        self.display.setAlignment(Qt.AlignRight)
        self.display.setFont(font)
        self.display.setReadOnly(True)
        self.display.setFixedWidth(LINE_EDIT_WIDTH)
        self.row_layout.addWidget(self.display)

        self.joint_layout.addLayout(self.row_layout)

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setFont(font)
        self.slider.setRange(0, RANGE)
        self.slider.setValue(int(RANGE / 2))
        self.slider.setFixedWidth(SLIDER_WIDTH)

        self.joint_layout.addWidget(self.slider)

        self.setLayout(self.joint_layout)

    def remove(self):
        self.joint_layout.removeWidget(self.slider)
        self.slider.setParent(None)

        self.row_layout.removeWidget(self.display)
        self.display.setParent(None)

        self.row_layout.removeWidget(self.label)
        self.label.setParent(None)

        self.row_layout.setParent(None)


class JointStatePublisherGui(QMainWindow):
    sliderUpdateTrigger = Signal()
    initialize = Signal()

    def __init__(self, title, jsp):
        super(JointStatePublisherGui, self).__init__()

        self.joint_map = {}

        self.setWindowTitle(title)

        # Button for randomizing the sliders
        self.rand_button = QPushButton('Randomize', self)
        self.rand_button.clicked.connect(self.randomizeEvent)

        # Button for centering the sliders
        self.ctr_button = QPushButton('Center', self)
        self.ctr_button.clicked.connect(self.centerEvent)

        # Scroll area widget contents - layout
        self.scroll_layout = FlowLayout()

        # Scroll area widget contents
        self.scroll_widget = QWidget()
        self.scroll_widget.setLayout(self.scroll_layout)

        # Scroll area for sliders
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setWidget(self.scroll_widget)

        # Main layout
        self.main_layout = QVBoxLayout()

        # Add buttons and scroll area to main layout
        self.main_layout.addWidget(self.rand_button)
        self.main_layout.addWidget(self.ctr_button)
        self.main_layout.addWidget(self.scroll_area)

        # central widget
        self.central_widget = QWidget()
        self.central_widget.setLayout(self.main_layout)
        self.setCentralWidget(self.central_widget)

        self.jsp = jsp
        self.jsp.set_source_update_cb(self.sliderUpdateCb)
        self.jsp.set_robot_description_update_cb(self.initializeCb)

        self.running = True
        self.sliders = {}

        # Setup signal for initializing the window
        self.initialize.connect(self.initializeSliders)
        # Set up a signal for updating the sliders based on external joint info
        self.sliderUpdateTrigger.connect(self.updateSliders)

        # Tell self to draw sliders in case the JointStatePublisher already has a robot_description
        self.initialize.emit()

    def initializeSliders(self):
        self.joint_map = {}

        for sl, _ in self.sliders.items():
            self.scroll_layout.removeWidget(sl)
            sl.remove()

        ### Generate sliders ###
        for name in self.jsp.joint_list:
            if name not in self.jsp.free_joints:
                continue
            joint = self.jsp.free_joints[name]

            if joint['min'] == joint['max']:
                continue

            slider = Slider(name)

            self.joint_map[name] = {'display': slider.display, 'slider': slider.slider, 'joint': joint}

            self.scroll_layout.addWidget(slider)
            # Connect to the signal provided by QSignal
            slider.slider.valueChanged.connect(lambda event,name=name: self.onSliderValueChangedOne(name))

            self.sliders[slider] = slider

        # Set zero positions read from parameters
        self.centerEvent(None)

        # Set size of min size of window based on number of sliders.
        if len(self.sliders) >= INIT_NUM_SLIDERS:  # Limits min size to show INIT_NUM_SLIDERS
            num_sliders = INIT_NUM_SLIDERS
        else:
            num_sliders = len(self.sliders)
        scroll_layout_height = num_sliders * DEFAULT_SLIDER_HEIGHT
        scroll_layout_height += (num_sliders + 1) * DEFAULT_CHILD_MARGIN
        self.setMinimumSize(MIN_WIDTH, scroll_layout_height + MIN_HEIGHT)

        self.sliderUpdateTrigger.emit()

    def sliderUpdateCb(self):
        self.sliderUpdateTrigger.emit()

    def initializeCb(self):
        self.initialize.emit()

    def onSliderValueChangedOne(self, name):
        # A slider value was changed, but we need to change the joint_info metadata.
        joint_info = self.joint_map[name]
        slidervalue = joint_info['slider'].value()
        joint = joint_info['joint']
        joint['position'] = self.sliderToValue(slidervalue, joint)
        joint_info['display'].setText("%.3f" % joint['position'])

    @pyqtSlot()
    def updateSliders(self):
        for name, joint_info in self.joint_map.items():
            joint = joint_info['joint']
            slidervalue = self.valueToSlider(joint['position'], joint)
            joint_info['slider'].setValue(slidervalue)

    def centerEvent(self, event):
        self.jsp.get_logger().info("Centering")
        for name, joint_info in self.joint_map.items():
            joint = joint_info['joint']
            joint_info['slider'].setValue(self.valueToSlider(joint['zero'], joint))

    def randomizeEvent(self, event):
        self.jsp.get_logger().info("Randomizing")
        for name, joint_info in self.joint_map.items():
            joint = joint_info['joint']
            joint_info['slider'].setValue(
                self.valueToSlider(random.uniform(joint['min'], joint['max']), joint))

    def valueToSlider(self, value, joint):
        return int((value - joint['min']) * float(RANGE) / (joint['max'] - joint['min']))

    def sliderToValue(self, slider, joint):
        pctvalue = slider / float(RANGE)
        return joint['min'] + (joint['max']-joint['min']) * pctvalue

    def closeEvent(self, event):
        self.running = False

    def loop(self):
        while self.running:
            rclpy.spin_once(self.jsp, timeout_sec=0.1)


def main():
    # Initialize rclpy with the command-line arguments
    rclpy.init()

    # Strip off the ROS 2-specific command-line arguments
    stripped_args = rclpy.utilities.remove_ros_args(args=sys.argv)
    parser = argparse.ArgumentParser()
    parser.add_argument('urdf_file', help='URDF file to use', nargs='?', default=None)

    # Parse the remaining arguments, noting that the passed-in args must *not*
    # contain the name of the program.
    parsed_args = parser.parse_args(args=stripped_args[1:])

    app = QApplication(sys.argv)
    jsp_gui = JointStatePublisherGui('Joint State Publisher',
                                     JointStatePublisher(parsed_args.urdf_file))

    jsp_gui.show()

    threading.Thread(target=jsp_gui.loop).start()
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
