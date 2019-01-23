#pragma once

#include "../Simulation.h"
#include <functional>
#include "farFilter.h"


class FaRClassifier: public PacketClassifier {
public:
	virtual void ConstructClassifier(const std::vector<Rule>& rules);
	virtual int ClassifyAPacket(const Packet& packet);

	virtual void DeleteRule(size_t index) {
		throw std::runtime_error("Can't delete rules (not implemented).");
	}
	virtual void InsertRule(const Rule& rule) {
		throw std::runtime_error("Can't insert rules (not implemented).");
	}
	virtual Memory MemSizeBytes() const {
		throw std::runtime_error("not implemented.");
	}
	virtual int MemoryAccess() const {
		throw std::runtime_error("not implemented.");
	}
	virtual size_t NumTables() const {
		throw std::runtime_error("not implemented.");
	}
	virtual size_t RulesInTable(size_t tableIndex) const {
		throw std::runtime_error("not implemented.");
	}

	virtual ~FaRClassifier() {
	}
private:
	FaRFilter filter;
};
