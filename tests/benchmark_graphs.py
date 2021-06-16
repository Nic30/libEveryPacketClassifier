# This import registers the 3D projection, but is otherwise unused.
import json
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import os

from benchmark import benchmarks
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm
import re


def load_data(key:str):
    # (x, y): float
    z_throughput = {}

    # cache misses

    for (alg, ruleset, result_dir) in benchmarks:
        ruleset_name = os.path.basename(ruleset)
        f_path = os.path.join(result_dir, f"{alg}_{ruleset_name}")
        # if not isfile(f_path) or f_path.endswith(".png"):
        #    continue
        # alg, ruleset, nominal_rule_cnt = ruleset.split("_")

        try:
            with open(f_path) as fp:
                results = json.load(fp)
                val = results[key]
                print(ruleset_name, 1 / float(val))  # 1Mpkt/t
        except FileNotFoundError:
            raise
            # val = None
        z_throughput[(alg, ruleset)] = val

    z_throughput = {k: 0 if v is None else float(v) for k, v in z_throughput.items()}
    x_algs = sorted(set(k[0] for k in z_throughput.keys()))
    y_rulesets = sorted(set(k[1] for k in z_throughput.keys()))
    # sort algs, slowest to highest x (background)
    x_algs = sorted(x_algs, key=lambda a:-sum(x[1] for x in z_throughput.items() if x[0][0] == a))
    return x_algs, y_rulesets, z_throughput


def plot_3d(key:str):
    # setup the figure and axes
    fig = plt.figure(figsize=(16, 16), dpi=100)
    ax1 = fig.add_subplot(1, 1, 1, projection='3d')
    # ax2 = fig.add_subplot(122, projection='3d')
    x_algs, y_rulesets, z_throughput = load_data(key)

    _x = np.arange(len(x_algs))
    _y = np.arange(len(y_rulesets))
    _xx, _yy = np.meshgrid(_x, _y)
    x, y = _xx.ravel(), _yy.ravel()

    top = np.array([
        [z_throughput[(alg, ruleset)] for alg in x_algs]
        for ruleset in y_rulesets
    ])
    _top = top
    top = top.astype(np.float)
    top = top.ravel()
    bottom = np.zeros_like(top)
    width = depth = 1
    # x, y, z : array-like
    #     The coordinates of the anchor point of the bars.
    # dx, dy, dz : float or array-like
    #     The width, depth, and height of the bars, respectively.
    ax1.bar3d(x, y, bottom, width, depth, top, shade=True)
    ax1.set_title(key)
    ax1.set_zlabel(re.match(".*(\(.*\))", key).group(1).replace("[", "").replace(")", "]"))
    # ax1.zaxis._set_scale('log')
    # ax1.set_zscale("log")  # https://github.com/matplotlib/matplotlib/issues/15248
    # print("x", x)
    # print("y", y)
    # print("top", _top)

    # ax1.plot_surface(x, y,  np.log10(_top), cmap=cm.viridis)
    # zticks = [1e-15, 1e-14, 1e-13, 1e-12, 1e-11, 1e-10]
    # ax1.set_zticks(np.log10(zticks))
    # ax1.set_zticklabels(zticks)

    ax1.set_xticks(range(len(x_algs)))
    ax1.set_xticklabels(x_algs)

    ax1.set_yticks(range(len(y_rulesets)))
    ax1.set_yticklabels([os.path.basename(r) for r in y_rulesets])
    plt.show()


def plot_2d(key):
    x_algs, y_rulesets, z_throughput = load_data(key)

    # plot lines
    fig, ax1 = plt.subplots(figsize=(8, 4.5))
    ax1.set_ylabel(key)
    ax1.set_xticks(range(len(y_rulesets)))
    ax1.set_xticklabels([os.path.basename(r) for r in y_rulesets])
    for a in x_algs:
        xy = [(k, v) for k, v in z_throughput.items() if k[0] == a]
        xy = sorted(xy, key=lambda i: y_rulesets.index(i[0][1]))
        plt.plot([y_rulesets.index(k[1]) for k, v in xy], [v for k, v in xy], '+', label=a,)
    ax1.set_yscale("log")
    plt.xticks(rotation=45)
    plt.legend()
    plt.show()


if __name__ == "__main__":
    key = "ClassificationTime(s)"
    # key = "ConstructionTime(ms)"  # classification per second

    plot_2d(key)
