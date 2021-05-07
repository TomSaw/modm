/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2014, 2017, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2017, Christopher Durand
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GRAPHIC_DISPLAY_HPP
#error "Don't include this file directly, use 'graphic_display.hpp' instead!"
#endif

#include "font/fixed_width_5x8.hpp"

using namespace std;

template<uint16_t Width, uint16_t Height>
modm::GraphicDisplay<Width, Height>::GraphicDisplay()
	: IOStream(writer), writer(this), font(modm::accessor::asFlash(modm::font::FixedWidth5x8))
{}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawLine(glcd::Point start, glcd::Point end)
{
	if (start.x == end.x)
	{
		// start.x|start.y must be the upper point
		if (start.y > end.y) { swap(start.y, end.y); }
		this->drawVerticalLine({start.x, start.y}, end.y - start.y + 1);
	} else if (start.y == end.y)
	{
		// start.x|start.y must be the left point
		if (start.x > end.x) { swap(start.x, end.x); }
		this->drawHorizontalLine({start.x, start.y}, end.x - start.x + 1);
	} else
	{
		// bresenham algorithm
		bool steep = abs(end.y - start.y) > abs(end.x - start.x);
		if (steep)
		{
			swap(start.x, start.y);
			swap(end.x, end.y);
		}

		if (start.x > end.x)
		{
			swap(start.x, end.x);
			swap(start.y, end.y);
		}

		int16_t deltaX = end.x - start.x;
		int16_t deltaY = abs(end.y - start.y);
		int16_t error = deltaX / 2;

		int16_t yStep;
		int16_t y = start.y;

		if (start.y < end.y)
			yStep = 1;
		else
			yStep = -1;

		for (int16_t x = start.x; x <= end.x; ++x)
		{
			// OPTIMIZE use setPixelFast(pos);
			if (steep)
				this->setPixel({y, x});
			else
				this->setPixel({x, y});

			error = error - deltaY;

			if (error < 0)
			{
				y += yStep;
				error += deltaX;
			}
		}
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawHorizontalLine(glcd::Point start, int16_t length)
{
	const int16_t x_max = min<int16_t>(start.x + length, Width);
	for (int16_t x = start.x; x < x_max; x++) this->setPixelFast({x, start.y});
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawVerticalLine(glcd::Point start, int16_t length)
{
	const int16_t y_max = min<int16_t>(start.y + length, Height);
	for (int16_t y = start.y; y < y_max; y++) this->setPixelFast({start.x, y});
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawRectangle(glcd::Point start, int16_t width, int16_t height)
{
	this->drawHorizontalLine(start, width);
	this->drawHorizontalLine({start.x, start.y + height - 1}, width);
	this->drawVerticalLine(start, height);
	this->drawVerticalLine({start.x + width - 1, start.y}, height);
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawRoundedRectangle(glcd::Point start, int16_t width,
														  int16_t height, int16_t radius)
{
	if (radius == 0) this->drawRectangle(start, width, height);

	int16_t x = 0;
	int16_t y = radius;
	int16_t f = 3 - 2 * radius;

	while (x <= y)
	{
		// OPTIMIZE precalculate Points
		this->setPixel({start.x + radius - x, start.y + radius - y});
		this->setPixel({start.x + radius - x, start.y + height - radius + y});
		this->setPixel({start.x + radius - y, start.y + radius - x});
		this->setPixel({start.x + radius - y, start.y + height - radius + x});

		this->setPixel({start.x + width - radius + x, start.y + radius - y});
		this->setPixel({start.x + width - radius + x, start.y + height - radius + y});
		this->setPixel({start.x + width - radius + y, start.y + radius - x});
		this->setPixel({start.x + width - radius + y, start.y + height - radius + x});

		if (f < 0)
		{
			f += (4 * x + 6);
		} else
		{
			f += (4 * (x - y) + 10);
			y--;
		}
		x++;
	}

	this->drawHorizontalLine({start.x + radius, start.y}, width - (2 * radius));
	this->drawHorizontalLine({start.x + radius, start.y + height}, width - (2 * radius));
	this->drawVerticalLine({start.x, start.y + radius}, height - (2 * radius));
	this->drawVerticalLine({start.x + width, start.y + radius}, height - (2 * radius));
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawCircle(glcd::Point center, int16_t radius)
{
	if (radius == 0) { return; }

	int16_t error = -radius;
	int16_t x = radius;
	int16_t y = 0;

	while (x > y)
	{
		this->drawCircle4(center, x, y);
		this->drawCircle4(center, y, x);

		error += y;
		++y;
		error += y;

		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
	this->drawCircle4(center, x, y);
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawCircle4(glcd::Point center, int16_t x, int16_t y)
{
	const int16_t cx = center.x;
	const int16_t cy = center.y;

	// OPTIMIZE use setPixelFast(pos);
	this->setPixel({cx + x, cy + y});
	this->setPixel({cx - x, cy - y});
	if (x != 0) this->setPixel({cx - x, cy + y});
	if (y != 0) this->setPixel({cx + x, cy - y});
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawEllipse(glcd::Point center, int16_t rx, int16_t ry)
{
	int32_t rx_2 = rx * rx;
	int32_t ry_2 = ry * ry;

	int16_t x = 0;
	int16_t y = ry;

	int32_t fx = 0;
	int32_t fy = rx_2 * 2 * ry;

	int32_t p = ry_2 - (rx_2 * ry) + (rx_2 + 2) / 4;

	drawCircle4(center, x, y);
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

		drawCircle4(center, x, y);
	}

	p = ((ry_2 * (4 * x * x + 4 * x + 1) / 2) + 2 * (rx_2 * (y - 1) * (y - 1)) - 2 * (rx_2 * ry_2) +
		 1) /
		2;

	while (y > 0)
	{
		y--;
		fy -= rx_2 * 2;

		if (p >= 0)
		{
			p += (rx_2 - fy);
		} else
		{
			x++;
			fx += ry_2 * 2;
			p += (fx + rx_2 - fy);
		}

		drawCircle4(center, x, y);
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawImage(glcd::Point start,
											   modm::accessor::Flash<uint8_t> image)
{
	uint8_t width = image[0];
	uint8_t height = image[1];

	drawImageRaw(start, width, height, modm::accessor::Flash<uint8_t>(image.getPointer() + 2));
}


#include <modm/debug/logger.hpp>
/**
 * Convertion MonochromeBuffer -> AnyBuffer
 */
template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::drawImageRaw(glcd::Point start, uint16_t width,
												  uint16_t height,
												  modm::accessor::Flash<uint8_t> data)
{
	// TODO check if start is in ScreenArea

	size_t x_max = min(width, Width);
	size_t yb_max = min(height, Height) / 8;

	for (size_t x = 0; x < x_max; x++)
		for (size_t yb = 0; yb < yb_max; yb++)
		{
			const int16_t y = yb * 8;

			uint8_t byte = data[x + yb * width];

			for (uint8_t j = 0; j < min<uint8_t>(height - y, 8); j++)
			{
				glcd::Point pos{start.x + x, start.y + y + j};

				// OPTIMIZE above code, so we don't need pointOnScreen(pos)-check
				if(!pointOnScreen(pos)) {
					MODM_LOG_ERROR << pos << modm::endl;
				}
				if (pointOnScreen(pos))
				{
					if (byte & 0x01)
						this->setPixelFast(pos);
					else
						this->clearPixelFast(pos);
				}

				byte >>= 1;
			}
		}
}
