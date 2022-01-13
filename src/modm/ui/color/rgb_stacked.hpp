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

#include "concepts.hpp"

namespace modm::color {
/**
 * @brief 			Color in RGB space - mandatory for most graphic displays and stored images.
 * 					Requires less space than unstacked rgb types. Calculations without dedicated graphics
 * 					acceleration hardware are very inefficient.
 * 					If theres no dedicated graphics acceleration but lots of RAM, using modm::RgbD<> for
 * 					calculations and afterwards conversion to modm::RgbStackedD<> may be an option.
 *
 * @tparam DR 		Digits for red channel
 * @tparam DG 		Digits for green channel
 * @tparam DB 		Digits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template <int DR, int DG, int DB>
requires (DR > 0) && (DG > 0) && (DB > 0)
class RgbStackedD
{
public:
	using RedType = GrayD<DR>;
	using GreenType = GrayD<DG>;
	using BlueType = GrayD<DB>;

	using T = uint_t<DR + DG + DB>::least;

	constexpr RgbStackedD() = default;

	constexpr RgbStackedD(const T value) : value(value){};

	constexpr RgbStackedD(RedType red, GreenType green, BlueType blue)
		: value(red << (DG + DB) | green << DB | blue) {}

	template<ColorRgbStacked C>
	constexpr RgbStackedD(const C &other)
		: RgbStackedD(other.getRed(), other.getGreen(), other.getBlue()) {}

	template<ColorGray C>
	constexpr RgbStackedD(const C &gray)
		: RgbStackedD(RgbD<C::Digits>(gray)) {}

	template<ColorRgb C>
	constexpr RgbStackedD(const C &rgb)
		: RgbStackedD(rgb.getRed(), rgb.getGreen(), rgb.getBlue()) {}

	template<ColorHsv C>
	constexpr RgbStackedD(const C &hsv)
		: RgbStackedD(RgbD<5,6,5>(hsv)) {}

	// getters and setters
	const RedType getRed() const { return value >> (DG + DB); }
	const GreenType getGreen() const { return value >> DB & GreenType::max;}
	const BlueType getBlue() const { return value & BlueType::max; }

	void setRed(RedType red) { value = (value & ~(bitmask<DR>() << (DG + DB))) | red << (DG + DB); }
	void setGreen(GreenType green) { value = (value & ~(bitmask<DG>() << DB)) | green << DB; }
	void setBlue(BlueType blue) { value = (value & ~bitmask<DB>()) | blue; }

	T getValue() const { return value; }

	RgbStackedD& operator+=(const RgbD<DR, DG, DB>& rgb) {
		setRgb(
			getRed() + rgb.getRed(),
			getGreen() + rgb.getGreen(),
			getBlue() + rgb.getBlue()
		);
		return *this;
	}

	RgbStackedD& operator-=(const RgbD<DR, DG, DB>& rgb) {
		setRgb(
			getRed() - rgb.getRed(),
			getGreen() - rgb.getGreen(),
			getBlue() - rgb.getBlue()
		);
		return *this;
	}

	RgbStackedD& operator*=(const RgbD<DR, DG, DB>& rgb) {
		setRgb(
			getRed() * rgb.getRed(),
			getGreen() * rgb.getGreen(),
			getBlue() * rgb.getBlue()
		);
		return *this;
	}

	RgbStackedD& operator/=(const RgbD<DR, DG, DB>& rgb) {
		setRgb(
			getRed() * rgb.getRed(),
			getGreen() * rgb.getGreen(),
			getBlue() * rgb.getBlue()
		);
		return *this;
	}

	RgbStackedD operator+(const RgbD<DR, DG, DB>& rgb) {
		return {
			getRed() + rgb.getRed(),
			getGreen() + rgb.getGreen(),
			getBlue() + rgb.getBlue()
		};
	}

	RgbStackedD operator-(const RgbD<DR, DG, DB>& rgb) {
		return {
			getRed() - rgb.getRed(),
			getGreen() - rgb.getGreen(),
			getBlue() - rgb.getBlue()
		};
	}

	RgbStackedD operator*(const RgbD<DR, DG, DB>& rgb) {
		return {
			getRed() * rgb.getRed(),
			getGreen() * rgb.getGreen(),
			getBlue() * rgb.getBlue()
		};
	}

	RgbStackedD operator/(const RgbD<DR, DG, DB>& rgb) {
		return {
			getRed() / rgb.getRed(),
			getGreen() / rgb.getGreen(),
			getBlue() / rgb.getBlue()
		};
	}

	// Equality
	constexpr bool
	operator==(const RgbStackedD& other) const = default;

	// Remaining comparison operators are server by color::RgbD<D> and implicit type conversion

	void invert() {
		value ^= bitmask<DR + DG + DB>();
	}

private:
	T value;

	void setRgb(RedType red, GreenType green, BlueType blue)
	{ value = red << (DG + DB) | green << DB | blue; }

	template<ColorRgbStacked C>
	friend IOStream&
	operator<<(IOStream&, const C&);
};

using Rgb565 = RgbStackedD<5,6,5>;
using Rgb666 = RgbStackedD<6,6,6>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgbStacked C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.getRed() << "\t" << rgb.getGreen() << "\t" << rgb.getBlue();
	return os;
}
#endif

}