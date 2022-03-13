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
#include <modm/ui/color/concepts.hpp>

namespace modm::color {

	/**
	 * @brief 	Concept to ident palettizing colortype instances
	 *			Targets: Monochrome, Gray2, Gray4
	 *
	 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 */
	template<class C>
	concept ColorPalletized = ColorGray<C> and C::Digits < 8 and std::popcount(unsigned(C::Digits)) == 1;

	/**
	 * @brief 	Concept to ident planar colortype instances
	 *
	 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 */
	template<class C>
	concept ColorPlanar = !ColorPalletized<C>;

	/**
	 * @brief 	Concept to ident monochrome colortype
	 */
	template<class C>
	concept ColorMonochrome = std::is_same_v<C, Monochrome>;
	// concept ColorMonochrome = ColorGray<C> and C::Digits == 1; // Alternative implementation
}

namespace modm::graphic
{
	template<class, Size>
	class BufferMemory;
}

#include "buffer_memory_planar.hpp"
#include "buffer_memory_palletized.hpp"
// TODO
// #include "buffer_memory_palletized_mono.hpp"