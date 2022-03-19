#pragma once
#include <concepts>
#include <limits>

namespace modm::math {

	template <std::unsigned_integral T, int DigitsElement>
	class Pallete {
	public:
		static constexpr T bitmask = std::numeric_limits<T>::max();
		static constexpr int digits = std::numeric_limits<T>::digits;
		static constexpr int elements = digits / DigitsElement;

		static std::size_t
		getOffset(int16_t y)
		{ return y / elements; }

		static int
		getLShift(int16_t y)
		{ return (y * DigitsElement) & (digits - 1); } // x & (digits - 1) coresponds x % digits with support for negative int
		// #############################################################
	};
}