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
#include <concepts>

#include "concepts.hpp"
#include "gray.hpp"

#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm::color
{
/**
 * @brief			Color in HSV space. Each channel has a memoryaddress on its own.
 *
 * @tparam DH 		Digits for hue
 * @tparam DS 		Digits for saturation
 * @tparam DV 		Digits for value
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DH, int DS = DH, int DV = DH>
requires (DH > 0) && (DS > 0) && (DV > 0)
class HsvD
{
public:
	using HueType = ProportionalUnsigned<DH>;
	using SaturationType = GrayD<DS>;
	using ValueType = GrayD<DV>;

	constexpr HsvD() = default;

	// TODO Support for https://en.cppreference.com/w/cpp/utility/initializer_list

	constexpr HsvD(HueType hue, SaturationType saturation, ValueType value)
		: hue_(hue), saturation_(saturation), value_(value)
	{}

	template<ColorHsv C>
	constexpr HsvD(const C &other)
		: hue_(other.hue_), saturation_(other.saturation_), value_(other.value_)
	{}

	template<ColorGray C>
	constexpr HsvD(const C &gray)
		: hue_(0), saturation_(0), value_(gray)
	{}

	template<ColorRgb C>
	constexpr HsvD(const C& rgb);

	template<ColorRgbStacked C>
 	constexpr HsvD(const C& rgbstacked)
	 	: HsvD(RgbD<C::RedType::Digits, C::GreenType::Digits, C::BlueType::Digits>(rgbstacked))
	{}

	// accesors
	const HueType hue() const { return hue_; }
	const SaturationType saturation() const { return saturation_; }
	const ValueType value() const { return value_; }

	HueType& hue() { return hue_; }
	SaturationType& saturation() { return saturation_; }
	ValueType& value() { return value_; }

	// TODO operator +=, -=, *=, /=
	// @see: https://gamedev.stackexchange.com/questions/26525/how-do-you-blend-multiple-colors-in-hsv-polar-color-space

	// TODO operator +, -, *, /

	// Equality
	bool
	operator==(const HsvD& other) const = default;

	bool
	operator>(const HsvD& other) const {
		return value_ > other.value();
	};

	bool
	operator<(const HsvD& other) const {
		return value_ < other.value();
	};

	bool
	operator>=(const HsvD& other) const {
		return value_ >= other.value();
	};

	bool
	operator<=(const HsvD& other) const {
		return value_ <= other.value();
	};

	void invert()
	{ hue_.invert(); }

private:
	HueType hue_{0};
	SaturationType saturation_{0};
	ValueType value_{0};

	template<int, int, int>
	friend class HsvD;

	template<ColorHsv C>
	friend IOStream&
	operator<<(IOStream&, const C&);

	// Human friendly output:
	// Hue in deg, Sat in pct, Value in pct
	template<ColorHsv C>
	void ostream_human_friendly(IOStream& os, const C&);
};

template<std::unsigned_integral T>
using HsvT = HsvD<std::numeric_limits<T>::digits>;

/// @ingroup modm_ui_color
using Hsv888 = HsvT<uint8_t>;
using Hsv161616 = HsvT<uint16_t>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorHsv C>
IOStream&
operator<<(IOStream& os, const C& hsv)
{
	os << hsv.hue() << "\t" << hsv.saturation() << "\t" << hsv.value();
	return os;
}

template<ColorHsv C>
void ostream_human_friendly(IOStream& os, const C& hsv) {
	using CalcTypeHue = modm::WideType<typename C::HueType::T>;
	using CalcTypeSaturation = modm::WideType<typename C::SaturationType::T>;
	using CalcTypeValue = modm::WideType<typename C::ValueType::T>;

	os << (CalcTypeHue(hsv.hue()) * 360 / hsv.hue().max) << "deg\t";
	os << (CalcTypeSaturation(hsv.saturation()) * 100 / hsv.saturation().max) << "%\t";
	os << (CalcTypeValue(hsv.value()) * 100 / hsv.value().max) << "%";
}
#endif

}  // namespace modm::color

#include "hsv_impl.hpp"
