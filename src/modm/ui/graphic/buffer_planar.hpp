/*
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
#include "buffer.hpp"

#include <bit>

namespace modm::graphic
{

template<BufMemDef BMD, Size R>
requires color::ColorPlanar<typename BMD::ColorType>
class Buffer<BMD, R> : public array2d<typename BMD::ColorType, R.width(), R.height(), BMD::Major>
{
	using array2dT = array2d<typename BMD::ColorType, R.width(), R.height(), BMD::Major>;

public:
	using ColorType = BMD::ColorType;
	using CursorType = Cursor<ColorType, BMD::Major, (BMD::Major == Row) ? R.width() : R.height()>;

	// constructors
	using array2dT::array2d;

	// accessor
	auto operator[](Point point) {
		return PainterFast<Buffer<BMD, R>>(*this, point);
	}

	// conversion Point -> Cursor
	CursorType operator()(Point point)
	{
		return CursorType(this->data()) + point;
	}

	// conversion Cursor -> Point
	Point operator()(CursorType cursor) {
		const std::size_t offset = &cursor - this->data();

		if constexpr(BMD::Major == Row)
			return Point(offset % this->minorFreq, offset / this->minorFreq);
		else // (BMD::Major == Col)
			return Point(offset / this->minorFreq, offset % this->minorFreq);
	}

	int16_t getX(CursorType cursor) {
		const std::size_t offset = &cursor - this->data();

		if constexpr(BMD::Major == Row)
			return offset % this->minorFreq;
		else // (BMD::Major == Col)
			return offset / this->minorFreq;
	}

	int16_t getY(CursorType cursor) {
		const std::size_t offset = &cursor - this->data();

		if constexpr(BMD::Major == Row)
			return offset / this->minorFreq;
		else // (BMD::Major == Col)
			return offset % this->minorFreq;
	}

	#if 0
	// #######################################
	// Cheap Cursor-Buffer Predicates
	
	// Note: Palletized Cursor maybe inbound of memory but outbound of visible area
	// This behaviour is intended and helps to accelerate some low lvl manipulations
	bool contains(const CursorType& cursor)
	{ 
		return &cursor >= this->data() and &cursor < this->data() + this->size;
	}
	#endif
};
}  // namespace modm::graphic