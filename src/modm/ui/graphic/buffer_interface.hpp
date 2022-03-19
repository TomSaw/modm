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

#include <tuple>
#include <modm/ui/color/concepts.hpp>
#include <modm/math/geometry/shape/point.hpp>
#include "canvas.hpp"

namespace modm::graphic
{

/**
 * @brief 		Interface to Buffer to serve graphic::ImageAccessor
 *
 * @tparam C 	ColorType of the Interface.
 * 				A consumer of BufferInterface (other Buffers or Displays) variate their methods
 * 				over C cause is critical for performance.
 * 				shape::Size and the actual address of the Buffer is decoupled: It's cheap enough using a virtual call.
 */

template<color::Color C>
class BufferInterface
{
public:
	// OPTIMIZE Maybe returned with one call using std::tuple
	virtual shape::Size
	virtualSize() const = 0;

	virtual const void*
	virtualBuffer() const = 0;

	virtual std::tuple<shape::Size, const void*>
	getSizeAndBuffer() const = 0;
protected:
	BufferInterface() = default;
};
}  // namespace modm