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

#include <concepts>
#include <algorithm>

#include <modm/math/geometry/shape.hpp>
#include <modm/ui/color/concepts.hpp>
#include "concepts.hpp"
#include "accessor_image.hpp"

namespace modm::graphic
{
	/**
	 * @brief 			Memory Abstraction Layer (MAL) for generic graphic::Buffer.
	 * 					There's a variant for planar and one for palletized ColorTypes.
	 * 
	 * 					There's also an optimized variant for palletized Monochrome ColorType.
	 * 					Changing a Monochrome pixels value does not require masking out the old value.
	 * 					In other words, manipulating binary (Monochrome) data is a feature of the CPU.
	 */
	template<class, shape::Size>
	class BufferMal;
}

#include "buffer_mal_planar.hpp"
#include "buffer_mal_palletized.hpp"
#include "buffer_mal_palletized_mono.hpp"