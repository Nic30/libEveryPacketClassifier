#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ClassBenchTraceGenerator/trace_tools.h"
#include "ElementaryClasses.h"

#include "IO/InputReader.h"
#include "IO/OutputWriter.h"

#include "Simulation.h"

#include "Utilities/MapExtensions.h"
#include "construct_classifier_by_name.h"

using namespace std;

vector<int> RunSimulatorClassificationTrial(PacketClassficationSimulator &s,
		const string &name, vector<map<string, string>> &data, size_t trials) {
	map<string, string> d = { { "Classifier", name } };
	std::cout << "[INFO]" << name << std::endl;
	auto r = s.ruhn_only_packet_classification(d, trials);
	data.push_back(d);
	return r;
}

pair<vector<string>, vector<map<string, string>>> RunSimulatorOnlyClassification(
		const unordered_map<string, string> &args,
		const vector<Packet> &packets, const vector<Rule> &rules,
		ClassifierSet classifiers, const string &outfile, size_t trials,
		size_t thread_cnt) {
	std::cerr << "[INFO] Classification Simulation" << std::endl;

	vector<string> header = { "Classifier", "ConstructionTime(ms)",
			"ClassificationTime(s)", "Size(bytes)", "MemoryAccess", "Tables",
			"TableSizes", "TableQueries", "AvgQueries" };
	vector<map<string, string>> data;

	for (auto &pair : classifiers) {
		PacketClassficationSimulator s(pair.second, rules, packets);
		RunSimulatorClassificationTrial(s, pair.first.c_str(), data, trials);
	}

	if (outfile != "") {
		OutputWriter::WriteJsonFile(outfile, header, data);
	}
	return make_pair(header, data);
}

void RunSimulatorUpdateTrial(PacketClassficationSimulator &s,
		const string &name, const vector<Request> &req,
		vector<map<string, string>> &data, int reps) {

	map<string, string> d = { { "Classifier", name } };
	map<string, double> trial;

	s.run_task_sequnce(req, trial, reps);
	for (auto pair : trial) {
		d[pair.first] = to_string(pair.second / reps);
	}
	data.push_back(d);
}

pair<vector<string>, vector<map<string, string>>> RunSimulatorUpdates(
		const unordered_map<string, string> &args,
		const vector<Packet> &packets, const vector<Rule> &rules,
		ClassifierSet classifiers, const string &outfile, int repetitions) {
	std::cerr << "[INFO] Update Simulation" << std::endl;

	vector<string> header = { "Classifier", "UpdateTime(s)" };
	vector<map<string, string>> data;

	for (const auto &pair : classifiers) {
		PacketClassficationSimulator s(pair.second, rules, packets);
		const auto req = s.SetupComputation(0, 500000, 500000);
		RunSimulatorUpdateTrial(s, pair.first.c_str(), req, data, repetitions);
	}
	if (outfile != "") {
		OutputWriter::WriteCsvFile(outfile, header, data);
	}
	return make_pair(header, data);
}

bool validation_run(const ClassifierSet classifiers, const vector<Rule> &rules,
		const vector<Packet> &packets, int error_threshold) {

	int numWrong = 0;

	vector<Rule> rules_sorted = rules;
	// highest priority first
	sort(rules_sorted.begin(), rules_sorted.end(),
			[](const Rule &r0, const Rule &r1) {
				return r0.priority >= r1.priority;
			});
	bool all_matched_rule0_or_not_found = true;
	for (const Packet &p : packets) {
		int result = -1;
		bool first = true;
		bool error_seen = false;
		// [todo] use index to store store value, instead of name
		for (const auto &pair : classifiers) {
			int result_tmp = pair.second[0]->ClassifyAPacket(p);
			if (result_tmp != 0 && result_tmp != -1) {
				all_matched_rule0_or_not_found = false;
			}
			if (first) {
				result = result_tmp;
			} else if (result != result_tmp) {
				error_seen = true;
			}
			first = false;
		}
		if (error_seen) {
			numWrong++;
			std::cout << Packet_to_string(p) << std::endl;
			for (const auto &pair : classifiers) {
				int res = pair.second[0]->ClassifyAPacket(p);
				std::cout << "\t" << pair.first << ": " << res;
				if (res != -1) {
					if ((size_t) res >= rules.size()) {
						std::cout << " <out of range>";
					} else {
						std::cout << " " << rules.at(res);
					}
				}
				std::cout << std::endl;
			}
			int i = 0;
			for (const Rule &r : rules_sorted) {
				if (r.MatchesPacket(p)) {
					std::cout << "\tmatch " << "(index: " << i << ", id: "
							<< r.id << ", prio:" << r.priority << ")" << " r:"
							<< r << std::endl;
					//break;
				}
				i++;
			}
		}
		if (numWrong >= error_threshold) {
			break;
		}
	}
	if (all_matched_rule0_or_not_found)
		throw std::runtime_error(
				"all packets matched rule0 or were not found: this is a sign of missconfiguration");
	return numWrong == 0;
}

void validation_prepare_and_run(const unordered_map<string, string> &args,
		const vector<Packet> &packets, const vector<Rule> &rules,
		ClassifierSet classifiers) {
	std::cerr << "[INFO] Validation Simulation, Building..." << std::endl;
	int error_threshold = GetIntOrElse(args, "Validate.Threshold", 10);
	if (classifiers.size() < 2) {
		throw std::runtime_error(
				"requires at least 2 classifiers for verification");
	}
	for (auto &pair : classifiers) {
		std::cerr << "[INFO] building " << pair.first << std::endl;
		//pair.second[0]->_ConstructClassifier(rules);
		pair.second[0]->ConstructClassifier(rules);
	}
	if (validation_run(classifiers, rules, packets, error_threshold)) {
		std::cerr << "[INFO] All classifiers are in accord" << std::endl;
	} else {
		std::cerr << "[ERROR] There were difference in classifier outputs"
				<< std::endl;
	}
}

int main(int argc, char *argv[]) {
	unordered_map<string, string> args = ParseArgs(argc, argv);

	string filterFile = GetOrElse(args, "f", "");
	string packetFile = GetOrElse(args, "p", "Auto");
	string outputFile = GetOrElse(args, "o", "");
	string trialsStr = GetOrElse(args, "r", "1");
	size_t trials = stoi(trialsStr);

	string database = GetOrElse(args, "d", "");
	int thread_cnt = std::stoi(GetOrElse(args, "t", "1"));
	//bool doShuffle = GetBoolOrElse(args, "Shuffle", true);

	//set by default
	auto classifiers = ParseClassifierName(GetOrElse(args, "c", ""), args,
			thread_cnt);
	string mode = GetOrElse(args, "m", "Classification");

	if (GetBoolOrElse(args, "?", false)) {
		std::cout << "Arguments:" << std::endl;
		std::cout << "\t-f <file> Filter File:" << std::endl;
		std::cout << "\t-p <file> Packet File:" << std::endl;
		std::cout << "\t-o <file> Output File:" << std::endl;
		std::cout << "\t-r <num> number of repetitions for benchmark"
				<< std::endl;
		std::cout << "\t-c <classifier> Classifier:" << std::endl;
		std::cout << "\t-m <mode> Classification Mode:" << std::endl;
		return 0;
	}

	//assign mode and classifer
	vector<Rule> rules = InputReader::ReadFilterFile(filterFile);

	vector<Packet> packets;
	//generate 1,000,000 packets from ruleset
	if (packetFile == "Auto")
		packets = GeneratePacketsFromRuleset(rules, 1000000);
	else if (packetFile != "")
		packets = InputReader::ReadPackets(packetFile);

	//if (doShuffle) {
	//	Random rand;
	//	rules = rand.shuffle_vector(rules);
	//}

	LIKWID_MARKER_INIT;
	LIKWID_MARKER_THREADINIT;

	if (mode == "Classification") {
		RunSimulatorOnlyClassification(args, packets, rules, classifiers,
				outputFile, trials, thread_cnt);
	} else if (mode == "Update") {
		RunSimulatorUpdates(args, packets, rules, classifiers, outputFile, 1);
	} else if (mode == "Validation") {
		validation_prepare_and_run(args, packets, rules, classifiers);
	} else {
		printf("Unknown mode: %s\n", mode.c_str());
		exit(EINVAL);
	}
	LIKWID_MARKER_CLOSE;

	std::cerr << "[INFO] Done" << std::endl;
	for (auto &cls_vector : classifiers) {
		for (auto cls : cls_vector.second) {
			delete cls;
		}
	}
	return 0;
}

