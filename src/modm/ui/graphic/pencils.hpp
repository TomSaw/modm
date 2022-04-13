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

#include "concepts.hpp"

namespace modm::graphic::pencil {

struct Pencil {
	uint16_t step{0};
};

template<color::Color C>
struct Dotted : Pencil {
	const C active, passive;
	const uint16_t length;
	
	Dotted(C active, C passive, uint16_t length = 1)
		: active(active), passive(passive), length(length)
	{}

	C operator()(void) {
		// OPTIMIZE benchmark if postincr is faster (as expected)
		// ++step;
		return step++ % (length * 2) < length ? active : passive;
	}

};

} // namespace modm::graphic