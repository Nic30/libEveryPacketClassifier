#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "BitVector/BitVector.h"
#include "ByteCuts/ByteCuts.h"
#include "ClassBenchTraceGenerator/trace_tools.h"
#include "ElementaryClasses.h"
#include "HyperCuts/HyperCuts.h"
#include "HyperSplit/HyperSplit.h"
#include "IO/InputReader.h"
#include "IO/OutputWriter.h"
#include "OVS/TupleSpaceSearch.h"
#include "PartitionSort/PartitionSort.h"
#include "pcv/pcv.h"
#include "Simulation.h"
#include "TupleMerge/TupleMergeOffline.h"
#include "Utilities/MapExtensions.h"
#include "packet_classifier_from_generic_classifier.h"
#include <cut_split.h>
#include <efficuts.h>

using namespace std;
using ClassifierSet = unordered_map<string, std::vector<PacketClassifier*>>;
using str_map = unordered_map<string, string>;

ClassifierSet ParseClassifierName(const string &line, const str_map &args,
		size_t count) {
	vector<string> tokens;
	Split(line, ',', tokens);
	ClassifierSet classifiers;

	for (const string &c : tokens) {
		std::function<PacketClassifier* ()> constructor;
		if (c == "PartitionSort") {
			constructor = []() {
				return new PartitionSort();
			};
		} else if (c == "PriorityTupleSpaceSearch") {
			constructor = []() {
				return new PriorityTupleSpaceSearch();
			};
		} else if (c == "HyperSplit") {
			constructor = [&args]() {
				return new HyperSplit(args);
			};
		} else if (c == "HyperCuts") {
			constructor = []() {
				return new HyperCuts();
			};
		} else if (c == "ByteCuts") {
			constructor = [&args]() {
				return new ByteCutsClassifier(args);
			};
		} else if (c == "BitVector") {
			constructor = []() {
				return new BitVector();
			};
		} else if (c == "TupleSpaceSearch") {
			constructor = []() {
				return new TupleSpaceSearch();
			};
		} else if (c == "TupleMergeOnline") {
			constructor = [&args]() {
				return new TupleMergeOnline(args);
			};
		} else if (c == "TupleMergeOffline") {
			constructor = [&args]() {
				return new TupleMergeOffline(args);
			};
		} else if (c == "CutSplit") {
			constructor = []() {
				return new PacketClassifierFromGenericClassifier(
						std::make_unique<CutSplit>(8, 8));
			};
		} else if (c == "EffiCuts") {
			constructor = []() {
				return new PacketClassifierFromGenericClassifier(
						std::make_unique<EffiCuts>(8));
			};
		} else if (c == "pcv") {
			constructor = []() {
				return new Pcv();
			};
		} else {
			printf("Unknown ClassifierTests: %s\n", c.c_str());
			exit(EINVAL);
		}
		for (size_t i = 0; i < count; i++) {
			classifiers[c].push_back(constructor());
		}
	}
	return classifiers;
}

vector<int> RunSimulatorClassificationTrial(Simulator &s, const string &name,
		vector<map<string, string>> &data, size_t trials) {
	map<string, string> d = { { "Classifier", name } };
	printf("%s\n", name.c_str());
	auto r = s.PerformOnlyPacketClassification(d, trials);
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
		Simulator s(pair.second, rules, packets);
		RunSimulatorClassificationTrial(s, pair.first.c_str(), data, trials);
	}

	if (outfile != "") {
		OutputWriter::WriteJsonFile(outfile, header, data);
	}
	return make_pair(header, data);
}

void RunSimulatorUpdateTrial(Simulator &s, const string &name,
		const vector<Request> &req,
		vector<map<string, string>> &data, int reps) {

	map<string, string> d = { { "Classifier", name } };
	map<string, double> trial;

    s.PerformTaskSequnce(req, trial, reps);
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
		Simulator s(pair.second, rules, packets);
		const auto req = s.SetupComputation(0, 500000, 500000);
		RunSimulatorUpdateTrial(s, pair.first.c_str(), req, data, repetitions);
	}
	if (outfile != "") {
		OutputWriter::WriteCsvFile(outfile, header, data);
	}
	return make_pair(header, data);
}

bool Validation(const ClassifierSet classifiers,
		const vector<Rule> &rules, const vector<Packet> &packets,
		int threshold) {
	int numWrong = 0;
	vector<Rule> sorted = rules;
	sort(sorted.begin(), sorted.end(), [](const Rule &rx, const Rule &ry) {
		return rx.priority >= ry.priority;
	});
	for (const Packet &p : packets) {
		unordered_map<string, int> results;
		int result = -1;
		for (const auto &pair : classifiers) {
			result = pair.second[0]->ClassifyAPacket(p);
			results[pair.first] = result;
		}
		if (!all_of(results.begin(), results.end(), [=](const auto &pair) {
			return pair.second == result;
		})) {
			numWrong++;
			for (auto x : p) {
				std::cout << x << " ";
			}
			std::cout << std::endl;
			for (const auto &pair : results) {
				std::cout << "\t" << pair.first << ": " << pair.second
						<< std::endl;
			}
			for (const Rule &r : sorted) {
				if (r.MatchesPacket(p)) {
					std::cout << "\tTruth: " << r.priority << std::endl;
					break;
				}
			}
		}
		if (numWrong >= threshold) {
			break;
		}
	}
	return numWrong == 0;
}

void RunValidation(const unordered_map<string, string> &args,
		const vector<Packet> &packets, const vector<Rule> &rules,
		ClassifierSet classifiers) {
	std::cerr << "[INFO] Validation Simulation, Building..." << std::endl;
	for (auto &pair : classifiers) {
		std::cerr << "[INFO] building" << pair.first << std::endl;
		pair.second[0]->_ConstructClassifier(rules);
	}

	int threshold = GetIntOrElse(args, "Validate.Threshold", 10);
	if (Validation(classifiers, rules, packets, threshold)) {
		std::cerr << "[INFO] All classifiers are in accord" << std::endl;
	}
}

int main(int argc, char *argv[]) {
	unordered_map<string, string> args = ParseArgs(argc, argv);

	string filterFile = GetOrElse(args, "f", "");
	string packetFile = GetOrElse(args, "p", "Auto");
	string outputFile = GetOrElse(args, "o", "");
	string trialsStr = GetOrElse(args, "r", "1");

	string database = GetOrElse(args, "d", "");
	int thread_cnt = std::stoi(GetOrElse(args, "t", "1"));
	bool doShuffle = GetBoolOrElse(args, "Shuffle", true);

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

	if (doShuffle) {
		rules = Random::shuffle_vector(rules);
	}

	LIKWID_MARKER_INIT;
	LIKWID_MARKER_THREADINIT;

	if (mode == "Classification") {
		size_t trials = stoi(trialsStr);
		RunSimulatorOnlyClassification(args, packets, rules, classifiers,
				outputFile, trials, thread_cnt);
	} else if (mode == "Update") {
		RunSimulatorUpdates(args, packets, rules, classifiers, outputFile, 1);
	} else if (mode == "Validation") {
		RunValidation(args, packets, rules, classifiers);
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

