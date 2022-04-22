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

#include <modm/utils/pallete.hpp>
#include <modm/math/geometry/dimension.hpp>
#include <modm/graphic/color/concepts.hpp>

namespace modm::graphic {

	/**
	 * @brief 		Concepts to distinguish color storage types.
	 * 				Required for Memory management and manipulatoin algorithms.
	 *
	 * @see:		https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 * 
	 * @author		Thomas Sommer
	 * @ingroup		modm_ui_color
	 * @ingroup		modm_ui_graphic
	 */
	template<class C>
	concept ColorPalletized = color::ColorGray<C> and C::digits < 8 and std::popcount(unsigned(C::digits)) == 1;

	template<class C>
	concept ColorPlanar = color::Color<C> && !ColorPalletized<C>;

	template<class C>
	concept ColorMonochrome = color::ColorGray<C> and C::digits == 1;
	// concept ColorMonochrome = std::is_same_v<C, color::Monochrome>;
	
	/**
	 * @brief 		Specialised modm::pallete for use in graphic Buffer
	 * 
	 */
	template<ColorPalletized C, std::unsigned_integral TP, Dimension D>
	class ColorPallete : public pallete<TP, C::digits, C> {
	public:
		static constexpr Dimension Dim = D;
		using ColorType = C;
		using pixel_accessor = pallete_element_accessor<TP, ColorType>; 

		// constructor
		using pallete<TP, C::digits, C>::pallete;

		void invert() { this->value_ ^= this->bitmask_pallete; }
	};


	/**
	 * @brief 		Specifies how the memory for a Display needs to be formatted.
	 * 				This allows to transfer composed buffers to the consumer (Display) directly via DMA.
	 * 				One or more definitions of BufferLayout should placed within each rasterized display
	 * 				driver according to the 'memory layout' chapter found in the displays Datasheet.
	 * 
	 * @tparam T	Type of addressable data: ColorPlain or ColorPallete
	 * @tparam D 	Major Dimension for 2d-array container
	 */
	template<class T, Dimension D = X>
	struct GddramLayout;

	template<ColorPlanar C, Dimension D>
	struct GddramLayout<C, D> {
		using ColorType = C;
		static constexpr Dimension Major = D;
	};

	template<class CP, Dimension D>
	requires ColorPalletized<typename CP::ColorType>
	struct GddramLayout<CP, D>
	{
		using PalleteType = CP;
		using ColorType = CP::ColorType;
		static constexpr Dimension Major = D;

		/// Helper functions
		static consteval std::size_t
		arrSize(uint16_t length)
		{ return (length + CP::size - 1) / CP::size; };
	};

	template<class GDDRAM>
	concept Gddram = true; // TODO Gddram concept
}