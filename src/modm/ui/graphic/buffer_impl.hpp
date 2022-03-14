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
#include "buffer.hpp"

namespace modm::graphic {

template<color::Color C, Size R>
C
Buffer<C, R>::get(const shape::Point& point) const
{
	// modm_assert(this->pointIntersects(point), "LocalPainter", "get(shape::Point)", "value out of range");
	return this->pointIntersects(point) ? this->getBlind(point) : C(0);
};

template<color::Color C, Size R>
void
Buffer<C, R>::clear(ColorType color)
{
	// OPTIMIZE support DMA
	std::fill(std::begin(this->buffer_1d), std::end(this->buffer_1d), this->clearValue(color));
	cursor = {0, 0};
}

template<color::Color C, Size R>
void
Buffer<C, R>::invert()
{
	const auto fullSat = this->clearValue(color::Monochrome(1));
	std::for_each(std::begin(this->buffer_1d), std::end(this->buffer_1d), [=](auto &value){value ^= fullSat; });
}


template<color::Color C, Size R>
void
Buffer<C, R>::operator<<=(const std::size_t shift)
{
	for (size_t yb = 0; yb < Buffer<C, R>::BY; yb++)
	{
		size_t x = 0;
		while (x < R.x() - shift)
		{
			this->buffer[yb][x] = this->buffer[yb][x + shift];
			x++;
		}
		const auto clear = this->clearValue();
		while (x < R.x())
			this->buffer[yb][x++] = clear;
	}
}

template<color::Color C, Size R>
void
Buffer<C, R>::operator>>=(const std::size_t shift)
{
	for (size_t yb = 0; yb < Buffer<C, R>::BY; yb++)
	{
		size_t x = R.x();
		while (x > shift)
		{
			x--;
			this->buffer[yb][x] = this->buffer[yb][x - shift];
		}
		const auto clear = this->clearValue();
		while (x)
			this->buffer[yb][--x] = clear;
	}
}

template<color::Color C, Size R>
void
Buffer<C, R>::writeChar(char character)
{
	switch(character) {
		case '\n':
			do_linebreak();
			return;
		case '\t':
			const uint8_t tab_width = font->getTabWidth();
			if (cursor.x() < R.x() + tab_width)
				cursor.x() += tab_width;
			else
				do_linebreak();
			return;
	}

	if (not font->charExists(character))
		return;

	const Size charSize = font->getCharSize(character);

	if (linebreak and cursor.x() > R.x() - charSize.x())
		do_linebreak();

	// this->writeImage may be an asynchronious call one day...
	this->writeImage(font->getImageAccessor(character, cursor));
	// ... concider updating cursor.x() previosly.
	cursor.x() += charSize.x() + font->vspace;
}

} // namespace modm::graphic