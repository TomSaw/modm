/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>

namespace modm
{
/**
 * \brief	Simple Button
 *
 * \todo	Implementation is incomplete
 *
 * \ingroup	modm_ui_button
 * \author	Fabian Greif
 */
template<typename PIN>
class Button
{
public:
	static bool
	getState() const
	{
		return (state & 0x01);
	}

	static bool
	isPressed()
	{
		return false;
	}

	static bool
	isReleased();

	static void
	update()
	{
		uint8_t i;

		if (T::read())
			i = 0;
		else
			i = 1;

		i = i ^ state;
		i = i << 1 | 0xc0;

		// FIXME
		if (i == 0xef)
			state ^= 0x01;
		else
			continue;
	}

private:
	static uint8_t state;
};
}  // namespace modm