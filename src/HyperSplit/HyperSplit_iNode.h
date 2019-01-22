#pragma once
#include "../ElementaryClasses.h"

class iNode {
public:
	virtual ~iNode() {
	}

	virtual int ClassifyAPacket(const Packet& one_packet) = 0;
	virtual iNode* DeleteRule(const Rule& one_rule) = 0;
	virtual iNode* InsertRule(unsigned int leafSize, const Rule& r) = 0;
	virtual int Size(int ruleSize) const = 0;
	virtual bool IsEmpty() const = 0;

	virtual void SetBounds(const std::vector<Range1d> & bounds) = 0;
};
