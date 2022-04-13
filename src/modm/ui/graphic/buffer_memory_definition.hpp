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

#include <modm/utils/memory/pallete.hpp>
#include <modm/utils/memory/array2d.hpp>

namespace modm::graphic {
	
	/// Specialisation of modm::pallete to use for 'palletes of Color' in graphic Buffers.
	template<color::ColorPalletized C, std::unsigned_integral TP, Dimension D = Row>
	class ColorPallete : public pallete<TP, C::digits, C> {
	public:
		static constexpr Dimension Dim = D;
		using ColorType = C;
		using pixel_accessor = pallete_element_accessor<TP, ColorType>; 

		// constructor
		using pallete<TP, C::digits, C>::pallete;
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

	template<class T, Dimension D = Row>
	struct BufferMemoryDefinition;

	template<color::ColorPlanar C, Dimension D>
	struct BufferMemoryDefinition<C, D> {
		using ColorType = C;
		static constexpr Dimension Major = D;
		static constexpr Dimension Minor = D == Row ? Col : Row;
	};

	template<class CP, Dimension D>
	requires color::ColorPalletized<typename CP::ColorType>
	struct BufferMemoryDefinition<CP, D>
	{
		using PalleteType = CP;
		using ColorType = CP::ColorType;

		static constexpr Dimension Major = D;
		static constexpr Dimension Minor = Major == Row ? Col : Row;

		/// Helper for array size calculation
		static consteval std::size_t
		arrSize(uint16_t length)
		{ return (length + CP::size - 1) / CP::size; };

		/// Helper for array index calculation
/* 		static std::size_t
		arrEnd(uint16_t offset) const
		{ return offset + 1 / CP::size; } */
	};

	template<class BMD>
	concept BufMemDef = true; // TODO BufMemDef concept
}