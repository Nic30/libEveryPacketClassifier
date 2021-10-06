#pragma once

#include "packet_classifier.h"

class ListClassifier: public PacketClassifier {
public:
	virtual void _ConstructClassifier(const std::vector<Rule> &rules) {
		this->rules = rules;
		sort(this->rules.begin(), this->rules.end(), [](const Rule &r0, const Rule &r1) {
			return r0.priority >= r1.priority;
		});
		assert(rules.size());
	}
	virtual int ClassifyAPacket(const Packet &packet) {
		for (const Rule &r : rules) {
			if (r.MatchesPacket(packet)) {
				return r.id;
			}
		}
		return -1;
	}
	virtual void DeleteRule(size_t index) {
	}
	virtual void InsertRule(const Rule &rule) {
	}
	virtual Memory MemSizeBytes() const {
		return 0;
	}
	virtual int MemoryAccess() const {
		return rules.size();
	}
	virtual size_t NumTables() const {
		return 1;
	}
	virtual size_t RulesInTable(size_t tableIndex) const {
		return rules.size();
	}

private:
	std::vector<Rule> rules;
};
