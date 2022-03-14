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

#include <algorithm>

#include <modm/math/geometry/shape.hpp>

#include "concepts.hpp"
#include "buffer.hpp"
#include "style.hpp"

namespace modm::graphic
{
/**
 * @brief		Painter for BufferMal stored in local RAM
 *				Can draw fundamental shapes: shape::Point, Line, Rectangle, Circle, Ellipse
 *				Can write other buffers from RAM and images from FLASH.
 *
 * @tparam	GB	GraphicBuffer in RAM
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 *
 * @see			graphic::RemotePainter (same with Resumable Functions)
 */

template<GraphicBuffer GB>
class LocalPainter : public GB
{
public:
	using ColorType = GB::ColorType;

	/**
	 * @brief 			Fundamental drawing routines. Should be self-explained.
	 */
	// OPTIMIZE check if pass-by-reference helps
	void draw(shape::Point point);
	void draw(shape::Line line);
	// void draw(shape::Polygon polygon, Style style = Style::Outline);
	void draw(shape::Rectangle rectangle, Style style = Style::Outline);
	void draw(shape::Rectangle rectangle, uint16_t radius, Style style = Style::Outline);
	void draw(shape::Circle circle, Style style = Style::Outline);
	void draw(shape::Ellipse ellipse, Style style = Style::Outline);

	template<color::Color CO, Size RO>
	LocalPainter& operator=(const Buffer<CO, RO> &other)
	{
		GB::template operator=(other);
		return *this;
	}
private:
	void drawQuadPoints(shape::Point center, shape::Point point);
};

} // namespace modm:: graphic

#include "painter_impl.hpp"
#include "painter_mono.hpp"