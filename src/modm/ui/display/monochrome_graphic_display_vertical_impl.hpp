/*
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_VERTICAL_HPP
#error "Don't include this file directly, use 'monochrome_graphic_display_vertical.hpp' instead!"
#endif

#include <algorithm>

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawHorizontalLine(glcd::Point start,
																		  uint16_t length)
{
	if (this->yOnScreen(start.y))
	{
		const uint8_t byte = 1 << (start.y % 8);
		const size_t yb = start.y / 8;
		const size_t x_max = std::min<size_t>(start.x + length, Width - 1);

		for (size_t x = start.x; x < x_max; x++)
			this->buffer[yb][x] |= byte;
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawVerticalLine(glcd::Point start,
																		uint16_t length)
{
	if (this->xOnScreen(start.x))
	{
		size_t yb = start.y / 8;
		const size_t y_max = std::min<size_t>((start.y + length), Height);
		const size_t yb_max = y_max / 8;

		uint8_t byte = 0xFF << start.y % 8; // Mask out start

		while (yb != yb_max)
		{
			this->buffer[yb][start.x] |= byte;
			byte = 0xFF;
			yb++;
		}

		byte &= 0xFF >> (8 - y_max % 8); // Mask out end
		this->buffer[yb][start.x] |= byte;
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawImageRaw(
	glcd::Point start, uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	if (!start.y % 8 && !height % 8)
	{
		const size_t x_max = std::min<size_t>(start.x + width, Width - 1);
		const size_t yb_max = std::min<size_t>(start.y / 8 + height, Height / 8 - 1);
		size_t i = 0;

		for (size_t x = start.x; x < x_max; x++)
			for (size_t yb = start.y / 8; yb <= yb_max; yb++) this->buffer[yb][x] = data[i++];
	} else
		GraphicDisplay::drawImageRaw(start, width, height, data);
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::setPixel(int16_t x, int16_t y)
{
	if (this->xOnScreen(x) and this->yOnScreen(y))
		this->buffer[y / 8][x] |= (1 << y % 8);
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if (this->xOnScreen(x) and this->yOnScreen(y))
		this->buffer[y / 8][x] &= ~(1 << y % 8);
}

template<uint16_t Width, uint16_t Height>
bool
modm::MonochromeGraphicDisplayVertical<Width, Height>::getPixel(int16_t x, int16_t y) const
{
	if (this->xOnScreen(x) and this->yOnScreen(y))
		return (this->buffer[y / 8][x] & (1 << y % 8));
	else
		return false;
}