# libEveryPacketClassifier

This library was originaly made as an extension of code provideded for:

ICNP 2016: A Sorted Partitioning Approach to High-speed and Fast-update OpenFlow Classification

Now it contains a collection of packet classification algorithms from various authors
embeded in likwid based benchmarking environmet. This environment is designed to pinout
exact part of CPU/machine which is bottleneck for specified algorithm. It also provides
necessary tests to verify the functionality of algorithms.

Build
```
mkdir build/default; cd build/default
meson ../..
# optionally meson configure -Ddebug=false
ninja
```

Usage
```
sudo python3 -m tests.benchmark
# or
cd build ninja tests
```


### Useful links
https://github.com/LucaCanali/Miscellaneous/blob/master/Spark_Notes/Tools_Linux_Memory_Perf_Measure.md
https://github.com/RRZE-HPC/likwid/wiki/likwid-perfctr#prerequisites
https://stackoverflow.com/questions/18661976/reading-dev-cpu-msr-from-userspace-operation-not-permitted
