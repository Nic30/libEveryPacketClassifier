import os

ROOT = os.path.join(os.path.dirname(__file__), "..")


# BIN = os.path.join(ROOT, "build/meson.debug.linux.x86_64/src/packetClassificators")
def get_latest_folder(root):
    latest = None
    latest_time = None
    for d in os.listdir(root):
        p = os.path.join(root, d)
        if os.path.isdir(p):
            mtime = os.stat(p).st_mtime
            if latest is None or mtime > latest_time:
                latest = p
                latest_time = mtime
    assert latest is not None, ("directory", root, "was empty")
    return latest


BIN = os.path.join(get_latest_folder(os.path.join(ROOT, "build")), "src/packetClassificators")
RULESET_ROOT = os.path.join(ROOT, "../classbench-ng/generated/")

CLASSBENCH_ROOT = os.path.join(os.path.dirname(__file__), "..", "..", "classbench-ng")
CLASSBENCH = os.path.join(os.path.dirname(__file__), "..", "..", "classbench-ng", "classbench")
EXACT_MATCH_RULE_GEN = os.path.join(os.path.dirname(__file__), "..", "..", "pclass-vectorized/build/default/benchmarks/exact_match_rule_gen")
RULESET_ROOT = os.path.join(CLASSBENCH_ROOT, "generated/")
SEED_ROOT = os.path.join(CLASSBENCH_ROOT, "vendor", "parameter_files")

ALGS = [
    #"PartitionSort",
    # "PriorityTupleSpaceSearch",
    #"HyperSplit",
    # "HyperCuts",
    # "ByteCuts",
    # "BitVector",
    "TSS",
    # "TupleMergeOnline",
    #"pcv",
    #"TupleMergeOffline",
    #"CutSplit",
    #"EffiCuts",
]

SIZES = [
    # 100,
    # 500,
     #1e3,
     #2e3,
     #5e3,
     10e3,
     65e3,
     100e3,
     200e3,
     #300e3,
    # 1e3, 10e3, 100e3, 1e6, 10e6
    # 100e3, 200e3, 300e3, 400e3, 500e3,
    # 1e6,
]
SEEDS = [os.path.join(SEED_ROOT, s) for s in [
    "exact",
    "acl1_seed",
    #"acl2_seed",
    # "acl3_seed",
    # "acl4_seed",
    # "acl5_seed",
    "fw1_seed",
    #"fw2_seed",
    # "fw3_seed",
    # "fw4_seed",
    # "fw5_seed",
    #"ipc1_seed",
    #"ipc2_seed",
]]
