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

#include "point.hpp"

namespace modm::shape {
	
	using scalar_t = int16_t;
	using radius_t = uint16_t;

	struct Line {
		const Point delta;
	};

	struct HLine {
		const scalar_t delta;
	};

	struct VLine {
		const scalar_t delta;
	};

	struct DLine {
		const scalar_t delta;
		const int dir;
	};
	
	struct Rectangle {
		const Point delta;
	};

	struct Circle {
		const radius_t radius;
	};

	struct Ellipse {
		const radius_t radius;
		const radius_t radius_2;
	};

	// struct Bow {
	// 	const radius_t radius;
	// };
}
