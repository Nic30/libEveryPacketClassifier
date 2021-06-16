import matplotlib.pyplot as plt
import os

tuple_counts = [1, 10, 50, 100]


def mk_data_pps(data):
    return list(zip(tuple_counts, data))


data_pps_X_tuples = {
    "tss": mk_data_pps([10, 10 / 5, 10 / 25, 10 / 50]),
    "pcv": mk_data_pps([7.5, 7.5, 7, 5]),
    "cutsplit": mk_data_pps([5, 4.6, 4.5, 4]),
    "hypersplit": mk_data_pps([8, 8, 7.5, 7]),
}

rule_count = [100, 1e3, 10e3, 100e3]
def mk_data_rules(data):
    return list(zip(rule_count, data))

data_pps_X_rules = {
    "tss": mk_data_rules([10/50, 10/50, 8/50, 5/50]),
    "pcv": mk_data_rules([5, 4.5, 4, 4]),
    "cutsplit": mk_data_rules([4, 4, 3.5, 3]),
    "hypersplit": mk_data_rules([7, 7, 6.5, 5]),
}

data_update_X_rules = {
    "tss": mk_data_rules([65e3, 65e3, 65e3, 65e3]),
    "pcv": mk_data_rules([1e3, 1e3, 0.5e3, 0.5e3]),
    "cutsplit": mk_data_rules([1e3, 1e3, 0.5e3, 0.5e3]),
    "hypersplit": mk_data_rules([1e2, 1e1, 0.1, 0.01]),
}


def plot_2d(data, result_dir, label, x_label, y_label, x_scale="linear", y_scale="linear"):
    fig, ax1 = plt.subplots(figsize=(8, 4.5))
    ax1.set_xlabel(x_label)
    ax1.set_ylabel(y_label)

    #ax1.set_xticks(range(len(y_rulesets)))
    #ax1.set_xticklabels([os.path.basename(r) for r in y_rulesets])
    for alg_name, data in sorted(data.items(), key=lambda x: x[0]):
        plt.plot([d[0] for d in data], [d[1] for d in data], label=alg_name)
    ax1.set_yscale(y_scale)
    ax1.set_xscale(x_scale)
    #plt.xticks(rotation=45)
    plt.legend()
    fig.savefig(os.path.join(result_dir, label.replace(" ", "_") + ".png"))
    #plt.show()

if __name__ == "__main__":
    result_dir = os.path.join(os.path.dirname(__file__), "..", "results")
    plot_2d(data_pps_X_tuples, result_dir, "pps X tuples, 10K rules", "Tuple#", "Throughput [MPps]")
    plot_2d(data_pps_X_rules, result_dir, "pps X rules, 100 tuples", "Rule#", "Throughput [MPps]")
    plot_2d(data_update_X_rules, result_dir, "update X rules, 100 tuples", "Rule#", "Update/s", x_scale="log", y_scale="log")

