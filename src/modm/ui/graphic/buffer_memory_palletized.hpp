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
#include "buffer.hpp"

#include <algorithm>
#include <concepts>

#include <modm/ui/color/gray.hpp>

namespace modm::graphic
{

template<color::Color, Size>
class Buffer;

/**
 * @brief		Framebuffer with random access and low-lvl drawing methods. Multiple pixels share one address
 *
 * @tparam	C	color::Monochrome, color::Gray2 or color::Gray4 - to be expanded in the future
 * @tparam	R	Resolution - R.x(): horizontal, R.y(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template<color::ColorPalletized C, Size R>
class BufferMemory<C, R> : public BufferInterface<C>, public Canvas<C, R>
{
protected:
	using PalleteType = C::PalleteType;

	// TODO check if it works with uint16_t and uint32_t
	static constexpr int digits = std::numeric_limits<PalleteType>::digits;
	static constexpr PalleteType allBits = std::numeric_limits<PalleteType>::max();

	static constexpr int ppb = digits / C::Digits;
	static constexpr std::size_t RY = (R.y() + digits - 1) / ppb;

	union {
		PalleteType buffer[RY][R.x()];
		PalleteType buffer_1d[RY * R.x()];
	};

	PalleteType clearValue(C color = 0) const {
		return color::GrayD<digits>(color).value();
	}

	// ------------------------------------------------------------------

	BufferMemory(C* colormap) : Canvas<C, R>(colormap) {};

	template<color::Color CO>
	constexpr BufferMemory(const BufferMemory<CO, R>& other) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	template<color::Color CO>
	void operator=(const BufferMemory<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	/**
	 * @brief				Write Image with foreign Color
	 *
	 * @param accessor		ImageAccessor inheriting an accessor::Flash or accessor::Ram
	 * @param placement		Placement for the image
	 */
	template<color::Color CO, template<typename> class Accessor>
	void
	writeImage(ImageAccessor<CO, Accessor> accessor);

	/**
	 * @brief				Write Image with same Color
	 *
	 * @param accessor		ImageAccessor inheriting an accessor::Flash or accessor::Ram
	 * @param placement		Placement for the image
	 */
 	template<template<typename> class Accessor>
	void
	writeImage(ImageAccessor<C, Accessor> accessor);

	/**
	 * @warning 	These methods do not check sanity!
	 *				It is your responsibility to check if the shape
	 *				is within buffer boundaries.
	 */
	void drawBlind(const shape::Point& point);
	void drawBlind(const shape::HLine& hline);
	void drawBlind(const shape::VLine& vline);
	void drawBlind(const shape::Section& section);

	C getBlind(const shape::Point& point) const;

private:
	static std::size_t
	getY(int_fast16_t y)
	{ return y / ppb; }

	static int
	getYlshift(int_fast16_t y)
	{ return (y * C::Digits) & (digits - 1); } // x & (digits - 1) coresponds x % digits with support for negative int

	PalleteType&
	getByte(const shape::Point& point)
	{ return buffer[getY(point.y())][point.x()]; }

	PalleteType
	getByte(const shape::Point& point) const
	{ return buffer[getY(point.y())][point.x()]; }

	/**
	 * @brief 	Calculates a ton of constants for
	 * 			looping palletized 2D memories (images)
	 */
	struct Looper {
		const std::size_t yb_top, yb_bot;
		const int lshift_top, lshift_bot;
		const PalleteType keepmask_top, keepmask_bot;

		// Maybe this is the beginning of a general service-class
		// including the // Top End, Middle part, Bottom end code-blocks in various methods of BufferMemory<C, R>
		Looper(const shape::Section section) :
			yb_top(getY(section.topLeft.y())),
			yb_bot(getY(section.bottomRight.y() - 1)),
			lshift_top(getYlshift(section.topLeft.y())),
			lshift_bot(getYlshift(section.bottomRight.y()) ? getYlshift(section.bottomRight.y()) : digits),
			keepmask_top(~(allBits << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | allBits << lshift_bot : allBits << lshift_bot)
		{}

		Looper(const shape::VLine vline) :
			yb_top(getY(vline.start.y())),
			yb_bot(getY(vline.end_y - 1)),
			lshift_top(getYlshift(vline.start.y())),
			lshift_bot(getYlshift(vline.end_y) ? getYlshift(vline.end_y) : digits),
			keepmask_top(~(allBits << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | allBits << lshift_bot : allBits << lshift_bot)
		{}
	};

	// OPTIMIZE confirm "inline" doesn't help
	template<color::Color CO, template<typename> class Accessor>
	PalleteType
	palletizeByte(ImageAccessor<CO, Accessor>& accessor, int lshift, const int lshift_max) {
		PalleteType byte(0);

		while(lshift < lshift_max) {
			if constexpr (color::ColorPalletized<CO>) {
				// Apply colormap
				byte |= this->colormap[ accessor.nextPixel().value() ].value() << lshift;
			} else {
				// Convert color
				byte |= C( accessor.nextPixel() ).value() << lshift;
			}
			lshift += C::Digits;
		}
		return byte;
	}

	template<class, Size>
	friend class BufferMemory;
};
}  // namespace modm

#include "buffer_memory_palletized_impl.hpp"
#include "buffer_memory_palletized_draw_impl.hpp"