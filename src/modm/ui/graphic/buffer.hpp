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

#include <modm/math/geometry/size.hpp>
#include "buffer_memory_definition.hpp"
#include "colorindex.hpp"

#include "cursor.hpp"
#include "view.hpp"
#include "painter_fast.hpp"

// #include "buffer_painter_fast.hpp"

// #include "buffer_interface.hpp"

namespace modm::graphic {

template<BufMemDef BMD, Size R>
class BufferBase;

template<BufMemDef BMD, Size R>
class Buffer : public BufferBase<BMD, R>
{
public:
	using MemoryDefinition = BMD;
	static constexpr Size size = R;
	
	using ColorType = BMD::ColorType;
	using CursorType = BufferBase<BMD, R>::CursorType;
	using ViewType = View<Buffer<BMD, R>>;

	// TODO as template arg
	using PainterType = PainterFast<Buffer<BMD, R>>;

	// constructors
	using BufferBase<BMD, R>::BufferBase;

	// conversion Point -> Cursor
	CursorType operator()(Point point)
	{ return CursorType(this->data()) + point; }

	// conversion Cursor -> Point
	Point operator()(CursorType cursor) const
	{ return cursor.getPoint(&cursor - this->data()); }

	// TODO let's see, if getX() getY() is needed needed ...
/* 	uint16_t getX(CursorType cursor) const
	{ return cursor.getX(&cursor - this->data()); }

	uint16_t getY(CursorType cursor) const
	{ return cursor.getY(&cursor - this->data()); } */

	// Create a View
	auto operator[](Section section)
	{ return ViewType(*this, section); }
	
	// Create a Painter
	auto operator[](Point point)
	{ return PainterType(*this, point); }


	void invert()
	{
		std::for_each(this->span().begin(), this->span().end(), [](auto &value){value.invert();});
	}
};

}

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<modm::graphic::BufMemDef BMD, modm::Size R>
modm::IOStream &
// TODO Would be nice, if palletes become visible by divergating the output color like a checkerboard
// TODO support to pass const &pallete
// operator<<(modm::IOStream &os, const modm::pallete<TP, DigitsElement, TE> &pallete)
operator<<(modm::IOStream &os, modm::graphic::Buffer<BMD, R> &buffer)
{
	using ColorType = BMD::ColorType;
	/// Print header
	os << "Width: " << R.width() << "px\t";
	os << "Height: " << R.height() << "px\t";
	os << "ColorType: " << ColorType::label() << modm::endl;

	// modm::graphic::ColorIndex<char*, modm::color::Gray<2>> index = {
	// 	(char*)"░",
	// 	(char*)"▒",
	// 	(char*)"▓",
	// 	(char*)"█"
	// };

	// TODO use this index for hosted ...
	#if 0
	const modm::graphic::ColorIndex<char*, modm::color::Gray<3>> index = {
		(char*)" ",
		(char*)"░",
		(char*)"░",
		(char*)"▒",
		(char*)"▒",
		(char*)"▓",
		(char*)"▓",
		(char*)"█"
	};
	#else
	// ... and this for embedded applications.
	const modm::graphic::ColorIndex<char, modm::color::Gray<2>> index = {
		' ',
		'.',
		'o',
		'O'
	};
	#endif
	
	// IMPLEMENT range based loop for columns?
 	for(uint16_t col = 0; col < R.height(); ++col) {
		for(ColorType pixel : buffer.xspan(col))
			os << index[pixel];
		os << modm::endl;
	}

	return os;
}
#endif

#include "buffer_planar.hpp"
#include "buffer_palletized.hpp"