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
#include "buffer_memory_palletized.hpp"

namespace modm::graphic {

template<color::ColorPalletized C, Size R>
void
BufferMemory<C, R>::drawBlind(const shape::Point& point)
{
	TPallete& byte = getByte(point);
	const int lshift = getYlshift(point.y());

	byte = (byte & ~(C::max << lshift)) | this->color.value() << lshift;
}

template<color::ColorPalletized C, Size R>
void
BufferMemory<C, R>::drawBlind(const shape::HLine& hline)
{
	const std::size_t yb = getY(hline.start.y());
	const int lshift = getYlshift(hline.start.y());

	const TPallete keepmask = ~(C::max << lshift);
	const typename C::T value = this->color.value() << lshift;

	size_t x = hline.start.x();
	while (x <= hline.end_x) {
		buffer[yb][x] = (buffer[yb][x] & keepmask) | value;
		x++;
	}
}

template<color::ColorPalletized C, Size R>
void
BufferMemory<C, R>::drawBlind(const shape::VLine& vline)
{
	const Looper looper(vline);
	const TPallete byte_middle = clearValue(this->color);

	const std::size_t x = vline.start.x();
	size_t yb = looper.yb_top;

	// Top end
	if(yb < looper.yb_bot) {
		buffer[yb][x] = (buffer[yb][x] & looper.keepmask_top) | (byte_middle & ~looper.keepmask_top);
		yb++;
	}

	// Middle part
	while (yb < looper.yb_bot)
		buffer[yb++][x] = byte_middle;

	// Bottom end
	buffer[yb][x] = (buffer[yb][x] & looper.keepmask_bot) | (byte_middle & ~looper.keepmask_bot);
}

template<color::ColorPalletized C, Size R>
void
BufferMemory<C, R>::drawBlind(const shape::Section& section)
{
	const Looper looper(section);

	const TPallete byte_middle = clearValue(this->color);

	for (int_fast16_t x = section.topLeft.x(); x < section.bottomRight.x(); x++) {
		size_t yb = looper.yb_top;

		// Top end
		if(yb < looper.yb_bot) {
			buffer[yb][x] = (buffer[yb][x] & looper.keepmask_top) | (byte_middle & ~looper.keepmask_top);
			yb++;
		}

		// Middle part
		while (yb < looper.yb_bot)
			buffer[yb++][x] = byte_middle; // OPTIMIZE would std::fill be faster ?

		// Bottom end
		buffer[yb][x] = (buffer[yb][x] & looper.keepmask_bot) | (byte_middle & ~looper.keepmask_bot);
	}
}

template<color::ColorPalletized C, Size R>
C
BufferMemory<C, R>::getBlind(const shape::Point& point) const
{
	return C(getByte(point) >> getYlshift(point.y()) & C::max);
}

} // namespace modm::graphic