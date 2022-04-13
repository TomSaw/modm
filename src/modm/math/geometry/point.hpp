/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "vector.hpp"

namespace modm
{

class Point : public Vector<int16_t, 2>
{
public:
	constexpr Point() = default;

	// Suspress "narrowing conversion" compiler warnings by defining some very tolerant constructors
	template<typename T, typename U>
	requires std::convertible_to<T, int16_t> && std::convertible_to<U, int16_t>
	constexpr Point(T x, U y)
		: Vector<int16_t, 2>(x, y)
	{}

	template<typename T>
	requires std::convertible_to<T, int16_t>
	constexpr Point(Vector<T, 2> vector)
		: Vector<int16_t, 2>(vector)
	{}

	/**
	 * @brief 			Constructors for Points placed 100%-relative to another Point / Size.
	 * 					Usefull to draw elements to relative Locations on Buffer or Display
	 * @param pct		x and y in 0-100% as float
	 * @param ref 		reference. usually you pass the Buffer or Display
	 */

	// float version is limited to compile time
	consteval Point(const Vector<float, 2> pct, const Point ref)
		: Point({
			std::round(ref.x() * pct.x() / 100.0),
			std::round(ref.y() * pct.y() / 100.0)
		})
	{}

	// runtime version takes an unsigned integral with factor 100
	Point(const Vector<uint16_t, 2> pct, const Point ref)
		: Point({
			std::round(ref.x() * pct.x() / 10000),
			std::round(ref.y() * pct.y() / 10000)
		})
	{}

	// This is usefull or overengineered
	// consteval Point(Vector<float, 2> pct, shape::Rectangle ref)
	// 	: Point(pct, ref.size)
	// {}

	// consteval Point(Vector<float, 2> pct, shape::Section ref)
	// 	: Point(pct, ref.getSize())
	// {}

	// consteval Point(Vector<float, 2> pct, graphic::Buffer ref)
	// 	: Point(pct, ref.getSize())
	// {}

	constexpr Point swapped() const {
		return {y(), x()};
	}

	void swap() {
		*this = swapped();
	}
};

}  // namespace modm::shape