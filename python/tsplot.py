import matplotlib.pyplot as plt
import numpy as np


def plot_ts_1d(ts, ax=None):
    fig = None

    if ax is None:
        fig = plt.figure()
        ax = fig.gca()
    ax.plot(ts)
    ax.set_ylim(0.0, 1.0)

    if fig is not None:
        plt.show()


def plot_ts_2d(ts, ax=None):
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


def plot_ts(ts):
    if len(ts.shape) == 1 or ts.shape[1] == 1:
        plot_ts_1d(ts)
    else:
        plot_ts_2d(ts)


def plot_multiple_ts(tss):
    fig = plt.figure()
    for i in range(len(tss)):
        if len(tss[i].shape) == 1 or tss[i].shape[1] == 1:
            plot_ts_1d(tss[i], fig.add_subplot(1, len(tss), i+1))
        else:
            plot_ts_2d(tss[i], fig.add_subplot(1, len(tss), i+1, projection='3d'))
    plt.show()
