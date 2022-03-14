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
#include "buffer_mal.hpp"

#include <modm/ui/color/gray.hpp>

namespace modm::graphic
{

/**
 * @brief		Memory abstration Layer for Palettized Monochrome colors. Palettized -> multiple pixels share one address
 *
 * @tparam	C	color::Monochrome
 * @tparam	R	Resolution - R.x(): horizontal, R.y(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
// IMPLEMENT buffer_mal_palletized_mono_impl
/* template<color::Monochrome C, Size R>
class BufferMal<C, R> : public BufferInterface<C>, public Canvas<C, R>
{

} */

} // namespace modm::graphic

// #include "buffer_mal_palletized_mono_impl.hpp"