#!/usr/bin/env python

# Copyright (c) 2011, Ye Cheng, Dorian Scholz
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above
#     copyright notice, this list of conditions and the following
#     disclaimer in the documentation and/or other materials provided
#     with the distribution.
#   * Neither the name of the TU Darmstadt nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from python_qt_binding import QT_BINDING, QT_BINDING_VERSION

try:
    from pkg_resources import parse_version
except:
    import re

    def parse_version(s):
        return [int(x) for x in re.sub(r'(\.0+)*$', '', s).split('.')]

if QT_BINDING == 'pyside':
    qt_binding_version = QT_BINDING_VERSION.replace('~', '-')
    if parse_version(qt_binding_version) <= parse_version('1.1.2'):
        raise ImportError('A PySide version newer than 1.1.0 is required.')

from python_qt_binding.QtCore import Slot, Qt, qVersion, qWarning, Signal
from python_qt_binding.QtGui import QColor
from python_qt_binding.QtWidgets import QWidget, QVBoxLayout, QSizePolicy

import operator
import matplotlib
if qVersion().startswith('5.'):
    if QT_BINDING == 'pyside':
        if parse_version(matplotlib.__version__) < parse_version('2.1.0'):
            raise ImportError('A newer matplotlib is required (at least 2.1.0 for PySide 2)')
    if parse_version(matplotlib.__version__) < parse_version('1.4.0'):
        raise ImportError('A newer matplotlib is required (at least 1.4.0 for Qt 5)')
    try:
        matplotlib.use('Qt5Agg')
        from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
    except ImportError:
        # work around bug in dateutil
        import sys
        import thread
        sys.modules['_thread'] = thread
        from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
    from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
elif qVersion().startswith('4.'):
    if parse_version(matplotlib.__version__) < parse_version('1.1.0'):
        raise ImportError('A newer matplotlib is required (at least 1.1.0 for Qt 4)')
    try:
        matplotlib.use('Qt4Agg')
        from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
    except ImportError:
        # work around bug in dateutil
        import sys
        import thread
        sys.modules['_thread'] = thread
        from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
    try:
        from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
    except ImportError:
        from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
else:
    raise NotImplementedError('Unsupport Qt version: %s' % qVersion())

from matplotlib.figure import Figure

import numpy


class MatDataPlot(QWidget):

    class Canvas(FigureCanvas):

        """Ultimately, this is a QWidget (as well as a FigureCanvasAgg, etc.)."""

        def __init__(self, parent=None):
            super(MatDataPlot.Canvas, self).__init__(Figure())
            self.axes = self.figure.add_subplot(111)
            self.axes.grid(True, color='gray')
            self.safe_tight_layout()
            self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
            self.updateGeometry()

        def resizeEvent(self, event):
            super(MatDataPlot.Canvas, self).resizeEvent(event)
            self.safe_tight_layout()

        def safe_tight_layout(self):
            """
            Deal with "ValueError: bottom cannot be >= top" bug in older matplotlib versions
            (before v2.2.3)

            References:
                - https://github.com/matplotlib/matplotlib/pull/10915
                - https://github.com/ros-visualization/rqt_plot/issues/35
            """
            try:
                if self.figure.get_figheight() == 0 or self.figure.get_figwidth() == 0:
                    return
                self.figure.tight_layout()
            except ValueError:
                if parse_version(matplotlib.__version__) >= parse_version('2.2.3'):
                    raise

    limits_changed = Signal()

    def __init__(self, parent=None):
        super(MatDataPlot, self).__init__(parent)
        self._canvas = MatDataPlot.Canvas()
        self._toolbar = NavigationToolbar(self._canvas, self._canvas)
        vbox = QVBoxLayout()
        vbox.addWidget(self._toolbar)
        vbox.addWidget(self._canvas)
        self.setLayout(vbox)

        self._curves = {}
        self._current_vline = None
        self._canvas.mpl_connect('button_release_event', self._limits_changed)

    def _limits_changed(self, event):
        self.limits_changed.emit()

    def add_curve(self, curve_id, curve_name, curve_color=QColor(Qt.blue), markers_on=False):

        # adding an empty curve and change the limits, so save and restore them
        x_limits = self.get_xlim()
        y_limits = self.get_ylim()
        if markers_on:
            marker_size = 3
        else:
            marker_size = 0
        line = self._canvas.axes.plot([], [], 'o-', markersize=marker_size, label=curve_name,
                                      linewidth=1, picker=5, color=curve_color.name())[0]
        self._curves[curve_id] = line
        self._update_legend()
        self.set_xlim(x_limits)
        self.set_ylim(y_limits)

    def remove_curve(self, curve_id):
        curve_id = str(curve_id)
        if curve_id in self._curves:
            self._curves[curve_id].remove()
            del self._curves[curve_id]
            self._update_legend()

    def _update_legend(self):
        handles, labels = self._canvas.axes.get_legend_handles_labels()
        if handles:
            hl = sorted(zip(handles, labels), key=operator.itemgetter(1))
            handles, labels = zip(*hl)
        self._canvas.axes.legend(handles, labels, loc='upper left')

    def set_values(self, curve, data_x, data_y):
        line = self._curves[curve]
        line.set_data(data_x, data_y)

    def redraw(self):
        self._canvas.axes.grid(True, color='gray')
        self._canvas.draw()

    def vline(self, x, color):
        # convert color range from (0,255) to (0,1.0)
        matcolor = (color[0] / 255.0, color[1] / 255.0, color[2] / 255.0)
        if self._current_vline:
            self._current_vline.remove()
        self._current_vline = self._canvas.axes.axvline(x=x, color=matcolor)

    def set_xlim(self, limits):
        self._canvas.axes.set_xbound(lower=limits[0], upper=limits[1])

    def set_ylim(self, limits):
        self._canvas.axes.set_ybound(lower=limits[0], upper=limits[1])

    def get_xlim(self):
        return list(self._canvas.axes.get_xbound())

    def get_ylim(self):
        return list(self._canvas.axes.get_ybound())
