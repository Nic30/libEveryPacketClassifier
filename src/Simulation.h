#pragma once

#include "likwid_common.h"
#include "ElementaryClasses.h"
#include "Utilities/MapExtensions.h"
#include <Utilities/thread_pool.h>

#include <map>
#include <unordered_map>
#include "packet_classifier.h"

enum RequestType {
	ClassifyPacket, Insertion, Deletion
};

struct Request {
	Request(RequestType request_type) :
			request_type(request_type), random_index_trace(-1) {
	}
	RequestType request_type;
	int random_index_trace;
};

struct Bookkeeper {
	Bookkeeper() {
	}
	Bookkeeper(const std::vector<Rule> rules) :
			rules(rules) {
	}
	Rule GetOneRuleAndPop(size_t i) {
		if (rules.size() == 0) {
			printf("warning: Bookeeper has no rule");
			return Rule();
		}
		Rule to_return = rules[i];
		if (i != rules.size() - 1)
			rules[i] = std::move(rules[rules.size() - 1]);
		rules.pop_back();
		return to_return;
	}
	std::vector<Rule> GetRules() const {
		return rules;
	}
	void InsertRule(const Rule &r) {
		rules.push_back(r);
	}
	int size() const {
		return rules.size();
	}
private:
	std::vector<Rule> rules;
};

class PacketClassficationSimulator {
public:
	using time_t = std::chrono::duration<double>;
	PacketClassficationSimulator(
			const std::vector<PacketClassifier*> &_packet_classifiers,
			const std::vector<Rule> &ruleset) :
			packet_classifiers(_packet_classifiers), ruleset(ruleset), pool(
					_packet_classifiers.size()) {
	}
	PacketClassficationSimulator(
			const std::vector<PacketClassifier*> &_packet_classifiers,
			const std::vector<Rule> &ruleset,
			const std::vector<Packet> &packets) :
			packet_classifiers(_packet_classifiers), ruleset(ruleset), packets(
					packets), pool(_packet_classifiers.size()) {
	}

	std::vector<Request> SetupComputation(int num_packet, int num_insert,
			int num_delete);
	void load_ruleset_into_classifier(
			std::map<std::string, std::string> &summary);
	std::vector<int> ruhn_only_packet_classification(
			std::map<std::string, std::string> &summary, size_t trials);
	static time_t run_only_packet_classification(
			const std::vector<Packet> &packets, PacketClassifier &classifier,
			size_t trials, std::vector<int> *results);
	std::vector<int> run_task_sequnce(const std::vector<Request> &sequence,
			std::map<std::string, double> &trial, size_t trial_cnt);

private:
	std::vector<Request> GenerateRequests(Random &rand, size_t num_packet,
			size_t num_insert, size_t num_delete) const;
	time_t sumTime(std::vector<std::future<time_t>> &elapsed_seconds);
	const std::vector<PacketClassifier*> &packet_classifiers;
	std::vector<Rule> ruleset;
	std::vector<Packet> packets;
	ThreadPool pool;
	Bookkeeper rules_in_use;
	Bookkeeper available_pool;
};
