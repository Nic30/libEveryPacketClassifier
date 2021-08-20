#pragma once

#include "likwid_common.h"
#include "ElementaryClasses.h"
#include "Utilities/MapExtensions.h"
#include <Utilities/thread_pool.h>

#include <map>
#include <unordered_map>

typedef uint32_t Memory;

class PartitionPacketClassifier {
public:
	virtual int ComputeNumberOfBuckets(const std::vector<Rule> &rules) = 0;
	virtual ~PartitionPacketClassifier() {
	}
};

class ClassifierTable {
public:
	virtual int ClassifyAPacket(const Packet &packet) const = 0;
	virtual void Insertion(const Rule &rule, bool &priorityChange) = 0;
	virtual void Deletion(const Rule &rule, bool &priorityChange) = 0;
	virtual bool CanInsertRule(const Rule &rule) const = 0;

	virtual size_t NumRules() const = 0;
	virtual int MaxPriority() const = 0;

	virtual Memory MemSizeBytes(Memory ruleSize) const = 0;
	virtual std::vector<Rule> GetRules() const = 0;
	virtual ~ClassifierTable() {
	}
};

class PacketClassifier {
public:
	virtual std::chrono::duration<double> ConstructClassifier(
			const std::vector<Rule> &rules) {
		std::chrono::time_point<std::chrono::steady_clock> start, end;
		std::chrono::duration<double> elapsed_seconds;
		start = std::chrono::steady_clock::now();
		LIKWID_MARKER_START("classifier_construction");
		_ConstructClassifier(rules);
		LIKWID_MARKER_STOP("classifier_construction");
		end = std::chrono::steady_clock::now();
		elapsed_seconds = end - start;
		return elapsed_seconds;
	}
	virtual void _ConstructClassifier(const std::vector<Rule> &rules) = 0;
	virtual int ClassifyAPacket(const Packet &packet) = 0;
	virtual void DeleteRule(size_t index) = 0;
	virtual void InsertRule(const Rule &rule) = 0;
	virtual Memory MemSizeBytes() const = 0;
	virtual int MemoryAccess() const = 0;
	virtual size_t NumTables() const = 0;
	virtual size_t RulesInTable(size_t tableIndex) const = 0;

	int TablesQueried() const {
		return queryCount;
	}
	int NumPacketsQueriedNTables(int n) const {
		return GetOrElse<int, int>(packetHistogram, n, 0);
	}
	;
	virtual ~PacketClassifier() {
	}

protected:
	/**
	 * Update query counters for performance analysis
	 */
	void QueryCountersUpdate(int query) {
		packetHistogram[query]++;
		queryCount += query;
	}

private:
	int queryCount = 0;
	std::unordered_map<int, int> packetHistogram;
};

class ListClassifier: public PacketClassifier {
public:
	virtual void _ConstructClassifier(const std::vector<Rule> &rules) {
		this->rules = rules;
	}
	virtual int ClassifyAPacket(const Packet &packet) {
		for (const Rule &r : rules) {
			if (r.MatchesPacket(packet)) {
				return r.priority;
			}
		}
		return -1;
	}
	virtual void DeleteRule(size_t index) {
	}
	;
	virtual void InsertRule(const Rule &rule) {
	}
	;
	virtual Memory MemSizeBytes() const {
		return 0;
	}
	;
	virtual int MemoryAccess() const {
		return rules.size();
	}
	virtual size_t NumTables() const {
		return 1;
	}
	;
	virtual size_t RulesInTable(size_t tableIndex) const {
		return rules.size();
	}
	;

private:
	std::vector<Rule> rules;
};

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

class Simulator {
public:
	using time_t = std::chrono::duration<double>;
	Simulator(const std::vector<PacketClassifier*> &_packet_classifiers,
			const std::vector<Rule> &ruleset) :
			packet_classifiers(_packet_classifiers), ruleset(ruleset), pool(
					_packet_classifiers.size()) {
	}
	Simulator(const std::vector<PacketClassifier*> &_packet_classifiers,
			const std::vector<Rule> &ruleset,
			const std::vector<Packet> &packets) :
			packet_classifiers(_packet_classifiers), ruleset(ruleset), packets(
					packets), pool(_packet_classifiers.size()) {
	}

	std::vector<Request> SetupComputation(int num_packet, int num_insert,
			int num_delete);
	void PerformRuleLoad(std::map<std::string, std::string> &summary);
	std::vector<int> PerformOnlyPacketClassification(
			std::map<std::string, std::string> &summary, size_t trials);
	static time_t PerformOnlyPacketClassificationTask(
			const std::vector<Packet> &packets, PacketClassifier &classifier,
			size_t trials, std::vector<int> *results);
	std::vector<int> PerformTaskSequnce(const std::vector<Request> &sequence,
			std::map<std::string, double> &trial, size_t trial_cnt);

private:
	std::vector<Request> GenerateRequests(size_t num_packet, size_t num_insert,
			size_t num_delete) const;
	time_t sumTime(
			std::vector<std::future<time_t>> &elapsed_seconds);
	const std::vector<PacketClassifier*> &packet_classifiers;
	std::vector<Rule> ruleset;
	std::vector<Packet> packets;
	ThreadPool pool;
	Bookkeeper rules_in_use;
	Bookkeeper available_pool;
};
