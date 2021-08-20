import matplotlib.pyplot as plt
import os
from tests.constants import RULESET_ROOT
from os import listdir
import json
from typing import List, Tuple
from tests.generate_rulesets import unformat_num


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
        f_split = f.split("_")
        alg = f_split[0]
        ruleset = "_".join(f_split[1:-1])
        nominal_rule_cnt = f_split[-1]

        rule_cnt = get_real_rule_cnt(ruleset, nominal_rule_cnt)

        with open(f_path) as fp:
            results = json.load(fp)
        data.append((alg, ruleset, nominal_rule_cnt, rule_cnt, results))
    return data


def load_likwid_data(data_dir, algs: List[str], ruleset_files: List[str], counter_groups: List[str]):
    result_files = set()
    for f in ruleset_files:
        f = os.path.basename(f)
        for a in algs:
            for cg in counter_groups:
                result_files.add(f"{a:s}_{f:s}.{cg:s}.csv")

    data = []
    for f in listdir(data_dir):
        f_path = os.path.join(data_dir, f)
        if f not in result_files:
            continue
        print(f)
        # if not isfile(f_path) or f_path.endswith(".png"):
        #    continue
        f_split0 = f.split(".")
        assert len(f_split0) == 3 and f_split0[2] == "csv", f_split0
        f, counter_group, _ = f_split0
        f_split1 = f.split("_")
        alg = f_split1[0]
        ruleset = "_".join(f_split1[1:-1])
        nominal_rule_cnt = f_split1[-1]

        rule_cnt = get_real_rule_cnt(ruleset, nominal_rule_cnt)

        with open(f_path) as fp:
            results = []
            for line in fp:
                line = line.strip().split(",")
                results.append(line)
        data.append((alg, ruleset, counter_group, nominal_rule_cnt, rule_cnt, results))

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

    def generate_graphs(self, key: str, title: str, filename: str, ylabel: str, xlabel: str):
        generate_graphs(self.RESULT_DIR, self.ALGS, self.RULESET_FILES,
                        key, title, filename, ylabel, xlabel)

    def generate_summary_graph(self, key, title, filename, ylabel,
                               xlabel, y_map=lambda y: y, y_log_scale=False,
                               figsize=(8, 6)):
        generate_summary_graph(self.RESULT_DIR, self.ALGS, self.RULESET_FILES,
                               key, title, filename, ylabel, xlabel, y_map,
                               y_log_scale, figsize)

    def generate_summary_grap_box_plot(self, key, title, filename, ylabel,
                                       xlabel,
                                       quantization: List[Tuple[int, int, str]],
                                       y_map=lambda y: y,
                                       y_log_scale=False, figsize=(8, 6)):
        result_dir = self.RESULT_DIR
        algs = self.ALGS
        ruleset_files = self.RULESET_FILES
        # {alg_name: {number_of_rule: sizes}}
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

        for quantum in quantization:
            # for each alg plot dependency on rule count
            fig1, ax1 = plt.subplots(figsize=figsize, dpi=80)
            if title is not None:
                ax1.set_title(title)
            if y_log_scale:
                ax1.set_yscale("log")

            algs = []
            _data = []
            for alg, sizes in data.items():
                size_series = list(sorted(((k, v) for k, v in sizes.items()
                                          if unformat_num(k[1]) >= quantum[0] and unformat_num(k[1]) < quantum[1]), \
                                          key=lambda x: x[0][2]))
                # x = [get_rulset_name(s) for s in size_series]
                y = [y_map(s[1][0]) for s in size_series]
                algs.append((alg))
                _data.append(y)
                # ax1.plot(x, y, 'o', label=alg, marker="X")
                # plt.xticks(x, x, rotation='vertical')

            ax1.boxplot(_data)
            x = list(range(1, len(algs) + 1))
            plt.xticks(x, algs, rotation='horizontal')
            # ax1.xticks(range(len(algs)), algs)

            # ax1.set_ylim(ymin=0)
            # plt.margins(0.2)
            # Tweak spacing to prevent clipping of tick-labels
            # plt.subplots_adjust(bottom=0.25)

            # ax1.tick_params(axis='x', which='major', pad=15)

            plt.xlabel(xlabel)
            plt.ylabel(ylabel)
            ax1.legend(loc='best', shadow=True, fancybox=True)
            fig1.savefig(os.path.join(result_dir, f"{filename:s}.{quantum[2]:s}.png"))
            plt.close(fig1)

    def generate_likwid_summary_grap_box_plot(self, sub_table, group, row_label, title, filename, ylabel,
                                       xlabel,
                                       quantization: List[Tuple[int, int, str]],
                                       y_map=lambda y: y,
                                       y_log_scale=False, figsize=(8, 6)):
        result_dir = self.RESULT_DIR
        algs = self.ALGS
        ruleset_files = self.RULESET_FILES
        # {alg_name: {number_of_rule: sizes}}
        data = {}
        for alg, ruleset, counter_group, nominal_rule_cnt, rule_cnt, results in load_likwid_data(
                result_dir, algs, ruleset_files, ["CYCLE_STALLS", "MEM_DP"]):
            print(alg)
            if group != counter_group:
                continue
            try:
                alg_d = data[alg]
            except KeyError:
                alg_d = {}
                data[alg] = alg_d

            if not results:
                print("[WARNING] no data for ", alg, ruleset, counter_group, nominal_rule_cnt)
                # no data available the test crashed
                continue

            _results = iter(results)
            val = None
            for row in _results:
                if row[0] == "TABLE" and row[1] == sub_table:
                    for row in _results:
                        if row[0] == row_label:
                            val = row[1]
                            break
                    break
            assert val is not None, (alg, ruleset, counter_group, nominal_rule_cnt)
            if val == "-":
                print("[WARNING] unknown value for ", alg, ruleset, counter_group, nominal_rule_cnt)
                continue
            val = float(val)
            print(val)
            k = (ruleset, nominal_rule_cnt, rule_cnt)
            try:
                vals = alg_d[k]
            except KeyError:
                vals = []
                alg_d[k] = vals

            vals.append(val)

        for quantum in quantization:
            # for each alg plot dependency on rule count
            fig1, ax1 = plt.subplots(figsize=figsize, dpi=80)
            if title is not None:
                ax1.set_title(title)
            if y_log_scale:
                ax1.set_yscale("log")

            algs = []
            _data = []
            for alg, sizes in data.items():
                size_series = list(sorted(((k, v) for k, v in sizes.items()
                                          if unformat_num(k[1]) >= quantum[0] and unformat_num(k[1]) < quantum[1]), \
                                          key=lambda x: x[0][2]))
                # x = [get_rulset_name(s) for s in size_series]
                y = [y_map(s[1][0]) for s in size_series]
                algs.append((alg))
                _data.append(y)
                # ax1.plot(x, y, 'o', label=alg, marker="X")
                # plt.xticks(x, x, rotation='vertical')

            ax1.boxplot(_data)
            x = list(range(1, len(algs) + 1))
            plt.xticks(x, algs, rotation='horizontal')
            # ax1.xticks(range(len(algs)), algs)

            # ax1.set_ylim(ymin=0)
            # plt.margins(0.2)
            # Tweak spacing to prevent clipping of tick-labels
            # plt.subplots_adjust(bottom=0.25)

            # ax1.tick_params(axis='x', which='major', pad=15)

            plt.xlabel(xlabel)
            plt.ylabel(ylabel)
            ax1.legend(loc='best', shadow=True, fancybox=True)
            fig1.savefig(os.path.join(result_dir, f"{filename:s}.{quantum[2]:s}.png"))
            plt.close(fig1)
