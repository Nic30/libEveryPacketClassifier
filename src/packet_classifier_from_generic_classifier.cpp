#include "packet_classifier_from_generic_classifier.h"

PacketClassifierFromGenericClassifier::PacketClassifierFromGenericClassifier(
		std::unique_ptr<GenericClassifier> _cls) :
		cls(std::move(_cls)) {
}

std::chrono::duration<double> PacketClassifierFromGenericClassifier::ConstructClassifier(
		const std::vector<Rule> &rules) {
	std::list<openflow_rule> rule_db;
	for (auto &r : rules) {
		openflow_rule ofr;
		ofr.priority = r.priority;
		for (auto &f : r.range) {
			range rew_ra(f.low, f.high);
			static_assert(sizeof f.low == sizeof rew_ra.low);
			ofr.fields.push_back(rew_ra);
		}
		rule_db.push_back(ofr);
	}

	cls->build(rule_db);
	auto build_time_ms = cls->get_build_time();
	std::chrono::duration<double> t = std::chrono::milliseconds(build_time_ms);
	return t;
}
int PacketClassifierFromGenericClassifier::ClassifyAPacket(
		const Packet &packet) {
	// returns 0xffffffff if not matching, which is also -1, no conversion required
	auto m = cls->classify_sync(
			reinterpret_cast<const unsigned int*>(&packet[0]), 0);
	return (int) m;
}

Memory PacketClassifierFromGenericClassifier::MemSizeBytes() const {
	return cls->get_size();
}
size_t PacketClassifierFromGenericClassifier::RulesInTable(
		size_t tableIndex) const {
	return cls->get_num_of_rules();
}
