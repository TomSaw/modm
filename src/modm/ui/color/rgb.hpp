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

#include <stdint.h>
#include <bit>

#include <concepts>
#include <modm/io/iostream.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>
#include <type_traits>

#include "concepts.hpp"
#include "gray.hpp"

namespace modm::color
{
/**
 * @brief 			Color in RGB space. Each channel has a memoryaddress on its own.
 *
 * @tparam DR 		Digits for red channel
 * @tparam DG 		Digits for green channel
 * @tparam DB 		Digits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DR, int DG = DR, int DB = DR>
requires (DR > 0) && (DG > 0) && (DB > 0)
class RgbD
{
public:
	using RedType = GrayD<DR>;
	using GreenType = GrayD<DG>;
	using BlueType = GrayD<DB>;

	// using RgbSumValueType = modm::fits_any_t<RedType::T, GreenType::T, BlueType::T>;

	constexpr RgbD() = default;

	constexpr RgbD(RedType red, GreenType green, BlueType blue)
		: red_(red), green_(green), blue_(blue)
	{}

	template<class C>
	requires ColorRgb<C> || ColorRgbStacked<C>
	constexpr RgbD(const C& other)
		: red_(other.red()), green_(other.green()), blue_(other.blue())
	{}

	template<ColorGray C>
	constexpr RgbD(const C &gray)
		: red_(gray), green_(gray), blue_(gray)
	{}

	template<ColorHsv C>
	constexpr RgbD(const C& hsv);

	// accessors
	const RedType red() const { return red_; }
	const GreenType green() const { return green_; }
	const BlueType blue() const { return blue_; }

	RedType& red() { return red_; }
	GreenType& green() { return green_; }
	BlueType& blue() { return blue_; }

	// operator +=, -=, *=, /=
	RgbD& operator+=(const RgbD& other) {
		red_ += other.red();
		green_ += other.green();
		blue_ += other.blue();
		return *this;
	}

	RgbD& operator-=(const RgbD& other) {
		red_ -= other.red();
		green_ -= other.green();
		blue_ -= other.blue();
		return *this;
	}

	RgbD& operator*=(const RgbD& other) {
		red_ *= other.red();
		green_ *= other.green();
		blue_ *= other.blue();
		return *this;
	}

	RgbD& operator/=(const RgbD& other) {
		red_ /= other.red();
		green_ /= other.green();
		blue_ /= other.blue();
		return *this;
	}

	// operator +, -, *, /
	RgbD operator+(const RgbD& rgb) {
		return {
			red_ + rgb.red(),
			green_ + rgb.green(),
			blue_ + rgb.blue()
		};
	}

	RgbD operator-(const RgbD& rgb) {
		return {
			red_ - rgb.red(),
			green_ - rgb.green(),
			blue_ - rgb.blue()
		};
	}

	RgbD operator*(const RgbD& rgb) {
		return {
			red_ * rgb.red(),
			green_ * rgb.green(),
			blue_ * rgb.blue()
		};
	}

	RgbD operator/(const RgbD& rgb) {
		return {
			red_ / rgb.red(),
			green_ / rgb.green(),
			blue_ / rgb.blue()
		};
	}

	// Equality
	constexpr bool
	operator==(const RgbD& other) const = default;

	// Comparison on perceived brightness. For simplicity, the intermediate brightnes tyoe
	// is hardcoded to Gray8, This may be improved.
	constexpr bool
	operator>(const Gray8& gray) const {
		return Gray8(*this) > gray;
	};

	constexpr bool
	operator<(const Gray8& gray) const {
		return Gray8(*this) < gray;
	};

	constexpr bool
	operator>=(const Gray8& gray) const {
		return Gray8(*this) >= gray;
	};

	constexpr bool
	operator<=(const Gray8& gray) const {
		return Gray8(*this) <= gray;
	};

	void invert() {
		red_.invert();
		green_.invert();
		blue_.invert();
	}

private:
	RedType red_{0};
	GreenType green_{0};
	BlueType blue_{0};

	template<int, int, int>
	friend class RgbD;

	template<ColorRgb C>
	friend IOStream&
	operator<<(IOStream&, const C&);
};

template<std::unsigned_integral U>
using RgbT = RgbD<std::numeric_limits<U>::digits>;

/// @ingroup modm_ui_color
using Rgb888 = RgbT<uint8_t>;
using Rgb161616 = RgbT<uint16_t>;

/**
 * Normalize color values based on a clear value
 *
 * Imagine a low band light, for example a green laser. In case the filters
 * of a sensor do not transfer this wavelength well, it might result in all
 * colors being very low. An aditional clear value (provided f.e. by TCS3472)
 * will not filter colors and thus it will see a bright light (intensity).
 * In order to still have some signal the very low green value can be
 * amplified with the clear value.
 *
 * @ingroup modm_ui_color
 */
template<ColorRgb C, typename IntermediateType = float, ColorRgb ReturnColor = C>
	requires std::is_fundamental_v<IntermediateType>
constexpr ReturnColor
normalizeColor(C rgb, IntermediateType multiplier = 1)
{
	const IntermediateType sum = IntermediateType(rgb.red()) + rgb.green() + rgb.blue();
	return {
		IntermediateType(rgb.red()) * multiplier / sum,
		IntermediateType(rgb.green()) * multiplier / sum,
		IntermediateType(rgb.blue()) * multiplier / sum
	};
}

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgb C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.red() << "\t" << rgb.green() << "\t" << rgb.blue();
	return os;
}
#endif

}  // namespace modm::color

#include "rgb_impl.hpp"