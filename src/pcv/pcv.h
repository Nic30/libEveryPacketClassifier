#pragma once

#include "../Simulation.h"
#include <limits>
#include <vector>
#include <pcv/partition_sort/b_tree.h>
#include <pcv/partition_sort/b_tree_search.h>
#include <pcv/partition_sort/b_tree_insert.h>
#include <pcv/partition_sort/b_tree_remove.h>
#include <pcv/partition_sort/b_tree_to_rules.h>
#include <pcv/partition_sort/b_tree_impl.h>
#include <pcv/partition_sort/rule_value_int.h>
#include <pcv/partition_sort/partition_sort_classifier.h>


using namespace std;


class Pcv: public PacketClassifier {
	using BTree = pcv::BTreeImp<pcv::_BTreeCfg<uint16_t, pcv::RuleValueInt, 7, 65535, 8>>;
	using Classifier = pcv::PartitionSortClassifer<BTree, 64, 10>;
	Classifier cls;

	template<typename T>
	void to_Range1d(pcv::Range1d<T> & dst, const Rule & src_r, Dimensions d) {
		dst.low = src_r.range[d].low;
		dst.high = src_r.range[d].high;
	}

public:

	Pcv() {
	}

	virtual ~Pcv() {
	}

	void ConstructClassifier(const std::vector<Rule>& rules) {
		for (auto & r: rules) {
			InsertRule(r);
		}
	}
	int ClassifyAPacket(const Packet& p) {
		Classifier::key_vec_t p0;
		// fill(&_r[0], r.sip);
		// fill(p, r.dip);
		// _r[4] = r.sport;
		// _r[5] = r.dport;
		// _r[6] = r.proto;
		p0[0] = p[FieldSA] >> 16;
		p0[1] = p[FieldSA] && 0xffff;
		p0[2] = p[FieldDA] >> 16;
		p0[3] = p[FieldDA] && 0xffff;
		p0[4] = p[FieldSP];
		p0[5] = p[FieldDP];
		p0[6] = p[FieldProto];

		auto r = cls.search(p0);
		return r.rule_id;
	}
	void DeleteRule(size_t index) {}
	void InsertRule(const Rule& r) {
		pcv::Rule_Ipv4_ACL r0;
		to_Range1d<uint32_t>(r0.dip, r, Dimensions::FieldDA);
		to_Range1d<uint32_t>(r0.sip, r, Dimensions::FieldSA);
		to_Range1d<uint16_t>(r0.dport, r, Dimensions::FieldDP);
		to_Range1d<uint16_t>(r0.sport, r, Dimensions::FieldSP);
		to_Range1d<uint16_t>(r0.proto, r, Dimensions::FieldProto);
		Classifier::rule_spec_t r1 = {
				pcv::rule_conv_fn::rule_to_array_16b(r0), {
						(uint32_t)r0.cummulative_prefix_len(), (uint32_t)r.id}
		};
		cls.insert(r1);
	}
	Memory MemSizeBytes() const {
		printf("warning unimplemented MemSizeBytes()\n");
		return 0;
	}

	int MemoryAccess() const {
		printf("warning unimplemented MemoryAccess()\n");
		return 0;
	}
	size_t NumTables() const {
		return cls.tree_cnt;
	}
	size_t RulesInTable(size_t index) const {
		return cls.trees.at(index)->rules.size();
	}
private:
};

