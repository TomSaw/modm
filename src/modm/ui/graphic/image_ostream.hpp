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

#include "image.hpp"
#include <modm/ui/color/gray.hpp>
#include <modm/io/iostream.hpp>

/**
 * @brief 				Outpouts an Image on a IOStream. Primarily used for development.
 * 
 * @tparam Image 
 * @param os 
 * @param image 
 * @return modm::IOStream& 
 */

// TODO need Image concept
template <class Image>
modm::IOStream &
operator<<(modm::IOStream &os, Image& image)
{
	/// Print header
	os << "Width: " << image.size.width() << "px\t";
	os << "Height: " << image.size.height() << "px\t";
	os << "Encoder: " << "TODO" << '\t';
	os << "Accessor: " << "TODO" << modm::endl;

	using Color = modm::color::Gray<2>;
			
	// const char shades[Color::max + 1] = {' ', '.', 'o', 'O'};
	const char* shades[Color::max + 1] = {"░", "▒", "▓", "█"};

	
	for(auto column : image.iterable()) {
		for(auto pixel : column) {
			// TODO Simple Color via IOStreams color feature
			os << shades[Color(pixel).value()];
		}
		os << modm::endl;
	}

	return os;
}