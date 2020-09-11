# This import registers the 3D projection, but is otherwise unused.
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import

import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
#    fig = plt.figure()
#    ax = fig.add_subplot(111, projection='3d')
#    
#    _x = [1, 2, 3, 2, 5, 2, 6, 8, 6, 7]
#    _y = [10, 10, 20, 50, 20, 20, 30, 10, 40, 50]
#    _xx, _yy = np.meshgrid(_x, _y)
#    x, y = _xx.ravel(), _yy.ravel()
#    _z = np.array([105, 25, 26, 74, 39, 85, 74, 153, 52, 98] + 
#                  list(range(10)))
#    
#    # There may be an easier way to do this, but I am not aware of it
#    z = np.zeros(len(x))
#    for i in range(1, len(x)):
#        z[i] = _z[(i * len(_z)) // len(x)]
#    
#    bottom = np.zeros_like(z)
#    width = depth = 1
#    
#    ax.bar3d(x, y, bottom, width, depth, z, shade=True)
#    plt.show()


    # setup the figure and axes
    fig = plt.figure(figsize=(8, 3))
    ax1 = fig.add_subplot(121, projection='3d')
    #ax2 = fig.add_subplot(122, projection='3d')
    
    # fake data
    _x = np.arange(4)
    _y = np.arange(5)
    _xx, _yy = np.meshgrid(_x, _y)
    x, y = _xx.ravel(), _yy.ravel()
    
    top = x + y
    bottom = np.zeros_like(top)
    width = depth = 1
    x_labels = ["a", "b", "c", "d"]
    ax1.bar3d(x, y, bottom, width, depth, top, shade=True)
    ax1.set_title('Shaded')
    ax1.set_xticks(range(len(x_labels)))
    ax1.set_xticklabels(x_labels)
    
    #ax2.bar3d(x, y, bottom, width, depth, top, shade=False)
    #ax2.set_title('Not Shaded')
    
    plt.show()