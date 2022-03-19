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

namespace modm::shape {
	/**
	 * @brief  	Redefinition of Point as Size adds typesafety
	 */

	// TODO Think of inheriting Vector<uint16_t, 2> instead of Point, using uint16_t instead of int16_t.
	// + Size components are always positive.
	// - Artihmetic pperations between Point and Size may have a drawback because of type conversion.
	// TODO Requires some simple benchmarks to figure out.
	// class Size : public Vector<uint16_t, 2>
	class Size : public shape::Point
	{
	public:
		using shape::Point::Point;
		
		// TODO Possible to alias these methods (in a future version of C++)? ...
		// @see: https://en.cppreference.com/w/cpp/language/using_declaration
		// using width = &shape::Point::x;
		// using height = &shape::Point::y;

		// ... so they don't have to be redefined as follows.
		// It's typesafe but unfortunately results in some code duplication
		constexpr int16_t width() const { return this->operator[](0); }
		constexpr int16_t& width() { return this->operator[](0); }

		constexpr int16_t height() const { return this->operator[](1); }
		constexpr int16_t& height() { return this->operator[](1); }
		
		// Needs redefinition because return type has to be Size
		constexpr Size swapped() const {
			return {height(), width()};
		}
	};
}