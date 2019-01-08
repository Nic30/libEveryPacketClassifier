#include "exceptions.h"

NonComparableErr::NonComparableErr() :
		std::runtime_error("NonComparable") {
}
