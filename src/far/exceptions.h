#pragma once
#include <stdexcept>

class NonComparableErr: std::runtime_error {
public:
	using std::runtime_error::runtime_error;
	NonComparableErr();
};
