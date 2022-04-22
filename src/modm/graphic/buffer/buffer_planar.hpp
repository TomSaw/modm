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
#include <modm/utils/array2d.hpp>
#include <modm/utils/array2d_dma.hpp>

namespace modm::graphic::detail {

template<Gddram GDDRAM, Size R>
requires ColorPlanar<typename GDDRAM::ColorType>
class BufferBase<GDDRAM, R> : public array2d<typename GDDRAM::ColorType, R.width(), R.height(), GDDRAM::Major>
{
protected:
	using array2dT = array2d<typename GDDRAM::ColorType, R.width(), R.height(), GDDRAM::Major>;

	// constructors
	using array2dT::array2d;

public:
	using CursorType = Cursor<typename GDDRAM::ColorType, GDDRAM::Major, (GDDRAM::Major == X) ? R.width() : R.height()>;

	auto xspan(std::size_t col)
	{ return array2dT::rowspan(col); }

	auto yspan(std::size_t col)
	{ return array2dT::colspan(col); }
};
}  // namespace modm::graphic