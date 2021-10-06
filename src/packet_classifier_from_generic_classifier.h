#pragma once

#include "Simulation.h"
#include <generic_classifier.h>
#include <stdexcept>

struct NotImplemented: std::exception {
	const char* what() const noexcept {
		return "NotImplemented\n";
	}
};

/*
 * \brief API translation from GenericClassifier to PacketClassifier
 * */
class PacketClassifierFromGenericClassifier: public PacketClassifier {

private:
	std::unique_ptr<GenericClassifier> cls;
	int rule_id_to_priority_offset;

public:
	PacketClassifierFromGenericClassifier(
			std::unique_ptr<GenericClassifier> _cls);
	virtual std::chrono::duration<double> ConstructClassifier(
			const std::vector<Rule> &rules) override;
	virtual void _ConstructClassifier(const std::vector<Rule> &rules) override {
	}
	virtual void DeleteRule(size_t index) override {
		throw NotImplemented();
	}
	virtual void InsertRule(const Rule &rule) override {
		throw NotImplemented();
	}
	virtual int MemoryAccess() const override {
		return 0;
	}
	virtual size_t NumTables() const override {
		return 0;
	}
	virtual int ClassifyAPacket(const Packet &packet) override;
	virtual Memory MemSizeBytes() const override;
	virtual size_t RulesInTable(size_t tableIndex) const override;
};
