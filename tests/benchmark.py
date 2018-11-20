import os
from os import listdir
from multiprocessing import Pool
import json
from subprocess import check_call
from os.path import basename, isfile
import matplotlib.pyplot as plt

ROOT = os.path.join(os.path.dirname(__file__), "..")
BIN = os.path.join(ROOT, "Debug/packetClassificators")

RULESET_ROOT = os.path.join(ROOT, "../classbench-ng/generated/")
ACL1s = [os.path.join(RULESET_ROOT, "acl1_%d" % i)
        for i in [100, 500, 1000, 2000,
                  5000, 10000, #65000
                  ]]

FW1s = [os.path.join(RULESET_ROOT, "fw1_%d" % i)
        for i in [100, 500, 1000, 2000,
                  5000, 10000, #65000
                  ]]

ALGS = [
    "PartitionSort",
    "PriorityTupleSpaceSearch",
    "HyperSplit",
    "HyperCuts",
    "ByteCuts",
    "BitVector",
    "TupleSpaceSearch",
    "TupleMergeOnline",
    # "TupleMergeOffline",
]


def run_benchmark(args):
    alg, ruleset, result_dir = args
    ruleset_name = basename(ruleset)
    result_file = os.path.join(result_dir, f"{alg}_{ruleset_name}")
    check_call([BIN, f"c={alg}", f"f={ruleset}", f"o={result_file}"])
    return result_file
    # res = json.load(result_file)
    # res = res[0]
    # return res


def run_classifications(rulesets, algs, result_dir):
    os.makedirs(result_dir, exist_ok=True)

    benchmarks = [
        (alg, ruleset, result_dir)
            for alg in algs
                for ruleset in rulesets
    ]

    with Pool() as pool:
        resutls = pool.map(run_benchmark, benchmarks)


def generate_graphs(result_dir):
    # {salg_name: {number_of_rule: sizes}}
    data = {}
    for f in listdir(result_dir):
        f_path = os.path.join(result_dir, f) 
        if not isfile(f_path) or f_path.endswith(".png"):
            continue
        alg, ruleset, rule_cnt = f.split("_")
        rule_cnt = int(rule_cnt)

        try:
            alg_d = data[alg]
        except KeyError:
            alg_d = {}
            data[alg] = alg_d

        with open(f_path) as fp:
            results = json.load(fp)
        
        mem_size = results["Size(bytes)"]
        mem_size = int(mem_size)
        try:
            sizes = alg_d[rule_cnt]
        except KeyError:
            sizes = []
            alg_d[rule_cnt] = sizes

        sizes.append(mem_size)
    

    # for each alg plot dependency on rule count
    for alg, sizes in data.items():
        fig1, ax1 = plt.subplots()
        size_series = list(sorted(sizes.items(), key=lambda x: x[0]))
        ax1.set_title(f'Memory consuptions for {alg}')
        ax1.boxplot([x[1] for x in size_series])
        plt.xticks([i for i in range(1, len(size_series) + 1)],
                   [x[0] for x in size_series])       
        fig1.savefig(os.path.join(result_dir, f'{alg}.png'))


if __name__ == "__main__":
    RULESETS = [*ACL1s,
                *FW1s]
    result_dir = os.path.join(ROOT, "results")
    
    #run_classifications(RULESETS, ALGS, result_dir)
    generate_graphs(result_dir)
