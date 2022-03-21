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

#include <bit>

#include <modm/architecture/interface/accessor.hpp>
#include <modm/math/geometry/shape/point.hpp>
#include <modm/math/utils/bit_constants.hpp>
#include <modm/ui/color/concepts.hpp>

namespace modm::graphic {
	/**
	 * @brief 			Sequential Accessor for images in RAM, Flash or SD-Card (later not yet implemented)
	 * 					Some image compressions or storages are very inefficient for random access.
	 * 					Also to fulfill the purpose of sequential reading an image, random access is not required.
	 * 					By consequently restricting the ImageAccessor to sequential access, intercompatibility
	 * 					is guaranteed.
	 */

	// TODO maybe renamed to simple 'Image' + moved to ui/image
	// 1. Ram and Flash don't have 'Accessor' in it's name
	// 2. In accordance to Font
	template<class, template<typename> class>
	class ImageAccessor;
}

#include "accessor_image_planar.hpp"
#include "accessor_image_palletized.hpp"
// #include "accessor_image_runlength.hpp"