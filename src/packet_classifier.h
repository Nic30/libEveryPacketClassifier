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
