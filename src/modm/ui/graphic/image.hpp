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
#include <modm/math/geometry/shape/size.hpp>
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
	const shape::Size size;

	ImageBase(T const *data, shape::Size size)
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

#include "image_uncompressed_planar.hpp"
#include "image_uncompressed_palletized.hpp"
#include "image_runlength.hpp"

#include "image_ostream.hpp"