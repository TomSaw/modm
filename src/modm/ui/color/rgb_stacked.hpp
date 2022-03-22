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
 * 					If theres no dedicated graphics acceleration but lots of RAM, using modm::Rgb<> for
 * 					calculations and afterwards conversion to modm::RgbStacked<> may be an option.
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
class RgbStacked
{
public:
	using RedType = Gray<DR>;
	using GreenType = Gray<DG>;
	using BlueType = Gray<DB>;

	using T = uint_t<DR + DG + DB>::least;

	constexpr RgbStacked() = default;

	constexpr RgbStacked(T value)
		: value_(value)
	{}

	constexpr RgbStacked(RedType red, GreenType green, BlueType blue)
		: value_(red.value() << (DG + DB) | green.value() << DB | blue.value())
	{}

	constexpr RgbStacked(const RgbStacked &rgbstacked)
		: value_(rgbstacked.value_)
	{}

	template<ColorGray C>
	constexpr RgbStacked(const C &gray)
		: RgbStacked(Rgb<C::Digits>(gray))
	{}

	template<ColorRgb C>
	constexpr RgbStacked(const C &rgb)
		: RgbStacked(rgb.red(), rgb.green(), rgb.blue())
	{}

	template<ColorRgbStacked C>
	constexpr RgbStacked(const C &rgstacked)
		: RgbStacked(rgstacked.red(), rgstacked.green(), rgstacked.blue())
	{}

	template<ColorHsv C>
	constexpr RgbStacked(const C &hsv)
		: RgbStacked(Rgb<5,6,5>(hsv))
	{}

	/**
	 * @brief 			Interface to channels of stacked color types
	 * 					May be used for StackedHsv as well
	 *
	 * @tparam TS 		unsigned integral type of stacked color value
	 * @tparam TC 		Gray<> type of channel
	 * @tparam Shift 	lshift of channel in stacked color value
	 *
	 * @ingroup			modm_ui_color
	 */
	template<std::unsigned_integral TS, ColorGray TC, int Shift>
	struct channel_accessor {
		TS &value_;

		void
		operator= (const TC new_value)
		{ value_ = (value_ & ~(TC::max << Shift)) | new_value.value() << Shift; }

		const TC value() const { return (value_ >> Shift) & TC::max; }
	};

	// accessors
	const T value() const { return value_; }

	const RedType red() const { return value_ >> (DG + DB); }
	const GreenType green() const { return value_ >> DB & GreenType::max;}
	const BlueType blue() const { return value_ & BlueType::max; }

	auto red() { return channel_accessor<T, RedType, DG + DB>{value_}; }
	auto green() { return channel_accessor<T, GreenType, DB>{value_}; }
	auto blue() { return channel_accessor<T, BlueType, 0>{value_}; }

	// assignment
	void operator=(const RgbStacked other) {
		value_ = other.value_;
	}

	RgbStacked& operator+=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red().value() + rgb.red(),
			green().value() + rgb.green(),
			blue().value() + rgb.blue()
		});
		return *this;
	}

	RgbStacked& operator-=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red().value() - rgb.red(),
			green().value() - rgb.green(),
			blue().value() - rgb.blue()
		});
		return *this;
	}

	RgbStacked& operator*=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red().value() * rgb.red(),
			green().value() * rgb.green(),
			blue().value() * rgb.blue()
		});
		return *this;
	}

	RgbStacked& operator/=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red().value() * rgb.red(),
			green().value() * rgb.green(),
			blue().value() * rgb.blue()
		});
		return *this;
	}

	RgbStacked operator+(const Rgb<DR, DG, DB>& rgb) {
		return {
			red().value() + rgb.red(),
			green().value() + rgb.green(),
			blue().value() + rgb.blue()
		};
	}

	RgbStacked operator-(const Rgb<DR, DG, DB>& rgb) {
		return {
			red().value() - rgb.red(),
			green().value() - rgb.green(),
			blue().value() - rgb.blue()
		};
	}

	RgbStacked operator*(const Rgb<DR, DG, DB>& rgb) {
		return {
			red().value() * rgb.red(),
			green().value() * rgb.green(),
			blue().value() * rgb.blue()
		};
	}

	RgbStacked operator/(const Rgb<DR, DG, DB>& rgb) {
		return {
			red().value() / rgb.red(),
			green().value() / rgb.green(),
			blue().value() / rgb.blue()
		};
	}

	// Equality
	constexpr bool
	operator==(const RgbStacked& other) const = default;

	// Remaining comparison operators are server by color::Rgb<D> and implicit type conversion

	void invert() {
		value_ ^= bitmask<DR + DG + DB>();
	}

private:
	T value_;
};

using Rgb565 = RgbStacked<5,6,5>;
using Rgb666 = RgbStacked<6,6,6>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgbStacked C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.red() << "\t" << rgb.green() << "\t" << rgb.blue();
	return os;
}
#endif

}