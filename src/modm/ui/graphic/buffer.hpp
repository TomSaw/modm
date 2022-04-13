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
#include "painter_fast.hpp"

// #include "buffer_painter_fast.hpp"

// #include "buffer_interface.hpp"

namespace modm::graphic {
	template<BufMemDef BMD, Size R>
	class Buffer;
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

	modm::graphic::ColorIndex<char*, modm::color::Gray<3>> index = {
		(char*)" ",
		(char*)"░",
		(char*)"░",
		(char*)"▒",
		(char*)"▒",
		(char*)"▓",
		(char*)"▓",
		(char*)"█"
	};
	
	// IMPLEMENT range based loop for columns?
 	for(uint16_t col = 0; col < R.height(); ++col) {
		for(ColorType pixel : buffer.rowspan(col))
			os << index[pixel] << index[pixel]; // Output 2 times so pixels look square
		os << modm::endl;
	}

	return os;
}
#endif

#include "buffer_planar.hpp"
#include "buffer_palletized.hpp"