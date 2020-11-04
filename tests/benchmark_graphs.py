# This import registers the 3D projection, but is otherwise unused.
import json
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import os

from benchmark import benchmarks
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm
import re

if __name__ == "__main__":
    # setup the figure and axes
    fig = plt.figure(figsize=(16, 16),  dpi=100)
    ax1 = fig.add_subplot(1,1,1, projection='3d')
    #ax2 = fig.add_subplot(122, projection='3d')
    
    # (x, y): float
    z_throughput = {}
    # key = "ClassificationTime(s)"
    key = "ConstructionTime(ms)" # classification per second
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
        except FileNotFoundError:
            raise
            #val = None
        z_throughput[(alg, ruleset)] = val

    z_throughput = {k: 0 if v is None else float(v) for k, v in z_throughput.items()}
    x_algs = sorted(set(k[0] for k in z_throughput.keys()))
    y_rulesets = sorted(set(k[1] for k in z_throughput.keys()))
    # sort algs, slowest to highest x (background)
    x_algs = sorted(x_algs, key=lambda a: -sum(x[1] for x in z_throughput.items() if x[0][0] == a ))
    
    _x = np.arange(len(x_algs))
    _y = np.arange(len(y_rulesets))
    _xx, _yy = np.meshgrid(_x, _y)
    x, y = _xx.ravel(), _yy.ravel()
    
    top = np.array([
        [z_throughput[(alg, ruleset)] for alg in x_algs]
        for ruleset in y_rulesets
    ])
    #print(top)
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
    #ax1.set_zscale("log")

    ax1.set_xticks(range(len(x_algs)))
    ax1.set_xticklabels(x_algs)
    
    ax1.set_yticks(range(len(y_rulesets)))
    ax1.set_yticklabels([os.path.basename(r) for r in y_rulesets])
    
    plt.show()
