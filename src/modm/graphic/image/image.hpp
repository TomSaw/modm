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

#include <modm/graphic/graphic/concepts.hpp>
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
 * @brief 	Image types are restricted to "scan-access": Image data can only be accessed pixel by pixel / pallete by pallete
 * 			using column and row iterators. Buffers - in contrast - support pixel random access required for fast manipulations.
 * 			Images will always be square, so there's no drawback.
 * 			
 * 			The benefits of this constraint are:
 * 			- Interoperations of Buffer, Display or Image only bound on ColorType not on Size. This keeps the memory footprint low.
 *			- Any Image Consumer is genuinly compatibility with:
 * 				- DMA based hardware acceleration
 * 					- DMA can at least accelerate copies of same ColorType
 * 					- DMA2D can also trim, convert and combine Images on copy
 * 				- Memory friendly implementation of simple decoders like run length encoding
 *
 * 			When implementing png or jpg decoders however, the design needs to be extended.
 *
 */
template<color::Color, Encoding>
class Image;

}  // namespace modm::graphic


#include <modm/graphic/color/gray.hpp>
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