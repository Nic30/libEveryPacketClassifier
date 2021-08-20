#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from datetime import datetime
from multiprocessing import Pool, Value
import os
from pathlib import Path
from subprocess import check_call
import sys

from tests.constants import SEEDS, SIZES, CLASSBENCH, \
    CLASSBENCH_ROOT, EXACT_MATCH_RULE_GEN

OUT = os.path.join(CLASSBENCH_ROOT, "generated")
OUT_TIME_LOG = os.path.join(CLASSBENCH_ROOT, "generated", "time.log")


def format_num(n: int):
    for i, unit in reversed([(1e3, "K"), (1e6, "M"), (1e9, "G")]):
        if n >= int(i):
            return "%d%s" % (n // i, unit)
    return "%d" % (n)


def unformat_num(n:str):
    # [TODO]
    return eval(n.replace("K", "*1e3").replace("M", "*1e6").replace("G", "*1e9"))


counter = None
tasks = [(seed, int(size)) for seed in SEEDS for size in SIZES]


def init(args):
    ''' store the counter for later use '''
    global counter
    counter = args


def run_classbench(args):
    global counter
    (seed, size) = args
    # += operation is not atomic, so we need to get a lock:

    res_f = os.path.join(OUT, os.path.basename(seed) + "_" + format_num(size))
    res_f_path = Path(res_f)
    if not res_f_path.is_file() or res_f_path.stat().st_size == 0:
        with open(res_f, "w") as f:
            print(f"Calling classbench generator for {seed} {size}")
            t0 = datetime.now()
            try:
                check_call([CLASSBENCH, "generate", "v4", "--count=%d" % (size), seed], stdout=f, cwd=CLASSBENCH_ROOT)
            except Exception as e:
                print(f"[ERROR] exact {seed} {size}", e, file=sys.stderr)

            t1 = datetime.now()
        with counter.get_lock():
            counter.value += 1
        print("%.2f%% %r" % (counter.value / len(tasks) * 100, args))
        with open(OUT_TIME_LOG, 'a+') as f:
            f.write(f"classbench {seed} {size} {str(t1-t0):s}")

    else:
        print(f"classbench {seed} {size} already exits")
        with counter.get_lock():
            counter.value += 1


def run_exact_gen(args):
    global counter
    (seed, size) = args
    # += operation is not atomic, so we need to get a lock:

    res_f = os.path.join(OUT, "exact_" + format_num(size))
    res_f_path = Path(res_f)
    if not res_f_path.is_file() or res_f_path.stat().st_size == 0:
        with open(res_f, "w") as f:
            print(f"Calling exactmatch generator for {seed} {size}")
            t0 = datetime.now()
            try:
                check_call([EXACT_MATCH_RULE_GEN, str(seed), str(size)], stdout=f, cwd=CLASSBENCH_ROOT)
            except Exception as e:
                print(f"[ERROR] exact {seed} {size}", e, file=sys.stderr)

            t1 = datetime.now()

        with counter.get_lock():
            counter.value += 1

        print("%.2f%% %r" % (counter.value / len(SIZES) * 100, args))
        with open(OUT_TIME_LOG, 'a+') as f:
            f.write(f"exact {seed} {size} {str(t1-t0):s}")

    else:
        print(f"exactmatch {seed} {size} already exits")
        with counter.get_lock():
            counter.value += 1


if __name__ == "__main__":
    counter = Value('i', 0)
    with Pool(14, initializer=init, initargs=(counter,)) as pool:
        i = pool.map_async(run_classbench, tasks)
        i.wait()
        print(i.get())

    # _tasks = [(0, size) for size in SIZES]
    # with Pool( initializer=init, initargs=(counter,)) as pool:
    #    i = pool.map_async(run_exact_gen, _tasks, chunksize=1)
    #    i.wait()
    #    print(i.get())
