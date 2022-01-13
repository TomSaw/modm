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
	static_assert(D > 0, "Positive number of digits required for grayscale / colorchannel type.");
	using ProportionalUnsigned<D>::ProportionalUnsigned;

public:
	using T = ProportionalUnsigned<D>::T;

  	template<ColorRgb C>
	constexpr GrayD(const C& rgb)
		: ProportionalUnsigned<D>((
			2125 * (modm::WideType<T>)(GrayD(rgb.getRed())) +
			7154 * (modm::WideType<T>)(GrayD(rgb.getGreen())) +
			0721 * (modm::WideType<T>)(GrayD(rgb.getBlue()))
			) / 10000
		)
	{}

	template<ColorRgbStacked C>
 	constexpr GrayD(const C& rgbstacked)
	 	: GrayD(RgbD<C::RedType::Digits, C::GreenType::Digits, C::BlueType::Digits>(rgbstacked))
	{}


	// FIXME conversion from Hsv does not reflect brightness perception like conversion from Rgb above.
	// Converting Hsv->Rgb->Gray will not be the same like Hsv->Gray
	// It feels natural to use hsv.getValue() like it is for now
 	template<ColorHsv C>
	constexpr GrayD(const C& hsv) : ProportionalUnsigned<D>(hsv.getValue())
	{}

	// operator +=, -=, *=, /=
	template <std::integral I>
	GrayD&
	operator+=(I value) {
		modm::Saturated<T&> saturated(this->value);
		saturated += value;
		// When using Ts complete range, std::min optimizes away
		this->value = std::min(this->value, this->max);

		return *this;
	}

	template <int I>
 	GrayD& operator+=(const GrayD<I>& other)
	{ return this->operator+=(other.value); }

	template <std::integral I>
	GrayD&
	operator-=(I value) {
		modm::Saturated<T&> saturated(this->value);
		saturated -= value;
		// When using Ts complete range, std::min optimizes away
		this->value = std::min(this->value, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator-=(const GrayD<I>& other)
	{ return this->operator-=(other.value); }

	template<std::integral I>
	GrayD&
	operator*=(I value) {
		modm::Saturated<T&> saturated(this->value);
		saturated *= value;
		// When using Ts complete range, std::min optimizes away
		this->value = std::min(this->value, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator*=(const GrayD<I>& other)
	{ return this->operator*=(other.value); }

	template<std::integral I>
	GrayD&
	operator/=(I value) {
		modm::Saturated<T&> saturated(this->value);
		saturated /= value;
		// When using Ts complete range, std::min optimizes away
		this->value = std::min(this->value, this->max);

		return *this;
	}

	template <int I>
 	GrayD&
	operator/=(const GrayD<I>& other)
	{ return this->operator*=(other.value); }

	// operator +, -, *, /
	template<std::integral I>
	GrayD
	operator+(I value) const {
		modm::Saturated<T> saturated(this->value);
		saturated += value;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.getValue(), this->max)};
	}

	GrayD
	operator+(const GrayD& other) const
	{ return this->operator+(other.value); }

	template<std::integral I>
	GrayD
	operator-(I value) const {
		modm::Saturated<T> saturated(this->value);
		saturated -= value;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.getValue(), this->max)};
	}

	GrayD
	operator-(const GrayD& other) const
	{ return this->operator-(other.value); }

	template<std::integral I>
	GrayD
	operator*(I scale) const {
		modm::Saturated<I> saturated(this->value);
		saturated *= scale;

		// When using Ts complete range, std::min optimizes away
		return {std::min(saturated.getValue(), this->max)};
	}

	GrayD
	operator*(const GrayD& other) const
	{ return this->operator*(other.value); }

	template<std::integral I>
	GrayD
	operator/(I scale) const {
		modm::Saturated<I> saturated(this->value);
		saturated /= scale;
	}

	GrayD
	operator/(const GrayD& other) const
	{ return this->operator/(other.value); }

	template<std::floating_point F>
	GrayD
	operator*(F scale) const
	{
		// OPTIMIZE develop optimal decimals from D
		static constexpr int decimals = 10;

		using WideType = modm::WideType<T>;
		WideType saturated = this->value * T(scale * decimals) / decimals;
		return {T(std::min<WideType>(saturated, max))};
	}

	void invert() {
		this->value ^= bitmask<D>();
	}

	bool isSaturated() const
	{ return this->value == this->max; }

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
	os << color.value;
	return os;
}
#endif

}