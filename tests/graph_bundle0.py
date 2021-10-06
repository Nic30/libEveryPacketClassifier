from tests.benchmark_graph_gen import GraphGen
from tests.benchmark import RESULT_DIR, make_tasks
from tests.constants import ALGS


def main():
    RULESET_FILES = make_tasks()
    gg_all = GraphGen(RESULT_DIR, sorted(set(f for _, f, _, _ in RULESET_FILES)), ALGS)
    # gg_no_long_constr = GraphGen(result_dir, RULESET_FILES,
    #                              [a for a in ALGS if a not in ["HyperSplit", "HyperCuts"]])
    # gg_long_constr = GraphGen(result_dir, RULESET_FILES,
    #                           ["HyperCuts", "HyperSplit",])

    # gg_all.generate_graphs(
    #    "Size(bytes)",
    #    'Memory consuptions for {alg}',
    #    'fig/{alg}_mem.png',
    #    "Size [B]",
    #    "Ruleset")
    # gg_all.generate_graphs(
    #   "ConstructionTime(ms)",
    #   'Construction time for {alg}',
    #   'fig/{alg}_constr_time.png',
    #   "Construction time [ms]",
    #   "Ruleset")
    # gg_all.generate_graphs(
    #    "ClassificationTime(s)",
    #    'Classification time for {alg} (1M packets)',
    #    'fig/{alg}_cls_time.png',
    #    'Classification time [s]',
    #    "Ruleset")
    thread_cnts = [1, 2, 4]
    figsize = (16, 8)
    quantization = [
        #(0, 1e3 + 1, "<1e3"),
        (1e3 + 1, 5e4 + 1, "1e3 to 5e4"),
        (5e4 + 1, 1e6 + 1, "5e4 to 1e6")
    ]

    gg_all.generate_summary_grap_box_plot(
      "ClassificationTime(s)",
      None,
      # 'Classification time (1M packets)',
      f'fig/summary_cls_time_boxplot',
      'Classification time [s]',
      "Algorithm",
      quantization,
      thread_cnts=thread_cnts,
      y_log_scale=False,
      figsize=figsize,)
    # gg_all.generate_likwid_summary_grap_box_plot("Region classifier_construction", "CYCLE_STALLS",
    #                                             "Stalls caused by memory loads rate [%]",
    #                                             "Stalls caused by memory loads rate",
    #                                             f"fig/likwid_mem_stall_constr",
    #                                             "[%]", "Alg",
    #                                             quantization,
    #                                             thread_cnts,
    #                                             figsize=figsize)
    #
    gg_all.generate_likwid_summary_grap_box_plot("Region classification", "CYCLE_STALLS",
                                            "Stalls caused by memory loads rate [%]",
                                            "Stalls caused by memory loads rate",
                                            f"fig/likwid_mem_stall_cls",
                                            "[%]", "Alg",
                                            quantization,
                                            thread_cnts,
                                            figsize=figsize)

    # gg_all.generate_likwid_summary_grap_box_plot("Region classifier_construction", "MEM_DP",
    #                                             "Memory data volume [GBytes]",
    #                                             "Memory data volume [GBytes]",
    #                                             f"fig/likwid_mem_constr",
    #                                             "[GBytes]", "Alg",
    #                                             quantization,
    #                                             thread_cnts,
    #                                             figsize=figsize)
    #
    gg_all.generate_likwid_summary_grap_box_plot("Region classification", "MEM_DP",
                                            "Memory data volume [GBytes]",
                                            "Memory data volume [GBytes]",
                                            f"fig/likwid_mem_cls",
                                            "[GBytes]", "Alg",
                                            quantization,
                                            thread_cnts,
                                            figsize=figsize)


    gg_all.generate_summary_grap_box_plot(
       "ConstructionTime(ms)",
       None,
       # 'Construction time',
       'fig/summary_constr_time',
       "Construction time [ms]",
       "Algorithm",
       quantization,
       thread_cnts=thread_cnts,
       y_log_scale=True,
       figsize=figsize)

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
