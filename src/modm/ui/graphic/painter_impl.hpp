/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2014, 2017, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2017, Christopher Durand
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
#include "painter.hpp"

#include <algorithm>
#include <bit>

#include "../font/fixed_width_5x8.hpp"

namespace modm::graphic {

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Point point)
{
	if (this->pointIntersects(point))
		this->drawBlind(point);
};

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Line line)
{
	if (line.isHorizontal())
	{
		if(this->yIntersects(line.start.y())) {
			if (line.start.x() > line.end.x()) std::swap(line.start.x(), line.end.x());
			const shape::HLine hline(
				{std::max<int16_t>(line.start.x(), 0), line.start.y()},
				std::min<int16_t>(line.end.x(), this->getWidth())
			);
			this->drawBlind(hline);
		}
	} else if (line.isVertical())
	{
		if(this->xIntersects(line.start.x())) {
			if (line.start.y() > line.end.y()) std::swap(line.start.y(), line.end.y());
			const shape::VLine vline(
				{line.start.x(), std::max<int16_t>(line.start.y(), 0)},
				std::min<int16_t>(line.end.y(), this->getHeight())
			);
			this->drawBlind(vline);
		}
	} else
	{
		line.clip(shape::Section({0,0}, this->getSize()));

		// Bresenham algorithm
		const shape::Point deltaSteep = line.end - line.start;
		const bool steep = std::abs(deltaSteep.y()) > std::abs(deltaSteep.x());
		if (steep)
		{
			line.start.swap();
			line.end.swap();
		}
		if (line.start.x() > line.end.x())
			std::swap(line.start, line.end);

		const shape::Point delta = line.end - line.start;
		const int16_t step = delta.y() > 0 ? 1 : -1;
		int16_t error = delta.x() / 2;

		if (steep)
		{
			line.start.swap();
			while (line.start.y() <= line.end.x())
			{
				this->drawBlind(line.start);
				line.start.y()++;

				error -= std::abs(delta.y());
				if (error < 0)
				{
					line.start.x() += step;
					error += delta.x();
				}
			}
		} else
		{
			while (line.start.x() <= line.end.x())
			{
				this->drawBlind(line.start);
				line.start.x()++;

				error -= std::abs(delta.y());
				if (error < 0)
				{
					line.start.y() += step;
					error += delta.x();
				}
			}
		}
	}
}

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Rectangle rectangle, Style style)
{
	const shape::Section section = this->getIntersection(rectangle);

	if(style == Style::Outline) {
		const shape::Point bottomRight = rectangle.getBottomRight();

		if (this->yIntersects(rectangle.origin.y()))
			this->drawBlind(shape::HLine(section.topLeft, section.bottomRight.x()));

		if (this->yIntersects(bottomRight.y()))
			this->drawBlind(shape::HLine(section.getBottomLeft(), section.bottomRight.x()));

		if (this->xIntersects(rectangle.origin.x()))
			this->drawBlind(shape::VLine(section.topLeft, section.bottomRight.y()));

		if (this->xIntersects(bottomRight.x()))
			this->drawBlind(shape::VLine(section.getTopRight(), section.bottomRight.y()));
	}
	else if(style == Style::Filled) {
		this->drawBlind(section);
	}
}

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Rectangle rectangle, uint16_t radius, Style style)
{
	if (radius == 0) {
		draw(rectangle, style);
		return;
	}

	if(style == Style::Outline) {
		const shape::Point center = rectangle.getCenter();
		const shape::Point circle_off = rectangle.size / 2 - shape::Point(radius, radius);

		int16_t error = -radius;
		shape::Point p(radius, 0);

		while (p.x() > p.y())
		{
			drawQuadPoints(center, p + circle_off);
			drawQuadPoints(center, p.swapped() + circle_off);

			error += p.y();
			++p.y();
			error += p.y();

			if (error >= 0)
				error -= 2 * --p.x();
		}
		drawQuadPoints(center, p + circle_off);

		// FIXME Crashes if one of these lines cross screen-border
		// OPTIMIZE There's potential for more recycling
		if (this->yIntersects(rectangle.origin.y()))
			this->drawBlind(shape::HLine({center.x() - circle_off.x(), rectangle.origin.y()}, center.x() + circle_off.x()));

		if (this->yIntersects(rectangle.getBottomRight().y()))
			this->drawBlind(shape::HLine({center.x() - circle_off.x(), rectangle.getBottomRight().y()}, center.x() + circle_off.x()));

		if (this->xIntersects(rectangle.origin.x()))
			this->drawBlind(shape::VLine({rectangle.origin.x(), center.y() - circle_off.y()}, center.y() + circle_off.y()));

		if (this->xIntersects(rectangle.getBottomRight().x()))
			this->drawBlind(shape::VLine({rectangle.getBottomRight().x(), center.y() - circle_off.y()}, center.y() + circle_off.y()));
		}
 	else if(style == Style::Filled) {
		// IMPLEMENT filled, rounded Rectangle
		draw(rectangle, style);
	}
}

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Circle circle, Style style)
{
	if (circle.radius == 0) {
		draw(circle.center);
		return;
	}

	if(style == Style::Outline) {
		int16_t error = -circle.radius;
		shape::Point p(circle.radius, 0);

		while (p.x() > p.y())
		{
			drawQuadPoints(circle.center, p);
			drawQuadPoints(circle.center, p.swapped());

			error += p.y();
			++p.y();
			error += p.y();

			if (error >= 0)
				error -= 2 * --p.x();
		}
		drawQuadPoints(circle.center, p);
	}
	else if (style == Style::Filled) {
		int16_t f = 1 - circle.radius;
		int16_t ddF_x = 0;
		int16_t ddF_y = -2 * circle.radius;
		int16_t x = 0;
		int16_t y = circle.radius;

		shape::Point start(circle.center.x(), circle.center.y() - circle.radius);
		this->drawBlind(shape::VLine(start, start.y() + 2 * circle.radius));

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x + 1;

			start = circle.center + shape::Point(x, -y);
			this->drawBlind(shape::VLine(start, start.y() + 2 * y));
			start = circle.center + shape::Point(y, -x);
			this->drawBlind(shape::VLine(start, start.y() + 2 * x));
			start = circle.center - shape::Point(x, y);
			this->drawBlind(shape::VLine(start, start.y() + 2 * y));
			start = circle.center - shape::Point(y, x);
			this->drawBlind(shape::VLine(start, start.y() + 2 * x));
		}
	}
}

template<GraphicBuffer GB>
void
LocalPainter<GB>::draw(shape::Ellipse ellipse, Style style)
{
	if(style == Style::Outline) {
		int32_t rx_2 = ellipse.radius_x * ellipse.radius_x;
		int32_t ry_2 = ellipse.radius_y * ellipse.radius_y;

		int16_t x = 0;
		int16_t y = ellipse.radius_y;

		int32_t fx = 0;
		int32_t fy = rx_2 * 2 * ellipse.radius_y;

		int32_t p = ry_2 - (rx_2 * ellipse.radius_y) + (rx_2 + 2) / 4;

		drawQuadPoints(ellipse.center, {x, y});
		while (fx < fy)
		{
			x++;
			fx += ry_2 * 2;

			if (p < 0)
			{
				p += (fx + ry_2);
			} else
			{
				y--;
				fy -= rx_2 * 2;
				p += (fx + ry_2 - fy);
			}

			drawQuadPoints(ellipse.center, {x, y});
		}

		p = ((ry_2 * (4 * x * x + 4 * x + 1) / 2) + 2 * (rx_2 * (y - 1) * (y - 1)) - 2 * (rx_2 * ry_2) +
			1) /
			2;

		while (y > 0)
		{
			y--;
			fy -= rx_2 * 2;

			if (p >= 0)
				p += (rx_2 - fy);
			else {
				x++;
				fx += ry_2 * 2;
				p += (fx + rx_2 - fy);
			}

			drawQuadPoints(ellipse.center, {x, y});
		}
	}
	else if (style == Style::Filled) {
		// IMPLEMENT Ellipse Style::Filled;
		draw(ellipse, Style::Outline);
	}
}

template<GraphicBuffer GB>
void
LocalPainter<GB>::drawQuadPoints(shape::Point center, shape::Point point)
{
	shape::Point qpoint = center + point;
	bool drawXpos = this->xIntersects(qpoint.x());
	bool drawXneg = this->xIntersects(center.x() - point.x());

	if(this->yIntersects(qpoint.y())) {
		if(drawXpos)
			this->drawBlind(qpoint);
		if(point.x() != 0 and drawXneg) {
			qpoint.x() = center.x() - point.x();
		 	this->drawBlind(qpoint);
		}
	}
	if(point.y() != 0) {
		qpoint = center - point;
		if(this->yIntersects(qpoint.y())) {
			if(drawXneg)
				this->drawBlind(qpoint);
			if(point.x() != 0 and drawXpos) {
				qpoint.x() = center.x() + point.x();
				this->drawBlind(qpoint);
			}
		}
	}
}
} // namespace modm::graphic