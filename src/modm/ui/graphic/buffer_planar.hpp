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
class BufferBase<BMD, R> : public array2d<typename BMD::ColorType, R.width(), R.height(), BMD::Major>
{
public:
	using CursorType = Cursor<typename BMD::ColorType, BMD::Major, (BMD::Major == X) ? R.width() : R.height()>;

protected:
	using array2dT = array2d<typename BMD::ColorType, R.width(), R.height(), BMD::Major>;

	// constructors
	using array2dT::array2d;

public:
	auto xspan(std::size_t col)
	{ return array2dT::rowspan(col); }

	auto yspan(std::size_t col)
	{ return array2dT::colspan(col); }
};
}  // namespace modm::graphic