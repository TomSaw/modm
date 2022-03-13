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
#include <functional>

#include <modm/math/saturation/saturated.hpp>
#include <modm/math/proportional_unsigned.hpp>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/misc.hpp>

#include "concepts.hpp"

namespace modm::color {
/**
 * @brief 		Unsigned integer with arbitrary number of digits, proportional conversion
 * 				and saturating arithemtics.
 *
 * @tparam D 	Number of Digits
 * @tparam P	When forming a Buffer with this color, this would be the internal storage.
 * 				Should be MCUs fastest unsigned int -> just keep the default.
 * 				Some Displays require a specific type therefore it's a template argument.
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_color
 */
template <int D>
requires (D > 0)
class GrayD : public modm::ProportionalUnsigned<D>
{
	using ProportionalUnsigned<D>::ProportionalUnsigned;

public:
	// TODO any unsigned_integral can be PalleteType
	// depends on architecture and target Display
	using TPallete = uint8_t;

	using T = ProportionalUnsigned<D>::T;

	using modm::ProportionalUnsigned<D>::operator=;

	// @see: https://en.wikipedia.org/wiki/Grayscale#Converting_colour_to_grayscale
	// OPTIMIZE what's the best algorithm?
	// @see: https://github.com/modm-io/modm/pull/781#discussion_r818419586
  	template<ColorRgb C>
	constexpr GrayD(const C& rgb)
		: ProportionalUnsigned<D>(
			( 1742 * GrayD(rgb.red()).value()
			+ 5859 * GrayD(rgb.green()).value()
			+ 591 * GrayD(rgb.blue()).value()
		) >> 13)
	{}

	template<ColorRgbStacked C>
 	constexpr GrayD(const C& rgbstacked)
	 	: GrayD(RgbD<C::RedType::Digits, C::GreenType::Digits, C::BlueType::Digits>(rgbstacked))
	{}

	// However, using hsv.value() feels natural
	// Conversion from Hsv does not reflect human brightness perception like conversion from Rgb above
	// e.g. converting Hsv->Rgb->Gray will not produce the same result like Hsv->Gray
 	template<ColorHsv C>
	constexpr GrayD(const C& hsv) : ProportionalUnsigned<D>(hsv.value())
	{}

	// operator +=, -=, *=, /=
	template <std::integral I>
	GrayD&
	operator+=(I value) {
		modm::Saturated<T&> saturated(this->value_);
		saturated += value;
		// When using Ts complete range, std::min optimizes away
		this->value_ = std::min(this->value_, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator+=(const GrayD<I>& other)
	{ return this->operator+=(other.value_); }

	template <std::integral I>
	GrayD&
	operator-=(I value) {
		modm::Saturated<T&> saturated(this->value_);
		saturated -= value;
		// When using Ts complete range, std::min optimizes away
		this->value_ = std::min(this->value_, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator-=(const GrayD<I>& other)
	{ return this->operator-=(other.value_); }

	template<std::integral I>
	GrayD&
	operator*=(I value) {
		modm::Saturated<T&> saturated(this->value_);
		saturated *= value;
		// When using Ts complete range, std::min optimizes away
		this->value_ = std::min(this->value_, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator*=(const GrayD<I>& other)
	{ return this->operator*=(other.value_); }

	template<std::integral I>
	GrayD&
	operator/=(I value) {
		modm::Saturated<T&> saturated(this->value_);
		saturated /= value;
		// When using Ts complete range, std::min optimizes away
		this->value_ = std::min(this->value_, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator/=(const GrayD<I>& other)
	{ return this->operator*=(other.value_); }

	// operator +, -, *, /
	template<std::integral I>
	GrayD
	operator+(I value) const {
		modm::Saturated<T> saturated(this->value_);
		saturated += value;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.value(), this->max)};
	}

	GrayD
	operator+(const GrayD& other) const
	{ return this->operator+(other.value_); }

	template<std::integral I>
	GrayD
	operator-(I value) const {
		modm::Saturated<T> saturated(this->value_);
		saturated -= value;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.value(), this->max)};
	}

	GrayD
	operator-(const GrayD& other) const
	{ return this->operator-(other.value_); }

	template<std::integral I>
	GrayD
	operator*(I scale) const {
		modm::Saturated<I> saturated(this->value_);
		saturated *= scale;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.value(), this->max)};
	}

	GrayD
	operator*(const GrayD& other) const
	{ return this->operator*(other.value_); }

	template<std::integral I>
	GrayD
	operator/(I scale) const {
		modm::Saturated<I> saturated(this->value_);
		saturated /= scale;
	}

	GrayD
	operator/(const GrayD& other) const
	{ return this->operator/(other.value_); }

	template<std::floating_point F>
	GrayD
	operator*(F scale) const
	{
		// OPTIMIZE develop optimal decimals from D
		static constexpr int decimals = 10;

		using WideType = modm::WideType<T>;
		WideType saturated = this->value_ * T(scale * decimals) / decimals;
		return {T(std::min<WideType>(saturated, max))};
	}

	void
	invert() {
		this->value_ ^= bitmask<D>();
	}

	bool
	isSaturated() const
	{ return this->value_ == this->max; }

private:
	template<int>
	friend class GrayD;

	template <int E>
	friend modm::IOStream &
	operator<<(modm::IOStream &, const GrayD<E> &);
};

template<typename E>
using GrayT = GrayD<std::numeric_limits<E>::digits>;

using Monochrome = GrayD<1>;
using Gray2 = GrayD<2>;
using Gray4 = GrayD<4>;
using Gray8 = GrayT<uint8_t>;
using Gray16 = GrayT<uint16_t>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template <int E>
modm::IOStream &
operator<<(modm::IOStream &os, const GrayD<E> &color)
{
	os << color.value();
	return os;
}
#endif

}