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
																		  int16_t length)
{
	if (this->yOnScreen(start.y))
	{
		const uint8_t byte = 1 << (start.y % 8);
		const size_t yb = start.y / 8;
		const size_t x_max = std::min<size_t>(start.x + length, Width - 1);

		for (size_t x = start.x; x < x_max; x++) this->buffer[yb][x] |= byte;
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawVerticalLine(glcd::Point start,
																		int16_t length)
{
	if (this->xOnScreen(start.x))
	{
		size_t yb = start.y / 8;
		const size_t y_max = std::min<size_t>((start.y + length), Height);
		const size_t yb_max = y_max / 8;

		// TODO Preserve existing pixels on top
		uint8_t byte = 0xFF << start.y % 8;  // Mask out start

		while (yb != yb_max)
		{
			this->buffer[yb][start.x] |= byte;
			byte = 0xFF;
			yb++;
		}

		// TODO Preserve existing pixels on bottom
		byte &= 0xFF >> (8 - y_max % 8);  // Mask out end

		this->buffer[yb][start.x] |= byte;
	}
}

#define MODM_LOG_DEBUG_VAR(var) MODM_LOG_DEBUG << #var << ":\t" << var << modm::endl
// OPTIMIZE This whole thing could work faster if more than a byte is handled at once.
// A whole column could be loaded, shiftet, masked and written in one cicle.
template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawImageRaw(
	glcd::Point pos, uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	const glcd::Point screenMin{
		max<int16_t>(pos.x, 0),
		max<int16_t>(pos.y, 0) };

	const glcd::Point screenMax{
		min<int16_t>(pos.x + width, Width - 1),
		min<int16_t>(pos.y + height, Height - 1) };

	const size_t yb_min = screenMin.y / 8;
	size_t yb_max = (screenMax.y - 1) / 8;

	// Rare case -> why render completely out of screen? Anyways, (yb < yb_max - 1) = true,
	// thus could result in bad access on data so let's get out here
	if (yb_min >= yb_max) return;
	// if (yb_min == yb_max) return; // Should be enough

	const glcd::Point dataMin{
		pos.x < 0 ? -pos.x : 0,
		pos.y < 0 ? -pos.y : 0
	};

	size_t i_start = dataMin.x + (dataMin.y / 8) * width;

	if (!(pos.y % 8) and !(height % 8))
	{
		// data and buffer congruent in y-dir
		// simple fast copy bytes from data to buffer
		for (int16_t x = screenMin.x; x < screenMax.x; x++)
		{
			size_t i = i_start++;
			size_t yb = yb_min;
			while (yb < yb_max)
			{
				this->buffer[yb][x] = data[i];
				i += width;
				yb++;
			}
			// TODO Preserve existing pixels on bottom
			this->buffer[yb][x] = data[i];
		}
	} else
	{
		// data and buffer shifted against each other in y-dir
		// need to split data-bytes

		const uint8_t lshift_upper = pos.y & 0b111;
		const uint8_t rshift_lower = 8 - lshift_upper;
		const bool need_top_piece = pos.y >= 0;
		const bool need_bottom_piece = pos.y + height < Height;

		// OPTIMIZE feels like this can be omitted somehow
		if (!need_bottom_piece) yb_max++;

		for (int16_t x = screenMin.x; x < screenMax.x; x++)
		{
			size_t i = i_start++;
			size_t yb = yb_min;

			// Preserve existing pixels on top
			if (need_top_piece)
			{
				this->buffer[yb][x] &= 0xFF >> rshift_lower;
				this->buffer[yb][x] |= data[i] << lshift_upper;
				yb++;
			}
			while (yb < yb_max)
			{
				this->buffer[yb][x] = data[i] >> rshift_lower;
				i += width;
				this->buffer[yb][x] |= data[i] << lshift_upper;
				yb++;
			}
			// Preserve existing pixels on bottom
			if (need_bottom_piece)
			{
				this->buffer[yb][x] &= 0xFF << lshift_upper;
				this->buffer[yb][x] |= data[i] >> rshift_lower;
			}
		}
	}
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::setPixelFast(glcd::Point pos)
{
	this->buffer[pos.y / 8][pos.x] |= (1 << pos.y % 8);
}

template<uint16_t Width, uint16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::clearPixelFast(glcd::Point pos)
{
	this->buffer[pos.y / 8][pos.x] &= ~(1 << pos.y % 8);
}

template<uint16_t Width, uint16_t Height>
bool
modm::MonochromeGraphicDisplayVertical<Width, Height>::getPixelFast(glcd::Point pos) const
{
	return (this->buffer[pos.y / 8][pos.x] & (1 << pos.y % 8));
}