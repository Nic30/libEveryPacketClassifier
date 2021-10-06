#include <construct_classifier_by_name.h>
#include "packet_classifier_from_generic_classifier.h"
#include "cut_split.h"
#include "efficuts.h"
#include "list_classifier.h"
#include "TupleMerge/TupleMergeOffline.h"
#include "OVS/TupleSpaceSearch.h"
#include "PartitionSort/PartitionSort.h"
#include "pcv/pcv.h"
#include "HyperCuts/HyperCuts.h"
#include "HyperSplit/HyperSplit.h"
#include "BitVector/BitVector.h"
#include "ByteCuts/ByteCuts.h"

using namespace std;

ClassifierSet ParseClassifierName(const string &line, const str_map &args,
		size_t count) {
	vector<string> tokens;
	Split(line, ',', tokens);
	ClassifierSet classifiers;

	for (const string &c : tokens) {
		std::function<PacketClassifier* ()> constructor;
		if (c == "List") {
			constructor = []() {
				return new ListClassifier();
			};
		} else if (c == "PartitionSort") {
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
