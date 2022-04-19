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

#include <modm/math/geometry/point.hpp>
#include <modm/graphic/color.hpp>

namespace modm::graphic::pattern {

	/**
	 * @brief 	Concept for lambda / functor that emits a color::Color for any Point
	 *
	 */
	template<class CP>
	concept ColorPattern = requires(CP cp)
	{
		{ cp(Point()) } -> color::Color;
	};

	/// Little basic pattern collection
	
	constexpr int width = 16;
	using namespace color::html;

	constexpr auto stripes_horizontal = [](Point p) { return p.x() % (2 * width) < width ? Yellow : Black; };
	constexpr auto stripes_vertical = [](Point p) { return p.y() % (2 * width) < width ? Yellow : Black; };
	constexpr auto stripes_diag_left = [](Point p) { return (p.x() + p.y()) % (2 * width) < width ? Yellow : Black; };
	constexpr auto stripes_diag_right = [](Point p) { return (p.x() - p.y()) % (2 * width) < width ? Yellow : Black; };

	constexpr auto gradient_hsv = [](Point p) { return color::Rgb888(color::Hsv888(p.x(), p.y(), 255)); };

	constexpr auto gradient_red = [](Point p) { return color::Rgb888(p.x() + p.y() / width, 30, 30); };
	constexpr auto gradient_green = [](Point p) { return color::Rgb888(30, p.x() + p.y() / width, 30); };
	constexpr auto gradient_blue = [](Point p) { return color::Rgb888(30, 30, p.x() + p.y() / width); };

	constexpr auto noize = [](Point) { return color::Rgb888(rand(), rand(), rand()); };
	constexpr auto white_noize = [](Point) { return color::Gray<8>(rand()); };
}