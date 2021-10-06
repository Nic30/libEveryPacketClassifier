#include "packet_classifier_from_generic_classifier.h"

PacketClassifierFromGenericClassifier::PacketClassifierFromGenericClassifier(
		std::unique_ptr<GenericClassifier> _cls) :
		cls(std::move(_cls)), rule_id_to_priority_offset(0) {
}

std::chrono::duration<double> PacketClassifierFromGenericClassifier::ConstructClassifier(
		const std::vector<Rule> &rules) {
	std::list<openflow_rule> rule_db;
	int i = 0;
	for (auto &r : rules) {
		openflow_rule ofr;
		ofr.priority = r.priority;
		assert(ofr.priority == i);
		i++;
		assert(r.range.size() == cls->get_supported_number_of_fields());
		for (auto &f : r.range) {
			assert(f.low <= f.high);
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
	// returns The matching rule action/priority (or 0xffffffff (-1) if not found)
	int m = cls->classify_sync(
			reinterpret_cast<const unsigned int*>(&packet[0]), -1);
	return m;
}

Memory PacketClassifierFromGenericClassifier::MemSizeBytes() const {
	return cls->get_size();
}
size_t PacketClassifierFromGenericClassifier::RulesInTable(
		size_t tableIndex) const {
	return cls->get_num_of_rules();
}
