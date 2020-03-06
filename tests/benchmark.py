import json
from multiprocessing import Pool, cpu_count
from os import listdir
import os
from os.path import basename
from subprocess import check_call

import matplotlib.pyplot as plt


ROOT = os.path.join(os.path.dirname(__file__), "..")
BIN = os.path.join(ROOT, "build/default/src/packetClassificators")

RULESET_ROOT = os.path.join(ROOT, "../classbench-ng/generated/")


def run_benchmark(args):
    alg, ruleset, result_dir = args
    ruleset_name = basename(ruleset)
    result_file = os.path.join(result_dir, f"{alg}_{ruleset_name}")
    if os.path.isfile(result_file) and os.path.getsize(result_file) > 0:
        return result_file
    cmd = [BIN, f"c={alg}", f"f={ruleset}", f"o={result_file}"]
    print("+>" + " ".join(cmd))
    check_call(cmd)
    print("->" + " ".join(cmd))
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

    with Pool(cpu_count() // 2) as pool:
        resutls = pool.map(run_benchmark, benchmarks)


# @lru_cache(maxsize=512)
def get_real_rule_cnt(ruleset, rule_cnt):
    with open(os.path.join(RULESET_ROOT, f'{ruleset}_{rule_cnt}')) as f:
        return len([line for line in f])


def load_data(data_dir, algs, ruleset_files):
    result_files = set()
    for f in ruleset_files:
        f = os.path.basename(f)
        for a in algs:
            result_files.add(f"{a}_{f}")

    data = []
    for f in listdir(data_dir):
        f_path = os.path.join(data_dir, f)
        if f not in result_files:
            continue
        # if not isfile(f_path) or f_path.endswith(".png"):
        #    continue
        alg, ruleset, nominal_rule_cnt = f.split("_")
        rule_cnt = get_real_rule_cnt(ruleset, nominal_rule_cnt)

        with open(f_path) as fp:
            results = json.load(fp)
        data.append((alg, ruleset, nominal_rule_cnt, rule_cnt, results))
    return data


def get_rulset_name(ruleset_info):
    s = ruleset_info
    size = s[0][1]
    if size.endswith("000"):
        size = size[:-3] + "K"
    return "%s_%s" % (s[0][0], size)


def generate_graphs(result_dir, algs, ruleset_files, key,
                    title, filename, ylabel, xlabel):
    # {salg_name: {number_of_rule: sizes}}
    data = {}
    for alg, ruleset, nominal_rule_cnt, rule_cnt, results in load_data(
            result_dir, algs, ruleset_files):
        try:
            alg_d = data[alg]
        except KeyError:
            alg_d = {}
            data[alg] = alg_d

        val = results[key]
        val = float(val)
        k = (ruleset, nominal_rule_cnt, rule_cnt)
        try:
            vals = alg_d[k]
        except KeyError:
            vals = []
            alg_d[k] = vals

        vals.append(val)

    # for each alg plot dependency on rule count
    for alg, sizes in data.items():
        fig1, ax1 = plt.subplots()
        size_series = list(sorted(sizes.items(), key=lambda x: x[0][2]))
        ax1.set_title(title.format(alg=alg))
        x = [get_rulset_name(s) for s in size_series]
        y = [s[1] for s in size_series]
        ax1.plot(x, y, 'o', label=alg, marker="X")
        # ax1.boxplot([x[1] for x in size_series])
        # plt.xticks([i for i in range(1, len(size_series) + 1)],
        #           [x[0] for x in size_series])
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        fig1.savefig(os.path.join(result_dir, filename.format(alg=alg)))
        plt.close(fig1)


def generate_summary_graph(result_dir, algs, ruleset_files, key, title,
                           filename, ylabel, xlabel, y_map, y_log_scale, figsize):
    # {salg_name: {number_of_rule: sizes}}
    data = {}
    for alg, ruleset, nominal_rule_cnt, rule_cnt, results in load_data(
            result_dir, algs, ruleset_files):
        try:
            alg_d = data[alg]
        except KeyError:
            alg_d = {}
            data[alg] = alg_d

        val = results[key]
        val = float(val)
        k = (ruleset, nominal_rule_cnt, rule_cnt)
        try:
            vals = alg_d[k]
        except KeyError:
            vals = []
            alg_d[k] = vals

        vals.append(val)
    # for each alg plot dependency on rule count
    fig1, ax1 = plt.subplots(figsize=figsize, dpi=80)
    if title is not None:
        ax1.set_title(title)
    if y_log_scale:
        ax1.set_yscale("log")

    for alg, sizes in data.items():
        size_series = list(sorted(sizes.items(), key=lambda x: x[0][2]))
        x = [get_rulset_name(s) for s in size_series]
        y = [y_map(s[1][0]) for s in size_series]
        ax1.plot(x, y, 'o', label=alg, marker="X")
        plt.xticks(x, x, rotation='vertical')

    ax1.set_ylim(ymin=0)
    plt.margins(0.2)
    # Tweak spacing to prevent clipping of tick-labels
    plt.subplots_adjust(bottom=0.25)

    # ax1.tick_params(axis='x', which='major', pad=15)

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    ax1.legend(loc='best', shadow=True, fancybox=True)
    fig1.savefig(os.path.join(result_dir, filename))
    plt.close(fig1)


class GraphGen():
    def __init__(self, RESULT_DIR, RULESET_FILES, ALGS):
        self.RESULT_DIR = RESULT_DIR
        self.RULESET_FILES = RULESET_FILES
        self.ALGS = ALGS

    def generate_graphs(self, key, title, filename, ylabel, xlabel):
        generate_graphs(self.RESULT_DIR, self.ALGS, self.RULESET_FILES,
                        key, title, filename, ylabel, xlabel)

    def generate_summary_graph(self, key, title, filename, ylabel,
                               xlabel, y_map=lambda y: y, y_log_scale=False,
                               figsize=(8, 6)):
        generate_summary_graph(self.RESULT_DIR, self.ALGS, self.RULESET_FILES,
                               key, title, filename, ylabel, xlabel, y_map,
                               y_log_scale, figsize)


def main():

    ALGS = [
        # "PartitionSort",
        "PriorityTupleSpaceSearch",
        "HyperSplit",
        "HyperCuts",
        # "ByteCuts",
        # "BitVector",
        # "TupleSpaceSearch",
        # "TupleMergeOnline",
        "pcv",
        # "TupleMergeOffline",
    ]

    RULESET_FILES = []
    for ruleset_i in [1, 2,
                      # 3,
                      # 4, 5
                      ]:
        for size in [100,
                     # 500,
                     1000,
                     # 2000,
                     5000,
                     # 10000,
                     # 65000
                     ]:
            f = os.path.join(RULESET_ROOT, f"acl{ruleset_i}_{size}")
            RULESET_FILES.append(f)

    for ruleset_i in [
            # 1,
            2,
            # 3,
            # 4, 5
            ]:
        for size in [100,
                     500,
                     1000,
                     # 2000,
                     5000,
                     # 10000,
                     # 65000
                     ]:
            f = os.path.join(RULESET_ROOT, f"fw{ruleset_i}_{size}")
            RULESET_FILES.append(f)
    for size in [
            # 100,
            # 500,
            1000,
            # 5000,
            10000,
            ]:
        f = os.path.join(RULESET_ROOT, f"exact_{size}")
        RULESET_FILES.append(f)

    result_dir = os.path.join(ROOT, "results")

    run_classifications(RULESET_FILES, ALGS, result_dir)
    gg_all = GraphGen(result_dir, RULESET_FILES, ALGS)
    # gg_no_long_constr = GraphGen(result_dir, RULESET_FILES,
    #                              [a for a in ALGS if a not in ["HyperSplit", "HyperCuts"]])
    # gg_long_constr = GraphGen(result_dir, RULESET_FILES,
    #                           ["HyperCuts", "HyperSplit",])

    gg_all.generate_graphs(
        "Size(bytes)",
        'Memory consuptions for {alg}',
        'fig/{alg}_mem.png',
        "Size [B]",
        "Ruleset")
    gg_all.generate_graphs(
        "ConstructionTime(ms)",
        'Construction time for {alg}',
        'fig/{alg}_constr_time.png',
        "Construction time [ms]",
        "Ruleset")
    gg_all.generate_graphs(
        "ClassificationTime(s)",
        'Classification time for {alg} (1M packets)',
        'fig/{alg}_cls_time.png',
        'Classification time [s]',
        "Ruleset")
    gg_all.generate_summary_graph(
        "ClassificationTime(s)",
        None,
        # 'Classification time (1M packets)',
        'fig/summary_cls_time.png',
        'Classification time [s]',
        "Ruleset",
        y_log_scale=True,
        figsize=(8, 4))
    gg_all.generate_summary_graph(
        "ConstructionTime(ms)",
        None,
        # 'Construction time',
        'fig/summary_constr_time.png',
        "Construction time [ms]",
        "Ruleset",
        y_log_scale=True,
        figsize=(8, 4))

    # gg_no_long_constr.generate_summary_graph(
    #     "ConstructionTime(ms)",
    #     'Construction time',
    #     'fig/summary_constr_time_fast.png',
    #     "Construction time [ms]",
    #     "Ruleset",
    #     y_log_scale=True)
    # gg_long_constr.generate_summary_graph(
    #     "ConstructionTime(ms)",
    #     'Construction time',
    #     'fig/summary_constr_time_slow.png',
    #     "Construction time [s]",
    #     "Ruleset",
    #     y_map=lambda y: y/1000,
    #     y_log_scale=True)


if __name__ == "__main__":
    main()
