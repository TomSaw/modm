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

#include <modm/math/geometry/shape/point.hpp>
#include <modm/math/geometry/shape/section.hpp>
#include <modm/ui/color/rgb_html.hpp>

namespace modm::graphic
{

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

/**
 * Baseclass for 2D Graphic Objects like Display and BufferMal
 *
 * @tparam	R	Resolution - R.width(): horizontal, R.height(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template <color::Color C, Size R>
class Canvas
{
public:	
	constexpr virtual Size
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