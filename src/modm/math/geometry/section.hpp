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

namespace modm
{

class Section
{
public:
	Point topLeft, bottomRight;
	Size size;

	constexpr Section() = default;

	constexpr Section(Point topLeft, Point bottomRight)
		: topLeft(topLeft), bottomRight(bottomRight), size({bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y()})
	{
		#ifdef MODM_DEBUG_BUILD
		modm_assert(bottomRight.x() > topLeft.x(), "shape.section", "Negative width is forbidden", bottomRight.x() - topLeft.x());
		modm_assert(bottomRight.y() > topLeft.y(), "shape.section", "Negative height is forbidden", bottomRight.y() - topLeft.y());
		#endif
	}

	constexpr Point
	getTopRight() const {
		return {bottomRight.x(), topLeft.y()};
	}

	constexpr Point
	getBottomLeft() const {
		return {topLeft.x(), bottomRight.y()};
	}

	constexpr Size
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