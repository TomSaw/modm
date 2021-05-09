/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2013, Thorsten Lajewski
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

#include <algorithm>

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::fillRectangle(glcd::Point start, int16_t width, int16_t height)
{
	const int16_t x_max = std::min<int16_t>(start.x + width, getWidth());
	const int16_t y_max = std::min<int16_t>(start.y + height, getHeight());

	for (int16_t x = start.x; x < x_max; x++)
		for (int16_t y = start.y; y < y_max; y++) this->setPixel({x, y});
}

template<uint16_t Width, uint16_t Height>
void
modm::GraphicDisplay<Width, Height>::fillCircle(glcd::Point center, int16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	this->drawVerticalLine({center.x, center.y - radius}, 2 * radius);

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

		this->drawVerticalLine({center.x + x, center.y - y}, 2 * y);
		this->drawVerticalLine({center.x + y, center.y - x}, 2 * x);
		this->drawVerticalLine({center.x - x, center.y - y}, 2 * y);
		this->drawVerticalLine({center.x - y, center.y - x}, 2 * x);
	}
}