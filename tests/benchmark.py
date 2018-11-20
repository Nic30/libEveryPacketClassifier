import os
from multiprocessing import Pool
import json
from subprocess import check_call
from os.path import basename

ROOT = os.path.join(os.path.dirname(__file__), "..")
BIN = os.path.join(ROOT, "Debug/packetClassificators")

RULESET_ROOT = os.path.join(ROOT, "../classbench-ng/generated/")
ACL1s = [os.path.join(RULESET_ROOT, "acl1_%d" % i)
        for i in [100, #500, 1000, #2000,
                  # 5000, 10000, 65000
                  ]]

FW1s = [os.path.join(RULESET_ROOT, "fw1_%d" % i)
        for i in [100, #500, 1000, 2000,
                  # 5000, 10000, 65000
                  ]]

ALGS = [
    "PartitionSort",
    #"PriorityTupleSpaceSearch",
    "HyperSplit",
    #"HyperCuts",
    #"ByteCuts",
    #"BitVector",
    #"TupleSpaceSearch",
    #"TupleMergeOnline",
    #"TupleMergeOffline",
]

if __name__ == "__main__":
    results = os.path.join(ROOT, "results")
    os.makedirs(results, exist_ok=True)

    def run_benchmark(args):
        alg, ruleset = args
        ruleset_name = basename(ruleset)
        result_file = f"{alg}_{ruleset_name}"
        check_call([BIN, f"c={alg}", f"f={ruleset}", f"o={result_file}"])
        res = json.load(result_file)
        res = res[0]
        res["ruleset"] = ruleset
        return res

    RULESETS = [*ACL1s,
                 #*FW1s
                 ]
    benchmarks = [
        (alg, ruleset)
            for alg in ALGS
                for ruleset in RULESETS
    ]

    with Pool() as pool:
        resutls = pool.map(run_benchmark, benchmarks)
    
    "Classifier"
    fig1, ax1 = plt.subplots()
    ax1.set_title('Memory consuptions')
    ax1.boxplot(data)        
        