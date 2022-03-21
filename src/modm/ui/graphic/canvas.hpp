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

#include <modm/math/geometry/shape/point.hpp>
#include <modm/math/geometry/shape/section.hpp>
#include <modm/ui/color/rgb_html.hpp>

namespace modm::graphic
{

/**
 * Baseclass for 2D Graphic Objects like Display and BufferMal
 *
 * @tparam	R	Resolution - R.width(): horizontal, R.height(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template <color::Color C, shape::Size R>
class Canvas
{
public:	
	constexpr virtual shape::Size
	getSize() const
	{ return R; }

	constexpr virtual int16_t
	getWidth() const
	{ return R.width(); }

	constexpr virtual int16_t
	getHeight() const
	{ return R.height(); }

	constexpr shape::Section
	asSection() const
	{ return {{0, 0}, getSize()}; }

 	constexpr shape::Section
	getIntersection(shape::Section section) {
		return {{
			std::clamp<int16_t>(section.topLeft.x(), 0, getWidth()),
			std::clamp<int16_t>(section.topLeft.y(), 0, getHeight())
		},
		{
			std::clamp<int16_t>(section.bottomRight.x(), 0, getWidth()),
			std::clamp<int16_t>(section.bottomRight.y(), 0, getHeight())
		}};
	}

	// TODO The colormap API has absolutely no protection bad color_idx or nullptr
	void
	setColormap(C* colormap)
	{ this->colormap = colormap; }

	void
	setColor(std::size_t color_idx)
	{ this->color = colormap[color_idx]; }

	void
	setColor(C color)
	{ this->color = color; }

	constexpr C getColor() const
	{ return this->color; }

protected:
	C* colormap{nullptr};
	C color{color::html::White};

	static constexpr shape::Point
	getSourceOrigin(shape::Point placement) {
		return {
			placement.x() < 0 ? -placement.x() : 0,
			placement.y() < 0 ? -placement.y() : 0
		};
	}

	// TODO reuse Intersection builtin geometry::shape
	constexpr bool
	xIntersects(int16_t x) const
	{ return x >= 0 and x < int16_t(getWidth()); }

	constexpr bool
	yIntersects(int16_t y) const
	{ return y >= 0 and y < int16_t(getHeight()); }

	constexpr bool
	pointIntersects(shape::Point position) const
	{ return xIntersects(position.x()) and yIntersects(position.y()); }
};
}  // namespace modm::graphic