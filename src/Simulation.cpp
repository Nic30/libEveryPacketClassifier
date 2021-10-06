#include "Simulation.h"
#include <string>
#include <sstream>

std::vector<Request> PacketClassficationSimulator::GenerateRequests(
		Random &rand, size_t num_packet, size_t num_insert,
		size_t num_delete) const {
	if (num_packet > packets.size()) {
		printf(
				"Warning in Simulator::GenerateRequests : too much request for num_packet--setting num to available size\n");
		num_packet = packets.size();
	}
	std::vector<Request> vr;
	for (size_t i = 0; i < num_packet; i++)
		vr.push_back(Request(RequestType::ClassifyPacket));
	for (size_t i = 0; i < num_insert; i++)
		vr.push_back(Request(RequestType::Insertion));
	for (size_t i = 0; i < num_delete; i++)
		vr.push_back(Request(RequestType::Deletion));
	return rand.shuffle_vector(vr);
}

std::vector<Request> PacketClassficationSimulator::SetupComputation(
		int num_packet, int num_insert, int num_delete) {
	Random rand;
	std::vector<Request> sequence = GenerateRequests(rand, num_packet,
			num_insert, num_delete);
	std::vector<Rule> shuff_rules = ruleset;

	shuff_rules = rand.shuffle_vector(shuff_rules);
	Bookkeeper rules_in_use_temp = (std::vector<Rule>(std::begin(shuff_rules),
			std::begin(shuff_rules) + shuff_rules.size() / 2));
	Bookkeeper available_pool_temp = (std::vector<Rule>(
			std::begin(shuff_rules) + shuff_rules.size() / 2,
			std::end(shuff_rules)));

	//need to adjust the case hitting threshold where there is nothing to delete or elements are full but trying to add
	int current_size = rules_in_use_temp.size();
	double treshold = 0.2;
	for (size_t i = 0; i < sequence.size(); i++) {
		Request n = sequence[i];
		Rule temp_rule;
		switch (n.request_type) {
		case RequestType::ClassifyPacket:
			break;
		case RequestType::Insertion:
			if (current_size >= (1 - treshold) * ruleset.size()) {
				///find furthest deletion and swap
				for (size_t j = sequence.size() - 1; j > i; j--) {
					if (sequence[j].request_type == RequestType::Deletion) {
						sequence[i].request_type = RequestType::Deletion;
						sequence[j].request_type = RequestType::Insertion;
						current_size--;
						break;
					}
				}
				//current_size++;
				//sequence[i].request_type = RequestType::Deletion;
				break;
			}
			current_size++;
			break;
		case RequestType::Deletion:
			if (current_size <= treshold * ruleset.size()) {
				///find farthest insertion and swap
				for (size_t j = sequence.size() - 1; j > i; j--) {
					if (sequence[j].request_type == RequestType::Insertion) {
						sequence[j].request_type = RequestType::Deletion;
						sequence[i].request_type = RequestType::Insertion;
						current_size++;
						break;
					}
				}
				//	current_size--;
				//	sequence[i].request_type = RequestType::Insertion;
				break;
			}
			current_size--;
			break;
		default:
			break;
		}
	}
	for (Request &n : sequence) {
		Rule temp_rule;
		// int result = -1;
		int index_delete = -1;
		int index_insert = -1;
		switch (n.request_type) {
		case RequestType::ClassifyPacket:
			break;
		case RequestType::Insertion:
			if (current_size >= (1 - treshold) * ruleset.size()) {
				printf(
						"Warning skipped perform insertion: no available pool\n");
				break;
			}
			index_insert = rand.random_int(0, available_pool_temp.size() - 1);
			temp_rule = available_pool_temp.GetOneRuleAndPop(index_insert);
			rules_in_use_temp.InsertRule(temp_rule);
			n.random_index_trace = index_insert;
			break;
		case RequestType::Deletion:
			if (current_size <= treshold * ruleset.size()) {
				printf(
						"Warning skipped perform deletion: no avilable rules_in_use\n");
				break;
			}
			index_delete = rand.random_int(0, rules_in_use_temp.size() - 1);
			temp_rule = rules_in_use_temp.GetOneRuleAndPop(index_delete);
			available_pool_temp.InsertRule(temp_rule);
			n.random_index_trace = index_delete;
			break;
		default:
			break;
		}
	}

	rules_in_use = (std::vector<Rule>(std::begin(shuff_rules),
			std::begin(shuff_rules) + shuff_rules.size() / 2));
	available_pool = (std::vector<Rule>(
			std::begin(shuff_rules) + shuff_rules.size() / 2,
			std::end(shuff_rules)));

	return sequence;
}
PacketClassficationSimulator::time_t PacketClassficationSimulator::sumTime(
		std::vector<std::future<time_t>> &times_of_execution) {
	time_t res(0);
	for (auto &t : times_of_execution) {
		res += t.get();
	}
	return res / packet_classifiers.size();;
}

void PacketClassficationSimulator::load_ruleset_into_classifier(
		std::map<std::string, std::string> &summary) {
	std::vector<std::future<time_t>> elapsed_time;
	// Submit a lambda object to the pool.
	for (size_t i = 0; i < packet_classifiers.size(); i++) {
		pool.enqueue([this, i]() {
			return packet_classifiers[i]->ConstructClassifier(ruleset);
		});
	}

	auto res = sumTime(elapsed_time);
	std::cout << "\tConstruction time: " << res.count() << " s" << std::endl;
	summary["ConstructionTime(ms)"] = std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(res).count());
}

PacketClassficationSimulator::time_t PacketClassficationSimulator::run_only_packet_classification(
		const std::vector<Packet> &packets, PacketClassifier &classifier,
		size_t trials, std::vector<int> *results) {

	std::chrono::time_point<std::chrono::steady_clock> start, end;
	time_t sum_time(0);

	LIKWID_MARKER_START("classification");
	assert(trials > 0);
	for (size_t t = 0; t < trials; t++) {
		if (results)
			results->clear();
		start = std::chrono::steady_clock::now();
		for (auto const &p : packets) {
			auto r = classifier.ClassifyAPacket(p);
			if (results)
				results->push_back(r);
		}
		end = std::chrono::steady_clock::now();
		time_t elapsed_seconds = end - start;
		sum_time += elapsed_seconds;
	}
	LIKWID_MARKER_STOP("classification");
	return sum_time;
}

std::vector<int> PacketClassficationSimulator::ruhn_only_packet_classification(
		std::map<std::string, std::string> &summary, size_t trials) {

	std::vector<std::future<time_t>> elapsed_time;
	load_ruleset_into_classifier(summary);
	std::vector<int> results;
	results.reserve(packets.size());

	// Submit a lambda object to the pool.
	for (size_t i = 0; i < packet_classifiers.size(); i++) {
		auto &cls = *packet_classifiers[i];
		auto &_packets = packets;
		auto t = pool.enqueue(
				[&cls, &_packets, i, &results, trials]() {
					return run_only_packet_classification(_packets, cls, trials,
							i == 0 ? &results : nullptr);
				});
		elapsed_time.push_back(std::move(t));
	}

	// Wait for all tasks in the pool to complete.

	auto sum_time = sumTime(elapsed_time);
	sum_time /= trials;
	std::cout << "\tClassification time: " << sum_time.count() << " s"
			<< std::endl;
	summary["ClassificationTime(s)"] = std::to_string(sum_time.count());

	PacketClassifier &classifier = *packet_classifiers[0];
	int memSize = classifier.MemSizeBytes();
	std::cout << "\tSize(bytes): " << memSize << std::endl;
	summary["Size(bytes)"] = std::to_string(memSize);
	int memAccess = classifier.MemoryAccess();
	std::cout << "\tMemoryAccess: " << memAccess << std::endl;
	summary["MemoryAccess"] = std::to_string(memAccess);
	int numTables = classifier.NumTables();
	std::cout << "\tTables: " << numTables << std::endl;
	summary["Tables"] = std::to_string(numTables);

	std::stringstream ssTableSize, ssTableQuery;
	for (int i = 0; i < numTables; i++) {
		if (i != 0) {
			ssTableSize << "-";
			ssTableQuery << "-";
		}
		ssTableSize << classifier.RulesInTable(i);
		ssTableQuery << classifier.NumPacketsQueriedNTables(i + 1);
	}
	summary["TableSizes"] = ssTableSize.str();
	summary["TableQueries"] = ssTableQuery.str();

	printf("\tTotal tables queried: %d\n", classifier.TablesQueried());
	printf("\tAverage tables queried: %f\n",
			1.0 * classifier.TablesQueried() / (trials * packets.size()));
	summary["AvgQueries"] = std::to_string(
			1.0 * classifier.TablesQueried() / (trials * packets.size()));

	return results;
}

std::vector<int> PacketClassficationSimulator::run_task_sequnce(
		const std::vector<Request> &sequence,
		std::map<std::string, double> &res, size_t trial_cnt) {
	if (available_pool.size() == 0) {
		throw std::runtime_error(
				"Warning no available pool left: need to generate computation first\n");
	}

	std::vector<std::future<time_t>> elapsed_time_total(
			packet_classifiers.size());

	std::vector<int> _results;
	for (size_t i = 0; i < packet_classifiers.size(); i++) {
		auto t = pool.enqueue([this, i, &_results, trial_cnt, &sequence]() {
			PacketClassifier &classifier = *packet_classifiers[i];
			Bookkeeper rules_in_use_temp = rules_in_use;
			Bookkeeper available_pool_temp = available_pool;

			time_t elapsed_seconds = classifier.ConstructClassifier(rules_in_use_temp.GetRules());

			for (size_t t = 0; t < trial_cnt; t++) {
				std::vector<int> *results = nullptr;
				if (i == 0 && t == 0) {
					results = &_results;
					results->clear();
					results->reserve(trial_cnt * sequence.size());
				}

				size_t packet_counter = 0;
				std::chrono::time_point<std::chrono::steady_clock> start, end;
				//invariant: at all time, DS.rules = rules_in_use.rules
				time_t elapsed_seconds_cnt2(0);
				for (Request n : sequence) {
					Rule temp_rule;
					int result = -1;
					switch (n.request_type) {
					case RequestType::ClassifyPacket:
						/*if (packets.size() == 0) {
						 printf("Warning packets.size() = 0 in packet request");
						 break;
						 }*/
						start = std::chrono::steady_clock::now();
						result = classifier.ClassifyAPacket(
								packets[packet_counter++]);
						end = std::chrono::steady_clock::now();
						elapsed_seconds_cnt2 += end - start;
						if (packet_counter == packets.size())
							packet_counter = 0;
						if (results)
							results->push_back(result);
						break;
					case RequestType::Insertion:
						if (available_pool.size() == 0) {
							printf(
									"Warning skipped perform insertion: no available pool\n");
							break;
						}
						temp_rule = available_pool_temp.GetOneRuleAndPop(
								n.random_index_trace);
						rules_in_use_temp.InsertRule(temp_rule);
						start = std::chrono::steady_clock::now();
						classifier.InsertRule(temp_rule);
						end = std::chrono::steady_clock::now();
						elapsed_seconds_cnt2 += end - start;

						break;
					case RequestType::Deletion:
						if (rules_in_use.size() == 0) {
							printf(
									"Warning skipped perform deletion: no avilable rules_in_use\n");
							break;
						}
						temp_rule = rules_in_use_temp.GetOneRuleAndPop(
								n.random_index_trace);
						available_pool_temp.InsertRule(temp_rule);

						start = std::chrono::steady_clock::now();
						classifier.DeleteRule(n.random_index_trace);
						end = std::chrono::steady_clock::now();
						elapsed_seconds_cnt2 += end - start;

						break;
					default:
						break;
					}
				}
				elapsed_seconds += elapsed_seconds_cnt2;
			}
			return elapsed_seconds;
		});
		elapsed_time_total.push_back(std::move(t));
	}

	// Wait for all tasks in the pool to complete.
	auto sum_elapsed2 = sumTime(elapsed_time_total);
	printf("\tUpdateTime time: %f \n", sum_elapsed2.count() / trial_cnt);
	if (!res.count("UpdateTime(s)")) {
		res["UpdateTime(s)"] = 0;
	}
	res["UpdateTime(s)"] += sum_elapsed2.count() / trial_cnt;

	return _results;
}
