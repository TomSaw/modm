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
#include <modm/math/geometry/section.hpp>

namespace modm::graphic
{

enum
// enum class
OrientationFlags : uint8_t
{
	Portrait = Bit0,
	TopDown = Bit1
};
// TODO implement modm typesafe registers for ssd1306 and ili9431
// MODM_FLAGS8(OrientationFlags);

enum
// enum class
Orientation : uint8_t
{
	Landscape0,
	Portrait90 = Portrait,
	Landscape180 = TopDown,
	Portrait270 = Portrait | TopDown
};

/**
 * Baseclass for Graphic Display
 *
 * @tparam	R			Resolution - R.width(): horizontal, R.height(): vertical
 *
 * @author				Thomas Sommer
 * @ingroup				modm_ui_graphic
 */

// TODO Replace <.., bool CanPortrait> with <.., Orientation Supported>
// so constexpr and runtime getWidth() / getHeight() can be enabled more cleverly
template <Size R>
class Display
{
public:
	Orientation getOrientation() const
	{ return orientation; }

	int16_t
	getWidth() const
	{ return (orientation & Orientation(OrientationFlags::Portrait)) ? R.height() : R.width(); }

	int16_t
	getHeight() const
	{ return (orientation & Orientation(OrientationFlags::Portrait)) ? R.width() : R.height(); }

	Size
	getSize() const
	{ return (orientation & Orientation(OrientationFlags::Portrait)) ? R.swapped() : R; }

protected:
	Display() = default;

	Orientation orientation{Orientation::Landscape0};

	// Static variables for Resumable Functions
	Section clipping;
};

} // namespace modm