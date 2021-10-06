#pragma once
#include <unordered_set>
#include <string>
#include <vector>
#include <packet_classifier.h>

using ClassifierSet = std::unordered_map<std::string, std::vector<PacketClassifier*>>;
using str_map = std::unordered_map<std::string, std::string>;

ClassifierSet ParseClassifierName(const std::string &line, const str_map &args,
		size_t count);
