#pragma once

#include <limits>
#include <array>
#include <exception>
#include <assert.h>

#include <boost/functional/hash.hpp>

class NonComparableErr: std::runtime_error {
public:
	using std::runtime_error::runtime_error;
	NonComparableErr() :
			std::runtime_error("NonComparable") {
	}
};

/*
 * :ivar vldMask: bit mask for value, it is expected that mask is set only
 *                for continuous prefix of value or is 0
 *
 **/
template<typename T>
class MaskedValue {
public:
	T value;
	T vldMask;
	int priority;

	explicit MaskedValue(T val, T mask = std::numeric_limits<T>::max(),
			int prio = -1) :
			value(val), vldMask(vldMask), priority(prio) {
		assert((val & mask) == val);
		if (mask != 0)
			assert(1 << (8 * sizeof(T) - 1) & mask);
	}

	/*
	 * :return: true if value is subinterval in other value
	 **/
	inline bool isContainedIn(const MaskedValue<T> & other) const {
		return (vldMask >= other.vldMask
				&& (value & other.vldMask) == other.value);
	}

	/*
	 * :return: true if there are not bits with undefined value
	 **/
	inline bool isFullyDefined() const {
		return vldMask == std::numeric_limits<T>::max();
	}

	inline bool isWildard() const {
		return vldMask == 0;
	}

	/**
	 * Check if range defined by the values is overlapping
	 *
	 * e.g. 00xx and 0xxx are overlapping
	 *      01xx and 00xx are not
	 *
	 **/
	inline bool isOverlapping(const MaskedValue<T> & other) const {
		T min_mask = vldMask < other.vldMask ? vldMask : other.vldMask;
		return (value && min_mask) == (other.value && min_mask);
	}

	//bool operator <(const MaskedValue<T> & other) const {
	//	if (vldMask == other.vldMask) {
	//		// 00x < 01x -> true
	//		return value <= other.value;
	//	} else if (vldMask > other.vldMask) {
	//		if (value < other.value) {
	//			// 00x < 1xx -> false
	//			return true;
	//		} else {
	//			// 00x < 0xx -> non-comparable
	//			// 01x < 0xx -> non-comparable
	//			throw NonComparableErr();
	//		}
	//	} else { // vldMask < other.vldMask
	//		if (value == other.value) {
	//			// 0xx < 00x -> false
	//			return false;
	//		} else if (value > other.value) {
	//			// 1xx < 00x -> false
	//			return false;
	//		} else {
	//			// 0xx < 01x -> non-comparable
	//			throw NonComparableErr();
	//		}
	//	}
	//}

	inline bool operator <=(const MaskedValue<T> & other) const {
		if (vldMask == other.vldMask) {
			// 00x < 01x -> true
			return value <= other.value;
		} else if (vldMask > other.vldMask) {
			if (value <= other.value) {
				// 00x <= 0xx -> false
				return true;
			} else {
				// 01x < 0xx -> non-comparable
				throw NonComparableErr();
			}
		} else { // vldMask < other.vldMask
			if (value == other.value) {
				// 0xx <= 00x -> true
				return true;
			} else if (value > other.value) {
				// 1xx < 00x -> false
				return false;
			} else {
				// 0xx < 01x -> non-comparable
				throw NonComparableErr();
			}
		}
	}

	inline bool operator ==(const MaskedValue<T> & other) const {
		if (vldMask != other.vldMask)
			throw NonComparableErr();
		else
			return value == other.value;
	}
};

template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
class FragmentedValue: public std::array<FRAGMENT_t, FRAGMENT_CNT> {
	inline bool operator <=(
			const FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> & other) const {
		for (size_t i; i < FRAGMENT_CNT; i++) {
			if (!((*this)[i] <= other[i]))
				return false;
		}
		return true;
	}

	inline bool operator ==(
			const FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> & other) const {
		for (size_t i; i < FRAGMENT_CNT; i++) {
			if (!((*this)[i] == other[i]))
				return false;
		}
		return true;
	}
};

/// Primary class template hash.
template<typename _Tp>
struct hash_mask;

template<typename FRAGMENT_t>
struct hash_mask<MaskedValue<FRAGMENT_t>> {
	typedef MaskedValue<FRAGMENT_t> argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& v) const noexcept {
		return std::hash<FRAGMENT_t> { }(v.vldMask);
	}
};

template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
struct hash_mask<FragmentedValue<FRAGMENT_t, FRAGMENT_CNT>> {
	typedef FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& fv) const noexcept {
		result_type h = 0;
		for (const auto & v : fv) {
			h = boost::hash_combine(v,
					hash_mask<MaskedValue<FRAGMENT_t>> { }(v));
		}
		return h;
	}
};

template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
struct hash_mask_eq {
	typedef FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> argument_type;
	bool operator()(const argument_type& l, const argument_type& r) const {
		for (size_t i; i < l.size(); i++) {
			if (!((*this)[i].vldMask != r[i].vldMask))
				return false;
		}
		return true;
	}
};

// inject serialization methods to std namespace so other object like unordered_set can use it
// without an extra specification
namespace std {

template<typename FRAGMENT_t>
struct hash<MaskedValue<FRAGMENT_t>> {
	typedef MaskedValue<FRAGMENT_t> argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& v) const noexcept {
		result_type h = boost::hash_combine(std::hash<FRAGMENT_t> { }(v.value),
				std::hash<FRAGMENT_t> { }(v.vldMask));
		h = boost::hash_combine(h, std::hash<int> { }(v.priority));
		return h;
	}
};

template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
struct hash<FragmentedValue<FRAGMENT_t, FRAGMENT_CNT>> {
	typedef FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& fv) const noexcept {
		result_type h = 0;
		for (const auto & v : fv) {
			h = boost::hash_combine(v,
					std::hash<MaskedValue<FRAGMENT_t>> { }(v));
		}
		return h;
	}
};

}

