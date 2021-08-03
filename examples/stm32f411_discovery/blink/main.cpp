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

#include <modm/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	LedGreen::set();
	LedRed::set();

	while (true)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		LedOrange::toggle();
		LedRed::toggle();
		modm::delay(Button::read() ? 250ms : 500ms);
	}
}
