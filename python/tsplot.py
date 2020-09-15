import matplotlib.pyplot as plt
import numpy as np


def plot_ts_1d(ts, ax=None):
    """
    Plot a one-dimensional time surface, either in a single figure or in a subplot

    :param ts: the time surface to be plotted
    :param ax: the axis of the subplot (leave it to None to create a new figure)
    """
    fig = None

    _ts = ts.reshape(-1, 1)

    if ax is None:
        fig = plt.figure()
        ax = fig.gca()
    ax.plot(_ts)
    ax.set_ylim(0.0, 1.0)

    if fig is not None:
        plt.show()


def plot_ts_2d(ts, ax=None):
    """
    Plot a two-dimensional time surface in a 3D plot, either in a single figure or in a subplot

    :param ts: the time surface to be plotted
    :param ax: the axis of the subplot (leave it to None to create a new figure)
    """
    fig = None

    if ax is None:
        fig = plt.figure()
        ax = fig.gca(projection='3d')
    x = np.arange(ts.shape[0])
    y = np.arange(ts.shape[1])
    x, y, = np.meshgrid(x, y)
    ax.plot_surface(x, y, ts)  # , cmap=cm.coolwarm, linewidth=0, antialiased=False)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlim(0.0, 1.0)

    if fig is not None:
        plt.show()


def is_1d(ts):
    """
    Check if a time surface is one-dimensional

    :param ts: the time surface to be checked
    :return: True if the time surface is one dimensional, False otherwise
    """
    return len(ts.shape) == 1 or ts.shape[0] == 1 or ts.shape[1] == 1


def plot_ts(ts):
    """
    Plot a time surface in its own figure

    This functions accepts both one and two-dimensional time surfaces.

    :param ts: the time surface to be plotted
    """
    if is_1d(ts):
        plot_ts_1d(ts)
    else:
        plot_ts_2d(ts)


def plot_multiple_ts(tss, arrangement=None):
    """
    Plot multiple time surfaces as subplot of the same figure

    This functions accepts both one and two-dimensional time surfaces.
    If no arrangment is specified, a best-effort arrangement that tries to minimize the number of empty subslots
    is performed.

    :param tss: a list of time surfaces
    :param arrangement: a tuple (rows, cols) that specifies how to arrange the subplots (optional)
    """
    fig = plt.figure()
    if arrangement is not None:
        rows, cols = arrangement
    else:
        n_plots = len(tss)
        if n_plots < 3:  # not much to do if 1 or 2 surfaces
            cols = n_plots
            rows = 1
        else:  # try to minimize the number of empty subplots (this covers also the case where there are 0 empty subplt)
            cols = -1
            empty = np.inf
            for tcols in [5, 4, 3]:
                trows = int(np.ceil(n_plots / tcols))
                tempty = tcols * trows - n_plots
                if tempty < empty:
                    cols = tcols
                    empty = tempty
            rows = int(np.ceil(n_plots / cols))

    for i in range(len(tss)):
        if is_1d(tss[i]):
            plot_ts_1d(tss[i], fig.add_subplot(rows, cols, i+1))
        else:
            plot_ts_2d(tss[i], fig.add_subplot(rows, cols, i+1, projection='3d'))
    plt.show()
