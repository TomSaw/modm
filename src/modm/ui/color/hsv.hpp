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

	constexpr HsvD(HueType hue, SaturationType saturation, ValueType value)
		: hue(hue), saturation(saturation), value(value)
	{}

	template<ColorHsv C>
	constexpr HsvD(const C &other)
		: hue(other.hue), saturation(other.saturation), value(other.value)
	{}

	template<ColorGray C>
	constexpr HsvD(const C &gray)
		: hue(0), saturation(0), value(gray)
	{}

	template<ColorRgb C>
	constexpr HsvD(const C& rgb);

	template<ColorRgbStacked C>
 	constexpr HsvD(const C& rgbstacked)
	 	: HsvD(RgbD<C::RedType::Digits, C::GreenType::Digits, C::BlueType::Digits>(rgbstacked))
	{}

	// getters and setters
	const HueType getHue() const { return hue; }
	const SaturationType getSaturation() const { return saturation; }
	const ValueType getValue() const { return value; }

	HueType& getHue() { return hue; }
	SaturationType& getSaturation() { return saturation; }
	ValueType& getValue() { return value; }

	void setHue(HueType hue) { this->hue = hue;}
	void setSaturation(SaturationType saturation) { this->saturation = saturation;}
	void setValue(ValueType value) { this->value = value;}

	// TODO operator +=, -=, *=, /=
	// @see: https://gamedev.stackexchange.com/questions/26525/how-do-you-blend-multiple-colors-in-hsv-polar-color-space

	// TODO operator +, -, *, /

	// Equality
	bool
	operator==(const HsvD& other) const = default;

	bool
	operator>(const HsvD& other) const {
		return value >= other.value;
	};

	bool
	operator<(const HsvD& other) const {
		return value >= other.value;
	};

	bool
	operator>=(const HsvD& other) const {
		return value >= other.value;
	};

	bool
	operator<=(const HsvD& other) const {
		return value <= other.value;
	};

	void invert()
	{ hue.invert(); }

private:
	HueType hue{0};
	SaturationType saturation{0};
	ValueType value{0};

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
	os << hsv.getHue() << "\t" << hsv.getSaturation() << "\t" << hsv.getValue();
	return os;
}

template<ColorHsv C>
void ostream_human_friendly(IOStream& os, const C& hsv) {
	using CalcTypeHue = modm::WideType<typename C::HueType::T>;
	using CalcTypeSaturation = modm::WideType<typename C::SaturationType::T>;
	using CalcTypeValue = modm::WideType<typename C::ValueType::T>;

	os << (CalcTypeHue(hsv.getHue()) * 360 / hsv.getHue().max) << "deg\t";
	os << (CalcTypeSaturation(hsv.getSaturation()) * 100 / hsv.getSaturation().max) << "%\t";
	os << (CalcTypeValue(hsv.getValue()) * 100 / hsv.getValue().max) << "%";
}
#endif

}  // namespace modm::color

#include "hsv_impl.hpp"
