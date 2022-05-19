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

#include "rgb.hpp"
#include <modm/math/utils/bit_constants.hpp>

/// @ingroup     modm_ui_color
namespace modm::color {
	/**
	 * @brief 	This is a colortype for plain unmixed colors. It only uses one byte and will be
	 * 			used for simple on-off LEDs or if storing a plain color is sufficient for your application.
	 * 
	 */
	class Plain {
	public:
		enum Color : uint8_t {
			Red = Bit0,
			Green = Bit1,
			Blue = Bit2,

			Cyan = Bit3,
			Magenta = Bit4,
			Yellow = Bit5,
			
			Black = Bit6,
			White = Bit7,
		};

		constexpr Plain(Color value)
			: value(value)
		{}

		template<class C>
		requires ColorRgb<C> || ColorRgbPallete<C>
		constexpr Plain(const C& other)
		{
			// TODO
		}

		template<ColorRgb C>
		constexpr operator C()
		{
			// TODO
		}

		template<ColorRgbPallete C>
		constexpr operator C()
		{
			// TODO
		}

	private:
		Color value;
	};
} // namespace modm::color::enum