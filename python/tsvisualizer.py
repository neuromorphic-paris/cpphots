import sys
import argparse

import numpy as np

from matplotlib.backends.qt_compat import QtCore, QtWidgets, QtGui
if QtCore.qVersion() >= "5.":
    from matplotlib.backends.backend_qt5agg import (
        FigureCanvas, NavigationToolbar2QT as NavigationToolbar)
else:
    from matplotlib.backends.backend_qt4agg import (
        FigureCanvas, NavigationToolbar2QT as NavigationToolbar)
from matplotlib.figure import Figure

from tsplot import plot_ts_1d, plot_ts_2d


class ControlToolbar(QtWidgets.QToolBar):
    """
    Toolbar to navigate the list of TS.

    Includes controls such as forward/back and to rearrange the plots.
    """

    def __init__(self, parent=None):
        super().__init__(parent)

        self._parent = parent

        self.setAllowedAreas(QtCore.Qt.TopToolBarArea | QtCore.Qt.BottomToolBarArea)

        # ts arrangement
        self.addWidget(QtWidgets.QLabel("Arrange:"))
        self._rows_le = QtWidgets.QLineEdit()
        self._rows_le.setMaximumWidth(25)
        self._rows_le.setAlignment(QtCore.Qt.AlignHCenter)
        self._rows_le.setFocusPolicy(QtCore.Qt.ClickFocus)
        self._rows_le.editingFinished.connect(self.change_arrangement)
        self.addWidget(self._rows_le)
        self.addWidget(QtWidgets.QLabel(" x "))
        self._cols_le = QtWidgets.QLineEdit()
        self._cols_le.setMaximumWidth(25)
        self._cols_le.setAlignment(QtCore.Qt.AlignHCenter)
        self._cols_le.setFocusPolicy(QtCore.Qt.ClickFocus)
        self._cols_le.editingFinished.connect(self.change_arrangement)
        self.addWidget(self._cols_le)

        # just some space
        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        self.addWidget(spacer)

        # commands
        act = self.addAction("Go back", parent.goback)
        act.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_Left))
        self._playact = self.addAction("Play", self.play)
        self._playact.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_Space))
        act = self.addAction("Advance", parent.advance)
        act.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_Right))

        # just some space
        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        self.addWidget(spacer)

        # current/total
        self._idx_le = QtWidgets.QLineEdit()
        self._idx_le.setMaximumWidth(50)
        self._idx_le.setAlignment(QtCore.Qt.AlignRight)
        self._idx_le.editingFinished.connect(self.change_number)
        self._idx_le.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.addWidget(self._idx_le)
        self._total = QtWidgets.QLabel()
        self.addWidget(self._total)

        self.reset()

    def play(self):
        """
        Calls parent.play() and set correct text
        """
        playing = self._parent.play()
        if playing:
            self._playact.setIconText("Pause")
        else:
            self._playact.setIconText("Play")

    def change_number(self):
        """
        Ask parent to change the displayed TS, after validating the input
        """
        try:
            idx = int(self._idx_le.text()) - 1
        except ValueError:
            self.reset()
            self._idx_le.clearFocus()
            return
        idx = self._parent.set_current(idx)
        self._idx_le.clearFocus()

    def update_number(self):
        """
        Update the number with the currently displayed TS
        """
        self._idx_le.setText(str(self._parent.current + 1))

    def reset(self):
        """
        Reset displayed information
        """
        rows, cols = self._parent.subplots
        self._rows_le.setText(str(rows))
        self._cols_le.setText(str(cols))
        if self._parent.total == 0:
            self._idx_le.setText("0")
        else:
            self._idx_le.setText(str(self._parent.current + 1))
        self._total.setText(f" / {self._parent.total}")

    def change_arrangement(self):
        """
        Ask the parent to change the arrangement of the plots, after validating the input
        """
        try:
            rows = int(self._rows_le.text())
            cols = int(self._cols_le.text())
        except ValueError:
            self.reset()
            self._rows_le.clearFocus()
            self._cols_le.clearFocus()
            return

        self._parent.rearrange_plots((rows, cols))
        self._rows_le.clearFocus()
        self._cols_le.clearFocus()


class TSDialog(QtWidgets.QDialog):
    """
    Dialog to ask the user information on the time surfaces
    """

    def __init__(self, parent=None):
        super().__init__(parent)

        # shape line edits
        shapelayout = QtWidgets.QHBoxLayout()

        self._rows_le = QtWidgets.QLineEdit(parent=self)
        self._rows_le.setMaximumWidth(40)
        shapelayout.addWidget(self._rows_le)

        shapelayout.addWidget(QtWidgets.QLabel(" x "))

        self._cols_le = QtWidgets.QLineEdit(parent=self)
        self._cols_le.setMaximumWidth(40)
        shapelayout.addWidget(self._cols_le)

        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        shapelayout.addWidget(spacer)

        # times
        self._times_cb = QtWidgets.QCheckBox("First column are times")

        # delimiter
        seplayout = QtWidgets.QHBoxLayout()
        seplayout.addWidget(QtWidgets.QLabel("Delimiter: "))

        self._sep_le = QtWidgets.QLineEdit("", parent=self)
        self._sep_le.setMaximumWidth(40)
        seplayout.addWidget(self._sep_le)

        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        seplayout.addWidget(spacer)

        # buttons
        btnlayout = QtWidgets.QHBoxLayout()
        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        btnlayout.addWidget(spacer)

        btn = QtWidgets.QPushButton("Ok", parent=self)
        btn.setDefault(True)
        btn.clicked.connect(self.accept)
        btnlayout.addWidget(btn)

        self.setLayout(QtWidgets.QVBoxLayout())
        self.layout().addWidget(QtWidgets.QLabel("Please enter time surfaces shape:"))
        self.layout().addLayout(shapelayout)
        self.layout().addLayout(seplayout)
        self.layout().addWidget(self._times_cb)
        self.layout().addLayout(btnlayout)

    def get_shape(self):
        """
        Return the shape inserted, or None if the shape is not valid
        """
        try:
            r = int(self._rows_le.text())
            c = int(self._cols_le.text())
        except ValueError:
            return None
        return (r, c)

    def get_hastimes(self):
        """
        Return true if the user has specified that the times are in the file
        """
        return self._times_cb.isChecked()

    def get_delimiter(self):
        """
        Return the delimiter specified by the user
        """
        if self._sep_le.text() == "":
            return None
        return self._sep_le.text()


class TSVisualizer(QtWidgets.QMainWindow):
    """
    Main window doing the plots
    """

    def __init__(self, datapath=None, subplots=(2, 2), shape=None, hastimes=False, delimiter=None):
        super().__init__()

        self._ctrltb = None

        self._data = None
        self._ndata = 0
        self._x = None
        self._y = None
        self._times = None

        # create central widget and layout
        self._main = QtWidgets.QWidget()
        self.setCentralWidget(self._main)
        layout = QtWidgets.QVBoxLayout(self._main)

        # canvas and plots
        self._canvas = FigureCanvas(Figure())  # figsize=(5, 3)))
        layout.addWidget(self._canvas)

        self.change_data(datapath, replot=False, shape=shape, hastimes=hastimes, delimiter=delimiter)

        self.rearrange_plots(subplots)

        # plot toolbar
        layout.addWidget(NavigationToolbar(self._canvas, self))

        # control toolbar
        self._ctrltb = ControlToolbar(self)
        layout.addWidget(self._ctrltb)

        # timer
        self._timer = QtCore.QTimer()
        self._timer.setInterval(200)
        self._timer.timeout.connect(self.advance)

        # menu
        menu = self.menuBar().addMenu("File")
        act = QtWidgets.QAction("Open...", self)
        act.triggered.connect(self.open_file)
        menu.addAction(act)

        act = QtWidgets.QAction("Shortcuts", self)
        act.triggered.connect(self.show_shortcuts)
        menu.addAction(act)

        act = QtWidgets.QAction("Quit", self)
        act.triggered.connect(QtWidgets.qApp.quit)
        menu.addAction(act)

    def open_file(self):
        """
        Select a file with a dialog and load its data
        """
        datapath = QtWidgets.QFileDialog.getOpenFileName(self, "Open TS file", ".")
        datapath = datapath[0]
        if datapath != "":
            self.change_data(datapath, replot=True)

    def change_data(self, datapath, shape=None, replot=False, hastimes=False, delimiter=None):
        """
        Change data displayed

        If no shape information is found in the file or passed as an argument, the shape is asked to the user
        """
        if datapath is None:
            self._data = None
            self._ndata = 0
            self._x = None
            self._y = None
            self._times = None
            return

        with open(datapath) as datafile:
            line = next(datafile).strip()
            if "TSDATA" in line:  # shape in file
                line = line.split(" ")
                rows = int(line[1])
                cols = int(line[2])
                if "TIMES" in line:
                    hastimes = True
                skipheader = 1
                delimiter = None
            else:

                if shape is None:

                    # ask sizes with dialog
                    diag = TSDialog()
                    res = diag.exec()

                    if res == QtWidgets.QDialog.Rejected or diag.get_shape() is None:
                        if self._data is None:
                            quit()
                        else:
                            return
                    rows = diag.get_shape()[0]
                    cols = diag.get_shape()[1]
                    hastimes = diag.get_hastimes()
                    delimiter = diag.get_delimiter()

                else:
                    rows = shape[0]
                    cols = shape[1]

                if rows == 1:
                    rows, cols = cols, rows

                skipheader = 0
        self._data = np.genfromtxt(datapath, skip_header=skipheader, delimiter=delimiter)

        # times
        if hastimes:
            self._times = self._data[:, 0]
            self._data = self._data[:, 1:]
        else:
            self._times = None

        if cols > 1:
            self._data = self._data.reshape(-1, rows, cols)

        self._ndata = self._data.shape[0]
        self._x = np.arange(self._data.shape[1])
        self._y = None
        if len(self._data.shape) > 2:
            self._y = np.arange(self._data.shape[2])

        if self._ctrltb is not None:
            self._ctrltb.reset()

        if replot:
            self.rearrange_plots()

    def is2d(self):
        """
        Return True if the TS are 2D, false otherwise
        """
        return self._y is None

    def rearrange_plots(self, subplots=None):
        """
        Change arrangement of plots, that is the number of rows and colums of subplots
        """
        if subplots is not None:
            self._subplots = subplots

        self._nsubplots = self._subplots[0] * self._subplots[1]

        self._canvas.figure.clf()
        if self.is2d():
            self._axs = self._canvas.figure.subplots(nrows=self._subplots[0],
                                                     ncols=self._subplots[1],
                                                     sharex=True, sharey=True)
        else:
            self._axs = self._canvas.figure.subplots(nrows=self._subplots[0],
                                                     ncols=self._subplots[1],
                                                     subplot_kw=dict(projection='3d'))
            self._canvas.mpl_connect("motion_notify_event", self.on_3dmove)
        self._axs = self._axs.reshape(self._nsubplots)

        self._current = 0

        self.plot_current()

    @property
    def subplots(self):
        """
        Shape of the suplots (rows, cols)
        """
        return self._subplots

    @property
    def current(self):
        """
        Index of the first displayed TS
        """
        return self._current

    @property
    def total(self):
        """
        Total number of TS loaded
        """
        return self._ndata

    def plot_current(self):
        """
        Plot currently selected TSs
        """
        if self._data is None:
            return

        for i, ax in enumerate(self._axs):

            data_idx = self._current + i
            if data_idx >= self._ndata:
                ax.clear()
                continue
            ts = self._data[data_idx]

            if self.is2d():
                lines = ax.get_lines()
                if len(lines) == 0:
                    plot_ts_1d(ts, ax)
                else:
                    lines[0].set_data(self._x, ts)
            else:
                ax.clear()
                plot_ts_2d(ts, ax)

            if self._times is None:
                ax.set_title(data_idx + 1)
            else:
                ax.set_title(self._times[data_idx])

        self._canvas.draw()

    def advance(self):
        """
        Increase index of displayed TS
        """
        self.set_current(self._current + self._nsubplots, wrap=True)

    def goback(self):
        """
        Decrease index of displayed TS
        """
        self.set_current(self._current - self._nsubplots, wrap=True)

    def play(self):
        """
        Start/stop automatic advance of display
        """
        if self._timer.isActive():
            self._timer.stop()
            return False

        self._timer.start()
        return True

    def set_current(self, newcurrent, wrap=False):
        """
        Seek to specific TS

        If wrap then go to the other end, otherwise just check boundaries.
        Calling this will also replot the TSs.
        """
        newcurrent = newcurrent - (newcurrent % self._nsubplots)

        if wrap:

            if newcurrent >= self._ndata:
                newcurrent = 0

            if newcurrent < 0:
                # we need to do this because of edge cases such as
                # self._ndata % self._nsubplots == 0
                newcurrent = self._nsubplots * (self._ndata // self._nsubplots)
                if newcurrent == self._ndata:
                    newcurrent -= self._nsubplots

        else:

            if newcurrent >= self._ndata:
                newcurrent = self._nsubplots * (self._ndata // self._nsubplots)
                if newcurrent == self._ndata:
                    newcurrent -= self._nsubplots

            if newcurrent < 0:
                newcurrent = 0

        self._current = newcurrent

        self.plot_current()

        self._ctrltb.update_number()

    def on_3dmove(self, event):
        """
        Event handler to sync all 3d subplots
        """
        ax = event.inaxes
        if ax is None:
            return

        for oax in self._axs:
            if oax == ax:
                continue

            if ax.button_pressed in ax._rotate_btn:
                oax.view_init(elev=ax.elev, azim=ax.azim)
            elif ax.button_pressed in ax._zoom_btn:
                oax.set_xlim3d(ax.get_xlim3d())
                oax.set_ylim3d(ax.get_ylim3d())
                oax.set_zlim3d(ax.get_zlim3d())

    def show_shortcuts(self):
        """
        Show dialog with keyboard shortcuts
        """
        mbox = QtWidgets.QMessageBox(self)
        mbox.setText("Keyboard shortcuts")
        mbox.setInformativeText("Left/Right arrows: navigate\nSpace: Play/Pause")
        mbox.setIcon(QtWidgets.QMessageBox.Information)
        mbox.exec()


def main():

    # args
    parser = argparse.ArgumentParser(description="Time surface visualizer")
    parser.add_argument("-r", "--rows", type=int, default=2, help="rows of plots")
    parser.add_argument("-c", "--cols", type=int, default=2, help="columns of plots")
    parser.add_argument("-tw", "--width", type=int, help="width of the TSs")
    parser.add_argument("-th", "--height", type=int, help="height of the TSs")
    parser.add_argument("-t", "--times", action="store_true", help="if set, times are expected in the file")
    parser.add_argument("-d", "--delimiter", type=str, default=None, help="string used to separate values")
    parser.add_argument("filepath", type=str, nargs="?", help="file to visualize")

    args = parser.parse_args()

    # check shape
    if args.width is not None and args.height is not None:
        shape = (args.width, args.height)
    else:
        shape = None

    # check for an already runnin qapp
    qapp = QtWidgets.QApplication.instance()
    if not qapp:
        qapp = QtWidgets.QApplication(sys.argv)

    # create main window
    app = TSVisualizer(datapath=args.filepath,
                       subplots=(args.rows, args.cols),
                       shape=shape,
                       hastimes=args.times,
                       delimiter=args.delimiter)
    app.show()
    app.activateWindow()
    app.raise_()
    qapp.exec_()


if __name__ == '__main__':
    main()
