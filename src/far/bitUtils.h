#pragma once

#include <limits>

/*
 * Container of common bit operations for specified type T
 **/
template<typename T>
class BitUtils {
public:
	static constexpr size_t BIT_LEN = sizeof(T) * 8;
	static constexpr size_t ALL_MASK = std::numeric_limits<T>::max();

	/*
	 * Get prefix prefix mask by prefix length
	 * */
	static constexpr T maskPrefix(size_t len) {
		if (len == BIT_LEN) {
			// can not shift too much because value would overflow
			return T(0) - T(1);
		} else {
			// get mask and shift it to msb area
			return ((T(1) << len) - 1) << (BIT_LEN - len);
		}
	}

	static constexpr T mask(size_t len) {
		// can not shift too much because value would overflow
		return (T(0) - T(1)) >> (BIT_LEN - len);
	}

	static constexpr T selectBitRange(T val, size_t bitsStart, size_t bitsLen) {
		val >>= bitsStart;
		return val & mask(bitsLen);
	}

	static constexpr T setBitRange(T val, size_t bitStart, size_t bitsLen,
			T newBits) {
		T _mask = mask(bitsLen);
		newBits &= _mask;

		_mask <<= bitStart;
		newBits <<= bitStart;

		return (val & ~_mask) | newBits;
	}

	static constexpr size_t countSetBits(T val) {
		return __builtin_popcount(val);
	}
};
