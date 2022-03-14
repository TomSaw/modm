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
#include "buffer_mal_palletized.hpp"

#include <modm/debug/logger.hpp>

namespace modm::graphic {

template<color::ColorPalletized C, Size R>
template<color::Color CO, template<typename> class Accessor>
void
BufferMal<C, R>::writeImage(ImageAccessor<CO, Accessor> accessor)
{
	// FIXME buggy for placement.y() < 0
	const shape::Section clipping = this->getIntersection(accessor.getSection());
	const Looper looper(clipping);

	for (int_fast16_t x = clipping.topLeft.x(); x < clipping.bottomRight.x(); x++)
	{
		size_t yb = looper.yb_top;
		int lshift = looper.lshift_top;
		accessor.incrementRow_preparePixel();

		// Top end
		if(yb < looper.yb_bot) {
			this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_top) | palletizeByte(accessor, lshift, digitsPallete);
			lshift = 0;
			yb++;
		}

		// Middle part
		while(yb < looper.yb_bot)
			this->buffer[yb++][x] = palletizeByte(accessor, 0, digitsPallete);

		// Bottom end
		this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | palletizeByte(accessor, lshift, looper.lshift_bot);
	}
}

template<color::ColorPalletized C, Size R>
template<template<typename> class Accessor>
void
BufferMal<C, R>::writeImage(ImageAccessor<C, Accessor> accessor)
{
	const shape::Section clipping = this->getIntersection(accessor.getSection());
	const Looper looper(clipping);

	const int rshift_bot = digitsPallete - looper.lshift_top;

	// FIXME looper.lshift_top always 0 for placement.y() < 0
	// Thus writeImage top offscreen is buggy
	// MODM_LOG_VAR(looper.lshift_top);

	if (looper.lshift_top) {
		// Split bytes, shift, reassemble
		for (int_fast16_t x = clipping.topLeft.x(); x < clipping.bottomRight.x(); x++)
		{
			accessor.incrementRow();
			size_t yb = looper.yb_top;

			// Top end
			if (clipping.topLeft.y() > 0)
			{
				this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_top) | *accessor << looper.lshift_top;
				yb++;
			}

			// Middle part
			while (yb < looper.yb_bot)
			{
				this->buffer[yb][x] = *accessor >> rshift_bot;
				accessor.incrementCol(); // Byte
				this->buffer[yb][x] |= *accessor << looper.lshift_top;
				yb++;
			}

			// Bottom end
			if (clipping.bottomRight.y() < int16_t(R.y()))
				this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | *accessor >> rshift_bot;
		}
	} else if (clipping.topLeft.y() < clipping.bottomRight.y()) {
		// Copy bytes
		for (int_fast16_t x = clipping.topLeft.x(); x < clipping.bottomRight.x(); x++)
		{
			accessor.incrementRow();
			size_t yb = looper.yb_top;

			while (yb < looper.yb_bot)
			{
				// OPTIMIZE Concider std::copy for Accessor == modm::accessor::Ram
				this->buffer[yb++][x] = *accessor;
				accessor.incrementCol();
			}

			this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | *accessor;
		}
	}
}

template<color::ColorPalletized C, Size R>
void
BufferMal<C, R>::drawBlind(const shape::Point& point)
{
	TPallete& byte = getByte(point);
	const int lshift = getYlshift(point.y());

	byte = (byte & ~(C::max << lshift)) | this->color.value() << lshift;
}

template<color::ColorPalletized C, Size R>
void
BufferMal<C, R>::drawBlind(const shape::HLine& hline)
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
BufferMal<C, R>::drawBlind(const shape::VLine& vline)
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
BufferMal<C, R>::drawBlind(const shape::Section& section)
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
BufferMal<C, R>::getBlind(const shape::Point& point) const
{
	return C(getByte(point) >> getYlshift(point.y()) & C::max);
}

} // namespace modm::graphic