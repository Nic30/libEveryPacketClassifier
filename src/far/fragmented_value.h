#pragma once

#include <limits>
#include <array>
#include <assert.h>
#include <initializer_list>
#include <functional>

#include <boost/functional/hash.hpp>
#include "exceptions.h"


/*
 * :note: interval defined by low high is defined as closed interval <low, high>
 * :ivar value: prefix or low if this is range
 * :ivar prefix_len: length of prefix or length of common  prefix between low and high boundary
 * :ivar vldMask: bit mask for value, it is expected that mask is set only
 *                for continuous prefix of value or is 0
 * :note: vldMask cached to simplify later evaluations
 *
 **/
template<typename T>
class MaskedValue {
private:
public:
	using value_type = T;
	T value;

	size_t prefix_len;
	T prefixMask;

	int priority;

	bool is_range;
	T _M_high;

	static constexpr size_t BIT_LEN = sizeof(T) * 8;
	static constexpr size_t ALL_MASK = std::numeric_limits<T>::max();

	constexpr T & high() {
		return _M_high;
	}
	constexpr T & low() {
		return value;
	}

	constexpr const T & high() const {
		return _M_high;
	}
	constexpr const T & low() const {
		return value;
	}

	static MaskedValue<T> from_range(T low, T high, int priority = -1) {
		assert(low <= high);
		size_t i;
		for (i = 0; i < BIT_LEN; i++) {
			auto m = T(1) << i;
			if ((low & m) != (high & m)) {
				// common prefix ends there because bits are different
				break;
			}
		}
		MaskedValue<T> self(low, i, priority);
		self.is_range = true;
		// high will not fit exactly if range is not aligned
		self._M_high = high;

		return self;
	}

	/*
	 * Get prefix mask by prefix length
	 * */
	static constexpr T getMask(size_t len) {
		// get mask and shift it to msb area
		return ((T(1) << len) - 1) << (BIT_LEN - len);
	}

	static constexpr T getRange(size_t prefix_len) {
		return (T(1) << (BIT_LEN - prefix_len));
	}

	MaskedValue(T prefix, size_t prefix_len = BIT_LEN, int prio = -1) :
			value(prefix), prefix_len(prefix_len), prefixMask(
					getMask(prefix_len)), priority(prio), is_range(false), _M_high(
					prefix + getRange(prefix_len) - 1) {
		assert(prefix_len <= BIT_LEN);
	}

	/*
	 * :return: true if this value is subinterval in other value
	 **/
	constexpr bool isContainedIn(const MaskedValue<T> & other) const {
		//  l < > h
		//  l < h >
		//  l h < >
		//  < > l h
		//  < l > h
		//  < l h > -> true
		if (other.low() <= low() && high() <= other.high())
			return true;
		else
			return false;
	}

	/*
	 * :return: true if there are not bits with undefined value
	 **/
	constexpr bool isFullyDefined() const {
		return prefix_len == BIT_LEN;
	}

	constexpr bool isWildard() const {
		return prefix_len == 0;
	}

	/**
	 * Check if range defined by the values is overlapping
	 *
	 * e.g. 00xx and 0xxx are overlapping
	 *      01xx and 00xx are not
	 *
	 **/
	constexpr bool isOverlapping(const MaskedValue<T> & other) const {
		//  l < > h
		//  l < h >
		//  l h < > -> false  1.
		//  < > l h -> false  2.
		//  < l > h
		//  < l h >
		if (high() < other.low() || other.high() < low()) {
			return false;
		}
		return true;
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
	inline bool operator <(const MaskedValue<T> & other) const {
		if (isOverlapping(other)) {
			if ((prefix_len == BIT_LEN && other.prefix_len == BIT_LEN)
					&& (*this == other)) {
				return true;
			}
			throw NonComparableErr();
		}
		return high() < other.low();
	}
	inline bool operator <=(const MaskedValue<T> & other) const {
		if (isOverlapping(other)) {
			if (*this == other)
				return true;
			throw NonComparableErr();
		}
		return high() <= other.low();
		//if (vldMask == other.vldMask) {
		//	// 00x < 01x -> true
		//	return value <= other.value;
		//} else if (vldMask > other.vldMask) {
		//	if (value <= other.value) {
		//		// 00x <= 0xx -> false
		//		return true;
		//	} else {
		//		// 01x < 0xx -> non-comparable
		//		throw NonComparableErr();
		//	}
		//} else { // vldMask < other.vldMask
		//	if (value == other.value) {
		//		// 0xx <= 00x -> true
		//		return true;
		//	} else if (value > other.value) {
		//		// 1xx < 00x -> false
		//		return false;
		//	} else {
		//		// 0xx < 01x -> non-comparable
		//		throw NonComparableErr();
		//	}
		//}
	}

	inline bool operator ==(const MaskedValue<T> & other) const {
		if (prefixMask != other.prefixMask)
			throw NonComparableErr();
		else
			return low() == other.low() && high() == other.high();
	}
};

template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
class FragmentedValue: public std::array<FRAGMENT_t, FRAGMENT_CNT> {
public:
	using mask_t = std::array<typename FRAGMENT_t::value_type, FRAGMENT_CNT>;
	using self_t = FragmentedValue<FRAGMENT_t, FRAGMENT_CNT>;

	FragmentedValue() {
	}
	FragmentedValue(std::array<FRAGMENT_t, FRAGMENT_CNT> a) :
			std::array<FRAGMENT_t, FRAGMENT_CNT>(a) {
	}
	FragmentedValue(std::array<FRAGMENT_t, FRAGMENT_CNT>& a) :
			std::array<FRAGMENT_t, FRAGMENT_CNT>(a) {
	}

	static constexpr size_t SIZE = FRAGMENT_CNT;

	// [TODO] is this copy really required?
	mask_t getMask() const {
		mask_t m;
		for (size_t i = 0; i < FRAGMENT_CNT; i++) {
			m[i] = (*this)(i);
		}
		return m;
	}

	template<typename op>
	constexpr bool chained_bin_rel(const FragmentedValue & other) const {
		bool res = true;
		for (size_t i = 0; i < FRAGMENT_CNT; i++) {
			// we have to check the rest of sequence because the item can be noncomparable
			const auto & a = (*this)[i];
			const auto & b = other[i];
			if (!(op()(a, b)))
				res = false;
		}
		return res;
	}

	constexpr bool operator <(const FragmentedValue & other) const {
		return chained_bin_rel<std::less<FRAGMENT_t>>(other);
	}

	constexpr bool operator <=(const FragmentedValue & other) const {
		return chained_bin_rel<std::less_equal<FRAGMENT_t>>(other);
	}

	constexpr bool operator ==(const FragmentedValue & other) const {
		return chained_bin_rel<std::equal_to<FRAGMENT_t>>(other);
	}
};

/// Primary class template hash.
//template<typename _Tp>
//struct hash_mask;
//
//template<typename FRAGMENT_t>
//struct hash_mask<MaskedValue<FRAGMENT_t>> {
//	typedef MaskedValue<FRAGMENT_t> argument_type;
//	typedef std::size_t result_type;
//	result_type operator()(argument_type const& v) const noexcept {
//		return std::hash<FRAGMENT_t> { }(v.vldMask);
//	}
//};

//template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
//struct hash_mask<FragmentedValue<FRAGMENT_t, FRAGMENT_CNT>> {
//	typedef FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> argument_type;
//	typedef std::size_t result_type;
//	result_type operator()(argument_type const& fv) const noexcept {
//		result_type h = 0;
//		for (const auto & v : fv) {
//			h = boost::hash_combine(v,
//					hash_mask<MaskedValue<FRAGMENT_t>> { }(v));
//		}
//		return h;
//	}
//};
//
//template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
//struct hash_mask_eq {
//	typedef FragmentedValue<FRAGMENT_t, FRAGMENT_CNT> argument_type;
//	bool operator()(const argument_type& l, const argument_type& r) const {
//		for (size_t i; i < l.size(); i++) {
//			if (!((*this)[i].vldMask != r[i].vldMask))
//				return false;
//		}
//		return true;
//	}
//};

// inject serialization methods to std namespace so other object like unordered_set can use it
// without an extra specification
namespace std {

template<typename FRAGMENT_t>
struct hash<MaskedValue<FRAGMENT_t>> {
	typedef MaskedValue<FRAGMENT_t> argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& v) const noexcept {
		result_type h = boost::hash_combine(std::hash<FRAGMENT_t> { }(v.value),
				std::hash<FRAGMENT_t> { }(v.prefixMask));
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

