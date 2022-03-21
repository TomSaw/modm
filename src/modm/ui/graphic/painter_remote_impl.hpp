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
#include "painter_remote.hpp"

#include <algorithm>
#include <bit>

namespace modm::graphic {

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Point point)
{
	RF_BEGIN();
	if (this->pointIntersects(point))
		RF_CALL(GD::drawBlind(point));
	RF_END();
};

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Line line)
{
	RF_BEGIN();

	if (line.isHorizontal())
	{
		if(this->yIntersects(line.start.y())) {
			if (line.start.x() > line.end.x()) std::swap(line.start.x(), line.end.x());

			RF_RETURN_CALL(GD::drawBlind(
				shape::HLine(
					{std::max<int16_t>(line.start.x(), 0), line.start.y()},
					std::min<int16_t>(line.end.x(), this->getWidth())
				)
			));
		}
	} else if (line.isVertical())
	{
		if(this->xIntersects(line.start.x())) {
			if (line.start.y() > line.end.y()) std::swap(line.start.y(), line.end.y());

			RF_RETURN_CALL(GD::drawBlind(
				shape::VLine(
					{line.start.x(), std::max<int16_t>(line.start.y(), 0)},
					std::min<int16_t>(line.end.y(), this->getHeight())
				)
			));
		}
	} else
	{
		l.line = line;

 		l.line.clip(this->asSection());

		// Bresenham algorithm
		l.delta = l.line.end - l.line.start;
		l.steep = std::abs(l.delta.y()) > std::abs(l.delta.x());
		if (l.steep)
		{
			l.line.start.swap();
			l.line.end.swap();
		}
		if (l.line.start.x() > l.line.end.x())
			std::swap(line.start, line.end);

		l.delta = l.line.end - l.line.start;
		l.step = l.delta.y() > 0 ? 1 : -1;
		l.error = l.delta.x() / 2;

		if (l.steep)
		{
			while (l.line.start.x() <= l.line.end.x())
			{
				RF_CALL(GD::drawBlind(l.line.start.swapped()));
				l.line.start.x()++;

				l.error -= std::abs(l.delta.y());
				if (l.error < 0)
				{
					l.line.start.y() += l.step;
					l.error += l.delta.x();
				}
			}
		} else
		{
			while (l.line.start.x() <= l.line.end.x())
			{
				RF_CALL(GD::drawBlind(l.line.start));
				l.line.start.x()++;

				l.error -= std::abs(l.delta.y());
				if (l.error < 0)
				{
					l.line.start.y() += l.step;
					l.error += l.delta.x();
				}
			}
		}
	}

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Rectangle rectangle, Style style)
{
	RF_BEGIN();

	section = this->getIntersection(rectangle);

	if (style == Style::Outline)
	{
		r.bottomRight = rectangle.getBottomRight();
		if (this->yIntersects(rectangle.origin.y()))
			RF_CALL(GD::drawBlind(shape::HLine(section.topLeft, section.bottomRight.x() - 1)));

		if (this->yIntersects(r.bottomRight.y()))
			RF_CALL(GD::drawBlind(shape::HLine(section.getBottomLeft(), section.bottomRight.x() - 1)));

		if (this->xIntersects(rectangle.origin.x()))
			RF_CALL(GD::drawBlind(shape::VLine(section.topLeft, section.bottomRight.y() - 1)));

		if (this->xIntersects(r.bottomRight.x()))
			RF_CALL(GD::drawBlind(shape::VLine(section.getTopRight(), section.bottomRight.y() - 1)));
	}
	else /* if (style == Style::Filled) */
		RF_CALL(GD::drawBlind(section));

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Rectangle rectangle, uint16_t radius, Style style)
{
	RF_BEGIN();

	if (radius == 0)
		RF_RETURN_CALL(draw(rectangle, style));

	if(style == Style::Outline) {
		const shape::Point center = rectangle.getCenter();
		const shape::Point circle_off = rectangle.size / 2 - shape::Point(radius, radius);

		int16_t error = -radius;
		shape::Point p(radius, 0);

		while (p.x() > p.y())
		{
			RF_CALL(drawQuadPoints(center, p + circle_off));
			RF_CALL(drawQuadPoints(center, shape::Point(p.y(), p.x()) + circle_off));

			error += p.y();
			++p.y();
			error += p.y();

			if (error >= 0)
				error -= 2 * --p.x();
		}
		RF_CALL(drawQuadPoints(center, p + circle_off));

		// FIXME Crashes if one of these lines cross screen-border
		// OPTIMIZE There's potential for more recycling
		if (this->yIntersects(rectangle.origin.y()))
			RF_CALL(GD::drawBlind(
				shape::HLine({center.x() - circle_off.x(), rectangle.origin.y()}, center.x() + circle_off.x())
			));

		if (this->yIntersects(rectangle.getBottomRight().y()))
			RF_CALL(GD::drawBlind(
				shape::HLine({center.x() - circle_off.x(), rectangle.getBottomRight().y()}, center.x() + circle_off.x())
			));

		if (this->xIntersects(rectangle.origin.x()))
			RF_CALL(GD::drawBlind(
				shape::VLine({rectangle.origin.x(), center.y() - circle_off.y()}, center.y() + circle_off.y())
			));

		if (this->xIntersects(rectangle.getBottomRight().x()))
			RF_CALL(GD::drawBlind(
				shape::VLine({rectangle.getBottomRight().x(), center.y() - circle_off.y()}, center.y() + circle_off.y())
			));
	}
	else if(style == Style::Filled) {
		// IMPLEMENT filled, rounded Rectangle
		RF_CALL(draw(rectangle, style));
	}

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Circle circle, Style style)
{
	RF_BEGIN();

	if (circle.radius == 0)
		RF_RETURN_CALL(draw(circle.center));

	if (style == Style::Outline)
	{
		co.error = -circle.radius;
		co.point = {circle.radius, 0};

		while (co.point.x() > co.point.y())
		{
			RF_CALL(drawQuadPoints(circle.center, co.point));
			RF_CALL(drawQuadPoints(circle.center, co.point.swapped()));

			co.error += co.point.y();
			++co.point.y();
			co.error += co.point.y();

			if (co.error >= 0) co.error -= 2 * --co.point.x();
		}
		RF_CALL(drawQuadPoints(circle.center, co.point));
	}
 	else if (style == Style::Filled) {
		cf.f = 1 - circle.radius;
		cf.ddF_x = 0;
		cf.ddF_y = -2 * circle.radius;
		cf.x = 0;
		cf.y = circle.radius;

		cf.start = {circle.center.x(), circle.center.y() - circle.radius};
		RF_CALL(GD::drawBlind(shape::VLine(cf.start, cf.start.y() + 2 * circle.radius)));

		while (cf.x < cf.y)
		{
			if (cf.f >= 0)
			{
				cf.y--;
				cf.ddF_y += 2;
				cf.f += cf.ddF_y;
			}
			cf.x++;
			cf.ddF_x += 2;
			cf.f += cf.ddF_x + 1;

			cf.start = circle.center + shape::Point(cf.x, -cf.y);
			RF_CALL(GD::drawBlind(shape::VLine(cf.start, cf.start.y() + 2 * cf.y)));
			cf.start = circle.center + shape::Point(cf.y, -cf.x);
			RF_CALL(GD::drawBlind(shape::VLine(cf.start, cf.start.y() + 2 * cf.x)));
			cf.start = circle.center - shape::Point(cf.x, cf.y);
			RF_CALL(GD::drawBlind(shape::VLine(cf.start, cf.start.y() + 2 * cf.y)));
			cf.start = circle.center - shape::Point(cf.y, cf.x);
			RF_CALL(GD::drawBlind(shape::VLine(cf.start, cf.start.y() + 2 * cf.x)));
		}
	}

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::draw(shape::Ellipse ellipse, Style style)
{
	RF_BEGIN();

	if(style == Style::Outline) {
		// IMPLEMENT RemotePainter brensenham ellipse
/* 		int32_t rx_2 = ellipse.radius_x * ellipse.radius_x;
		int32_t ry_2 = ellipse.radius_y * ellipse.radius_y;

		int16_t x = 0;
		int16_t y = ellipse.radius_y;

		int32_t fx = 0;
		int32_t fy = rx_2 * 2 * ellipse.radius_y;

		int32_t p = ry_2 - (rx_2 * ellipse.radius_y) + (rx_2 + 2) / 4;

		RF_CALL(drawQuadPoints(ellipse.center, x, y));
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

			RF_CALL(drawQuadPoints(ellipse.center, x, y));
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

			RF_CALL(drawQuadPoints(ellipse.center, x, y)); */
	}
	else if(style == Style::Filled) {
		// IMPLEMENT Ellipse Style::Filled;
		RF_CALL(draw(ellipse, Style::Outline));
	}

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<void>
RemotePainter<GD>::drawQuadPoints(shape::Point center, shape::Point point)
{
	RF_BEGIN();

 	qpoint = center + point;
	drawXpos = this->xIntersects(qpoint.x());
	drawXneg = this->xIntersects(center.x() - point.x());

	if(this->yIntersects(qpoint.y())) {
		if(drawXpos)
			RF_CALL(GD::drawBlind(qpoint));
		if(point.x() != 0 and drawXneg) {
			qpoint.x() = center.x() - point.x();
		 	RF_CALL(GD::drawBlind(qpoint));
		}
	}
	if(point.y() != 0) {
		qpoint = center - point;
		if(this->yIntersects(qpoint.y())) {
			if(drawXneg)
				RF_CALL(GD::drawBlind(qpoint));
			if(point.x() != 0 and drawXpos) {
				qpoint.x() = center.x() + point.x();
				RF_CALL(GD::drawBlind(qpoint));
			}
		}
	}

	RF_END();
}

template<GraphicDisplay GD>
modm::ResumableResult<typename GD::ColorType>
RemotePainter<GD>::get(shape::Point point) const
{
	RF_BEGIN();
	if(this->pointIntersects(point)) RF_RETURN_CALL(this->getBlind(point));
	RF_END_RETURN(0);
}

template<GraphicDisplay GD>
template<color::Color CO>
modm::ResumableResult<void>
RemotePainter<GD>::write(BufferInterface<CO> &buffer, shape::Point placement) {
	RF_BEGIN();
	RF_END_RETURN_CALL(this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&buffer, placement)));
}

template<GraphicDisplay GD>
template<color::Color CO>
modm::ResumableResult<void>
RemotePainter<GD>::write(const uint8_t *addr, shape::Point placement) {
	RF_BEGIN();
	RF_END_RETURN_CALL(this->writeImage(ImageAccessor<CO, modm::accessor::Flash>(addr, placement)));
}

} // namespace modm::graphic