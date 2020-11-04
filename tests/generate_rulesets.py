import os
from multiprocessing import Pool, Value
from subprocess import check_call
from pathlib import Path

CLASSBENCH_ROOT = os.path.join(os.path.dirname(__file__), "..", "..", "classbench-ng")
CLASSBENCH = os.path.join(os.path.dirname(__file__), "..", "..", "classbench-ng", "classbench")
RULESET_ROOT = os.path.join(CLASSBENCH_ROOT, "generated/")
SEED_ROOT = os.path.join(CLASSBENCH_ROOT, "vendor", "parameter_files")
OUT = os.path.join(CLASSBENCH_ROOT, "generated")


def format_num(n):
    for i, unit in [(1e3, "K"), (1e6, "M"), (1e9, "G")]:
        if n >= int(i):
            return "%d%s" % (n // i, unit)
    return "%d" % (n)


SIZES = [
    #100,
    #500,
    #1e3,
    2e3,
    5e3,
    10e3,
    #65e3,
    #100e3,
]
SEEDS = [os.path.join(SEED_ROOT, s) for s in [
    "acl1_seed",
    "acl2_seed",
    "acl3_seed",
    #"acl4_seed",
    #"acl5_seed",
    #"fw1_seed",
    #"fw2_seed",
    #"fw3_seed",
    #"fw4_seed",
    #"fw5_seed",
    #"ipc1_seed",
    #"ipc2_seed",
]]

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
            check_call([CLASSBENCH, "generate", "v4", "--count=%d" % (size), seed], stdout=f, cwd=CLASSBENCH_ROOT)
            with counter.get_lock():
                counter.value += 1
            print("%.2f%% %r" % (counter.value / len(tasks) * 100, args))
    else:
        with counter.get_lock():
            counter.value += 1


if __name__ == "__main__":
    counter = Value('i', 0)
    with Pool(14, initializer=init, initargs=(counter,)) as pool:
        i = pool.map_async(run_classbench, tasks, chunksize=1)
        i.wait()
        print(i.get())
