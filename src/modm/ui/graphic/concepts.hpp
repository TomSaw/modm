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

#include <modm/ui/color/concepts.hpp>
#include <modm/ui/color/gray.hpp>
#include <modm/math/geometry/point.hpp>

namespace modm::color {
	/**
	 * @brief 		Concepts to distinguish color storage types.
	 * 				Required for Memory management and manipulatoin algorithms.
	 *
	 * @see:		https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 * @author		Thomas Sommer
	 * 
	 * @ingroup		modm_ui_color
	 * @ingroup		modm_ui_graphic
	 */
	template<class C>
	concept ColorPalletized = ColorGray<C> and C::digits < 8 and std::popcount(unsigned(C::digits)) == 1;

	template<class C>
	concept ColorPlanar = Color<C> && !ColorPalletized<C>;

	template<class C>
	concept ColorMonochrome = std::is_same_v<C, Monochrome>;
	// concept ColorMonochrome = ColorGray<C> and C::digits == 1; // Alternative implementation
}
namespace modm::graphic
{


// DEPRICATED
template<class B>
concept GraphicBuffer = color::Color<typename B::ColorType>;
// TODO check B for existence of Size

template<class D>
concept GraphicDisplay = color::Color<typename D::ColorType>;
// TODO check D for existence of Size

/**
 * @brief 	Concept for lambda / functor that emitts a color::Color on each call
 * 
 */
template<class CF>
concept ColorPencil = requires (CF cf)
{
	{ cf() } -> color::Color;
};

/**
 * @brief 	Concept for lambda / functor that emits a color::Color for any Point
 *
 */
template<class CP>
concept ColorPattern = requires(CP cp)
{
	{ cp(Point()) } -> color::Color;
};

}  // namespace modm::graphic