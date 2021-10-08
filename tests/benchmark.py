#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from colorama import Fore, Style
from multiprocessing import Pool, cpu_count
import os
from os.path import basename
from subprocess import check_call, check_output
import sys

from tests.constants import BIN, ROOT, ALGS, CORE_SELECT
from tests.generate_rulesets import format_num, SEEDS, SIZES, OUT

RESULT_DIR = os.path.join(ROOT, "results")
TRIALS = 1
WALL_TIME = 5 * 60


def run_benchmark(args):
    alg, ruleset, result_dir, cores = args
    ruleset_name = basename(ruleset)
    result_file = os.path.join(result_dir, f"{alg}_{ruleset_name}_t{len(cores)}")
    if os.path.isfile(result_file) and os.path.getsize(result_file) > 0:
        print(f"+> {result_file} already generated")
        return result_file

    cmd = [BIN, f"c={alg}", f"f={ruleset}", f"o={result_file}", f'r={TRIALS:d}', f't={len(cores)}']
    print("+>" + " ".join(cmd))
    try:
        check_call(cmd, timeout=WALL_TIME)
        print("->" + " ".join(cmd))
    except Exception as e:
        print("->" + Fore.RED + "[ERROR]" + Style.RESET_ALL + " ".join(cmd), e, file=sys.stderr)
    return result_file
    # res = json.load(result_file)
    # res = res[0]
    # return res


def run_likwid_benchmark(args):
    alg, ruleset, result_dir, cores = args
    modules = check_output(["lsmod"]).decode()
    if "msr                    " not in modules:
        check_call(["modprobe", "msr"])

    ruleset_name = basename(ruleset)
    result_file = os.path.join(result_dir, f"{alg}_{ruleset_name}_t{len(cores)}")
    counter_groups = [
        # "CYCLE_ACTIVITY",
        "CYCLE_STALLS",
        # "L3",
        "MEM_DP",
        # "DATA",
    ]
    for counter_group in counter_groups:
        # https://github.com/RRZE-HPC/likwid/blob/master/doc/applications/likwid-perfctr.md
        likwid_report_file = f'{result_file:s}.{counter_group:s}.csv'
        if os.path.isfile(result_file) and os.path.getsize(result_file) > 0 and\
                os.path.isfile(likwid_report_file) and os.path.getsize(likwid_report_file) > 0:
            print(f"+> {result_file} {likwid_report_file:s} already generated")
            continue

        cmd = ['likwid-perfctr', '-C', ','.join([str(c) for c in cores]), '-g', counter_group, '-O', '-o', likwid_report_file,
               '-m', BIN, f"c={alg}", f"f={ruleset}", f"o={result_file}", f'r={TRIALS:d}', f't={len(cores)}'
                ]
        print("+>" + " ".join(cmd))
        try:
            check_call(cmd, timeout=WALL_TIME)
            print("->" + " ".join(cmd))
        except Exception as e:
            print("->" + Fore.RED + "[ERROR]" + Style.RESET_ALL + " ".join(cmd), e, file=sys.stderr)

    return result_file


def run_classifications(benchmarks, result_dir, run_benchmark_fn, thread_pool_size:int):
    os.makedirs(result_dir, exist_ok=True)

    if thread_pool_size == 1:
        for b in benchmarks:
            run_benchmark_fn(b)
    else:
        with Pool(cpu_count() // 2) as pool:
            pool.map(run_benchmark_fn, benchmarks)


def make_tasks():
    RULESET_FILES = []
    for seed in SEEDS:
        for size in SIZES:
            f = os.path.join(OUT, os.path.basename(seed) + "_" + format_num(size))
            RULESET_FILES.append(f)

    benchmarks = [
        (alg, ruleset, RESULT_DIR, cores)
        for alg in ALGS
        for ruleset in RULESET_FILES
        for cores in CORE_SELECT
    ]
    return benchmarks


def main():
    benchmarks = make_tasks()
    # run_classifications(benchmarks, RESULT_DIR, run_benchmark, 1)
    run_classifications(benchmarks, RESULT_DIR, run_likwid_benchmark, 1)


if __name__ == "__main__":
    main()
