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

#include "point.hpp"
// #include "shape/section.hpp"
namespace modm {
	/**
	 * @brief  	Redefinition of Point as Size adds typesafety
	 */

	// TODO Think of inheriting Vector<uint16_t, 2> instead of Point, using uint16_t instead of int16_t.
	// + Size components are always positive.
	// - Artihmetic pperations between Point and Size may have a drawback because of type conversion.
	// TODO Requires some simple benchmarks to figure out.
	// class Size : public Vector<uint16_t, 2>
	class Size : public modm::Vector<uint16_t, 2>
	{
	public:
		using modm::Vector<uint16_t, 2>::Vector;
		
		// TODO Possible to alias these methods (in a future version of C++)? ...
		// @see: https://en.cppreference.com/w/cpp/language/using_declaration
		// using width = &Point::x;
		// using height = &Point::y;

		// ... so they don't have to be redefined as follows.
		// It's typesafe but unfortunately results in some code duplication
		constexpr uint16_t width() const { return this->operator[](0); }
		constexpr uint16_t& width() { return this->operator[](0); }

		constexpr uint16_t height() const { return this->operator[](1); }
		constexpr uint16_t& height() { return this->operator[](1); }
		
		// Needs redefinition because return type has to be Size
		constexpr Size swapped() const {
			return {height(), width()};
		}

/* 		constexpr bool
		xIntersects(int16_t x) const
		{ return x >= 0 and x < width(); }

		constexpr bool
		yIntersects(int16_t y) const
		{ return y >= 0 and y < height(); }

		constexpr bool
		Intersects(Point point) const
		{ return xIntersects(point.x()) and yIntersects(point.y()); } */

/* 	 	constexpr shape::Section
		getIntersection(shape::Section section) {
			return {{
				std::clamp<int16_t>(section.topLeft.x(), 0, width()),
				std::clamp<int16_t>(section.topLeft.y(), 0, height())
			},
			{
				std::clamp<int16_t>(section.bottomRight.x(), 0, width()),
				std::clamp<int16_t>(section.bottomRight.y(), 0, height())
			}};
		} */
	};
}