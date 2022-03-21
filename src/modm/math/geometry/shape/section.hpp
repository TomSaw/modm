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
#include "size.hpp"

#include "rectangle.hpp"
#include "circle.hpp"

namespace modm::shape
{

// forward declarations for convertion constructors
class Rectangle;
class Circle;

class Section
{
public:
	Point topLeft, bottomRight;
	
	// TODO use Size
	Point size;

	constexpr Section() = default;

	constexpr Section(Point topLeft, Point bottomRight)
		: topLeft(topLeft), bottomRight(bottomRight), size({bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y()})
	{
		#ifdef MODM_DEBUG_BUILD
		modm_assert(bottomRight.x() >= topLeft.x(), "shape.section", "Width must be positive", bottomRight.x() - topLeft.x());
		modm_assert(bottomRight.y() >= topLeft.y(), "shape.section", "Height must be positive", bottomRight.y() - topLeft.y());
		#endif
	}

	constexpr Section(Size size)
		: topLeft({0, 0}), bottomRight(size), size(size)
	{}

	// Conversion constructors
	constexpr Section(Rectangle rectangle)
		: topLeft(rectangle.origin), bottomRight(rectangle.origin + rectangle.size), size(rectangle.size)
	{}

/* 	constexpr Section(Circle circle)
	{
		Point half_size(circle.radius, circle.radius);
		topLeft = circle.center - half_size;
		bottomRight = circle.center + half_size;
	} */

	constexpr Point
	getTopRight() const {
		return {bottomRight.x(), topLeft.y()};
	}

	constexpr Point
	getBottomLeft() const {
		return {topLeft.x(), bottomRight.y()};
	}

	constexpr Point
	getSize() const
	{ return size; }

	constexpr uint16_t
	getWidth() const
	{ return size.x(); }

	constexpr uint16_t
	getHeight() const
	{ return size.y(); }

	constexpr uint32_t
	getPixels() const
	{ return size.x() * size.y(); }
};
}  // namespace modm::shape