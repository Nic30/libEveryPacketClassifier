#pragma once

#include <array>
#include <assert.h>
#include <initializer_list>
#include <functional>

#include <boost/functional/hash.hpp>
#include "exceptions.h"
#include "bitUtils.h"

using priority_t = int;

/*
 * Value with validity mask
 *
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
	static constexpr size_t BIT_LEN = BitUtils<T>::BIT_LEN;
	static constexpr size_t ALL_MASK = BitUtils<T>::ALL_MASK;

	T _M_low; // or low if this is a range
	T _M_high;

public:
	T prefix_mask;
	size_t prefix_len;
	priority_t priority;
	bool is_range;

	constexpr T & low() {
		return _M_low;
	}

	constexpr T & high() {
		return _M_high;
	}

	constexpr const T & low() const {
		return _M_low;
	}

	constexpr const T & high() const {
		return _M_high;
	}

	static MaskedValue from_range(T low, T high, priority_t priority = -1) {
		assert(low <= high);
		size_t i;
		for (i = 0; i < BIT_LEN; i++) {
			auto m = T(1) << i;
			if ((low & m) != (high & m)) {
				// common prefix ends there because bits are different
				break;
			}
		}
		MaskedValue self(low, i, priority);
		self.is_range = true;
		// high will not fit exactly if range is not aligned
		self.high() = high;

		return self;
	}

	static constexpr T getRange(size_t prefix_len) {
		return (T(1) << (BIT_LEN - prefix_len));
	}

	MaskedValue(T prefix, size_t prefix_len = BIT_LEN, priority_t priority = -1) :
			_M_low(prefix), _M_high(prefix + getRange(prefix_len) - 1), prefix_mask(
					BitUtils<T>::maskPrefix(prefix_len)), prefix_len(
					prefix_len), priority(priority), is_range(false) {
		assert(prefix_len <= BIT_LEN);
	}

	/*
	 * :return: true if this value is subinterval in other value
	 **/
	constexpr bool isContainedIn(const MaskedValue & other) const {
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
	constexpr bool isOverlapping(const MaskedValue & other) const {
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

	inline bool operator <(const MaskedValue & other) const {
		if (isOverlapping(other)) {
			if ((prefix_len == BIT_LEN && other.prefix_len == BIT_LEN)
					&& (*this == other)) {
				return true;
			}
			throw NonComparableErr();
		}
		return high() < other.low();
	}
	inline bool operator <=(const MaskedValue & other) const {
		if (isOverlapping(other)) {
			if (*this == other)
				return true;
			throw NonComparableErr();
		}
		return high() <= other.low();
	}

	inline bool operator ==(const MaskedValue & other) const {
		if (prefix_mask != other.prefix_mask)
			throw NonComparableErr();
		else
			return low() == other.low() && high() == other.high();
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const MaskedValue & t) {
		if (t.is_range) {
			str << t.low() << "-" << t.high();
		} else {
			str << t.low() << "/" << t.prefix_len;
		}
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};

/*
 * Value composed of multiple parts
 * Part with with index=0 has lower priority while comparing
 * */
template<typename FRAGMENT_t, size_t FRAGMENT_CNT>
class FragmentedValue: public std::array<FRAGMENT_t, FRAGMENT_CNT> {
public:
	using mask_t = std::array<typename FRAGMENT_t::value_type, FRAGMENT_CNT>;
	using self_t = FragmentedValue<FRAGMENT_t, FRAGMENT_CNT>;

	FragmentedValue(std::array<FRAGMENT_t, FRAGMENT_CNT> a) :
			std::array<FRAGMENT_t, FRAGMENT_CNT>(a) {
	}
	//FragmentedValue(std::array<FRAGMENT_t, FRAGMENT_CNT>& a) :
	//		std::array<FRAGMENT_t, FRAGMENT_CNT>(a) {
	//}

	static constexpr size_t SIZE = FRAGMENT_CNT;

	// [TODO] is this copy really required?
	mask_t getMask() const {
		mask_t m;
		for (size_t i = 0; i < FRAGMENT_CNT; i++) {
			const auto & tmp = (*this)[i].prefix_mask;
			m[i] = tmp;
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
		for (size_t i = 0; i < FRAGMENT_CNT; i++) {
			size_t _i = FRAGMENT_CNT - i - 1;
			// we have to check the rest of sequence because the item can be noncomparable
			const auto & a = (*this)[_i];
			const auto & b = other[_i];
			if (a < b) {
				return true;
			} else if (a == b) {
				continue;
			} else {
				return false;
			}
		}
		return false;
	}

	constexpr bool operator <=(const FragmentedValue & other) const {
		for (size_t i = 0; i < FRAGMENT_CNT; i++) {
			size_t _i = FRAGMENT_CNT - i - 1;
			// we have to check the rest of sequence because the item can be noncomparable
			const auto & a = (*this)[_i];
			const auto & b = other[_i];
			if (a <= b) {
				if (b <= a)
					continue;
				return true;
			} else {
				return false;
			}
		}
		return true;
	}

	constexpr bool operator ==(const FragmentedValue & other) const {
		return chained_bin_rel<std::equal_to<FRAGMENT_t>>(other);
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const FragmentedValue & t) {
		str << "<";
		for (auto const & item : t) {
			str << item << ", ";
		}
		str << ">";
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
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
				std::hash<FRAGMENT_t> { }(v.prefix_mask));
		h = boost::hash_combine(h, std::hash<priority_t>()(v.priority));
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

