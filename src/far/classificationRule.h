#pragma once
#include "fragmentedValue.h"

template<typename DIM_t, size_t DIM_CNT>
class ClassificationRule: public FragmentedValue<MaskedValue<DIM_t>, DIM_CNT> {
	using _fv_t = FragmentedValue<MaskedValue<DIM_t>, DIM_CNT>;
public:
	int id = 0;

	ClassificationRule(std::array<MaskedValue<DIM_t>, DIM_CNT> a, int id) :
			_fv_t(a), id(id) {
	}
	//ClassificationRule(std::array<MaskedValue<DIM_t>, DIM_CNT> & a, int id) :
	//		_fv_t(a), id(id) {
	//}
};
