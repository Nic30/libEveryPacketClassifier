#pragma once

#include <vector>
#include <inttypes.h>
#include <map>

template<typename T>
class DimensionCut {
	uint8_t dim;
	T cut_point;
	//std::map<T>
};


template<typename T>
class FaR_node {
public:
	using FaR_sigling_segment = std::vector<FaR_node<T>*>();

	std::vector<DimensionCut<T>> cuts;
	FaR_sigling_segment children;

};
