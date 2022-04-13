/*
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <concepts>
#include <modm/math/utils/integer_traits.hpp>
#include <modm/architecture/interface/assert.hpp>

namespace modm {

/**
 * @brief 		Unsigned integer with arbitrary number of digits and proportional conversion.
 * 				Artihmetic over- and underflows are being wrapped, just like builtin integral types.
 *
 * @tparam D 	Number of Digits
 *
 * @author		Thomas Sommer
 * @ingroup		modm_math
 */
template<int D>
requires (D > 0)
class ProportionalUnsigned {
public:
	static constexpr int digits = D;

	using T = uint_t<D>::least;
	static constexpr T bitmask = ::modm::bitmask<D>();

	static constexpr T min = 0;
	static constexpr T max = bitmask;

	constexpr ProportionalUnsigned() = default;

	constexpr ProportionalUnsigned(T value) {
		#ifdef MODM_DEBUG_BUILD
		if(not modm_assert_continue_ignore_debug(value <= max, "propun.ctor", "value exceeded max", max)) {
		 	value_ = std::min(max, value);
			return;
		}
		#endif

		value_ = value;
	}

	template <int E>
	requires (D <= E)
	constexpr ProportionalUnsigned(const ProportionalUnsigned<E>& other)
		: value_(other.value_ >> (E - D)) {}

	template <int E>
	requires (D > E)
	constexpr ProportionalUnsigned(const ProportionalUnsigned<E>& other)
		: value_(other.value_ * max / other.max)
	{}

	// Optimized for Monochrome
	// TODO May already be optimized: Benchmark or check asm
	template <int E>
	requires (E == 1)
	constexpr ProportionalUnsigned(const ProportionalUnsigned<1> &other) : value_(other.value_ & 1){}

	constexpr T value() const { return value_; }
	constexpr T& value() { return value_; }

	bool operator<=>(const ProportionalUnsigned& other) const = default;

	// operator +, -, *, /
	template<std::integral U>
	ProportionalUnsigned
	operator+(U value)
	{
		return {T(value_ + value) % (ProportionalUnsigned::max + 1)};
	}

	template<std::integral U>
	ProportionalUnsigned
	operator-(U value)
	{
		return {T(value_ - value) % (ProportionalUnsigned::max + 1)};
	}

	template<std::integral U>
	ProportionalUnsigned
	operator*(U value)
	{
		return {T(value_ * value) % (ProportionalUnsigned::max + 1)};
	}

	template<std::integral U>
	ProportionalUnsigned
	operator/(U value)
	{
		return {T(value_ / value) % (ProportionalUnsigned::max + 1)};
	}

	// operator +=, -=, *=, /=
	template<std::integral U>
	ProportionalUnsigned&
	operator+=(U value)
	{
		value_ = T(value_ + value) % (ProportionalUnsigned::max + 1);
		return *this;
	}

	template<std::integral U>
	ProportionalUnsigned&
	operator-=(U value)
	{
		value_ = T(value_ - value) % (ProportionalUnsigned::max + 1);
		return *this;
	}

	template<std::integral U>
	ProportionalUnsigned&
	operator*(U value)
	{
		value_ = T(value_ * value) % (ProportionalUnsigned::max + 1);
		return *this;
	}

	template<std::integral U>
	ProportionalUnsigned&
	operator/(U value)
	{
		value_ = T(value_ / value) % (ProportionalUnsigned::max + 1);
		return this;
	}

	void operator=(T value) {
		#ifdef MODM_DEBUG_BUILD
		if(not modm_assert_continue_ignore_debug(value <= max, "propun.assign", "value exceeded max", max)) {
		 	value_ = std::min(max, value);
			return;
		}
		#endif

		value_ = value;
	}

	template <int E>
	requires (D <= E)
	constexpr void operator=(const ProportionalUnsigned<E>& other) {
		value_ = other.value_ >> (E - D);
	}

	template <int E>
	requires (D > E)
	constexpr void operator=(const ProportionalUnsigned<E>& other) {
		value_ = other.value_ * max / other.max;
	}
protected:
	T value_{0};

private:
	template<int>
	friend class ProportionalUnsigned;
};

}