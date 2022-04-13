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

#include <modm/ui/graphic/concepts.hpp>
#include <modm/math/geometry/size.hpp>
#include <modm/math/geometry/shape/section.hpp>

namespace modm::graphic
{

enum Encoding : uint8_t
{
	Uncompressed,
	RunLength,
};

template<typename T>
class ImageBase {
protected:
	T const *data;

public:
	const Size size;

	ImageBase(T const *data, Size size)
		: data(data), size(size)
	{}
};

/**
 * @brief 	This Image class uses a nested iterator approach, allowing to scan out the Image
 * 			column and pixel wise. The approach is straight forward, performs great with Uncompressed
 * 			(Planar or Palettized) and RunLenght encoded Images.
 *			
 * 			When implementing png or jpg decoders however, another/an aditional design is required.
 *
 */
template<color::Color, Encoding>
class Image;

}  // namespace modm::graphic


#include <modm/ui/color/gray.hpp>
#include <modm/io/iostream.hpp>

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

#include "image_uncompressed_planar.hpp"
#include "image_uncompressed_palletized.hpp"
#include "image_runlength.hpp"