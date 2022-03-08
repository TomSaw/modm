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

#pragma once
#include "accessor_image.hpp"

#include <modm/math/geometry/shape/point.hpp>
#include <modm/ui/color/concepts.hpp>

namespace modm::graphic
{

/**
 * @brief 				Sequential accessor for palettized stored image with clipping support
 *
* @tparam C 			Palletized Colortype of image to access
 * @tparam Accessor 	Any of modm::accessor::*
 *
 * @author				Thomas Sommer
 * @ingroup				modm_ui_graphic
 */
template<color::ColorPalletized C, template<typename> class Accessor>
class ImageAccessor<C, Accessor> : public Accessor<typename C::PalleteType>
{
public:
	static constexpr int digits = std::numeric_limits<typename C::PalleteType>::digits;

	ImageAccessor() = default;

	ImageAccessor(const BufferInterface<C>* buffer, shape::Point placement = {0, 0})
		: Accessor<typename C::PalleteType>((typename C::PalleteType*)(buffer->virtualBuffer())), section(placement, placement + buffer->virtualSize())
	{
		col_size = buffer->virtualSize().x();
		initialize(placement);
	}

	ImageAccessor(const uint8_t* addr, shape::Point placement = {0, 0})
		: Accessor<typename C::PalleteType>((typename C::PalleteType*)(addr + 2))
	{
		auto flash = modm::accessor::asFlash(addr);
		section = {placement, placement + shape::Point(flash[0], flash[1])};
		col_size = flash[0];
		initialize(placement);
	}

	ImageAccessor(const uint8_t* addr, shape::Point size, shape::Point placement)
		: Accessor<typename C::PalleteType>((typename C::PalleteType*)(addr)), section(placement, placement + size), col_size(size.x())
	{
		initialize(placement);
	}

	shape::Section getSection() const
	{ return section; }

	void incrementRow()
	{ this->address = addr_top++; }

	void incrementCol()
	{ this->address += col_size; }

	void
	incrementRow_preparePixel()
	{
		incrementRow();
		// Reset reading head
		rotr = rotr_top;
		byte = std::rotr(Accessor<typename C::PalleteType>::operator*(), rotr);
	}

	C
	nextPixel()
	{
		rotr += C::Digits;
 		if (rotr == digits)
		{
			rotr = 0;
			incrementCol();
			// load next byte
			byte = Accessor<typename C::PalleteType>::operator*();
		} else {
			// rotate to next pixel
			byte = std::rotr(byte, C::Digits);
		}

		return byte & C::max;
	}
private:
	void
	initialize(const shape::Point placement)
	{
		const shape::Point topLeft = {placement.x() < 0 ? -placement.x() : 0, placement.y() < 0 ? -placement.y() : 0};

		addr_top = this->address + topLeft.y() * C::Digits / digits + topLeft.x() * col_size;
		// & (digits - 1) coresponds x % digits with support for negative int
		rotr_top = ((topLeft.y() * C::Digits) & (digits - 1)) - C::Digits;
	}

	int
	getYlshift(int16_t y) const
	{ return (y * C::Digits) % digits; }

	// Image properties
	shape::Section section;
	size_t col_size;

	// Start conditions for new row
	const C::PalleteType* addr_top;
	int rotr_top;

	// Current byte
	C::PalleteType byte;
	// Track current rotation
	int rotr;
};
}  // namespace modm::graphic