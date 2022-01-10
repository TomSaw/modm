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

#include <modm/math/utils/integer_traits.hpp>
#include <modm/architecture/interface/assert.hpp>

namespace modm {

/**
 * @brief 		Unsigned integer with arbitrary digits and scaling value on conversion
 * 				between instances with different digits.
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
	static constexpr int Digits = D;

	using T = uint_t<D>::least;
	static constexpr T min = 0;
	static constexpr T max = bitmask<D>();

	constexpr ProportionalUnsigned() = default;

	constexpr ProportionalUnsigned(T value) {
		// TODO not sure if modm_assert_continue_fail_debug or modm_assert_continue_fail is needed
		modm_assert_continue_fail_debug(value <= max, "ProportionalUnsigned", "value out of range");
		value_ = value;
	}

	template <int E>
	requires (D <= E)
	constexpr ProportionalUnsigned(const ProportionalUnsigned<E>& other)
		: value_(other.value_ >> (E - D)) {}

	template <int E>
	requires (D <= E)
	constexpr ProportionalUnsigned(ProportionalUnsigned<E> &&other)
		: value_(other.value_ >> (E - D)) {}

	template <int E>
	requires (D > E)
	constexpr ProportionalUnsigned(const ProportionalUnsigned<E>& other)
		: value_(other.value_ * max / other.max)
	{}

	template <int E>
	requires (D > E)
	constexpr ProportionalUnsigned(ProportionalUnsigned<E> &&other)
		: value_(other.value_ * max / other.max)
	{}

	/* 	// Faster construction for D == 1
	constexpr ProportionalUnsigned(const ProportionalUnsigned<1> &other) : value_(other.value_ ? bitmask<D>() : 0){}

	// constexpr ProportionalUnsigned(ProportionalUnsigned<1> &&other) : value_(other.value_ ? bitmask<D>() : 0){}
	constexpr ProportionalUnsigned& operator=(const ProportionalUnsigned<1> &other) {
		value_ = other.value_ ? bitmask<D>() : 0;
		return *this;
	} */

	constexpr T value() const
	{ return value_; }

	// Cast to underlying type. No more comparison operators required.
	// @see https://en.cppreference.com/w/cpp/language/cast_operator
	constexpr operator T() const
	{ return value_; }

	void operator=(T value) {
		// TODO not sure if modm_assert_continue_fail_debug or modm_assert_continue_fail is needed
		modm_assert_continue_fail_debug(value_ <= max, "ProportionalUnsigned", "value out of range");
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