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
#include "buffer_mal.hpp"

#include <limits>
#include <modm/ui/color/gray.hpp>
#include <modm/math/utils/integer_pallete.hpp>

namespace modm::graphic
{

/**
 * @brief		Memory abstration Layer for Palettized colors. Palettized -> multiple pixels share one address
 *
 * @tparam	C	color::Monochrome, color::Gray2 or color::Gray4 - (to be expanded in the future)
 * @tparam	R	Resolution - R.width(): horizontal, R.height(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template<color::ColorPalletized C, shape::Size R>
class BufferMal<C, R> : public BufferInterface<C>, public Canvas<C, R>
{
public:
	using T = C::T;
	using Pallete = math::Pallete<T, C::Digits>;

	BufferMal() = default;

	template<color::Color CO>
	constexpr BufferMal(const BufferMal<CO, R>& other) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	template<color::Color CO>
	void operator=(const BufferMal<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

protected:
	static constexpr std::size_t RY = (R.height() + Pallete::digits - 1) / Pallete::elements;

	union {
		T buffer[RY][R.width()];
		// algorithms dislike 2 dimensional arrays
		T buffer_1d[RY * R.width()];
	};

	T clearValue(C color = 0) const {
		return color::GrayT<T>(color).value();
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
	T&
	getByte(const shape::Point& point)
	{ return buffer[Pallete::getOffset(point.y())][point.x()]; }

	T
	getByte(const shape::Point& point) const
	{ return buffer[Pallete::getOffset(point.y())][point.x()]; }

	/**
	 * @brief 	Calculates a ton of constants for
	 * 			looping palletized 2D memories (images)
	 */
	struct Looper {
		const std::size_t yb_top, yb_bot;

		const int lshift_top, lshift_bot;

		const T keepmask_top, keepmask_bot;

		// Maybe this is the beginning of a general service-class
		// including the // Top End, Middle part, Bottom end code-blocks in various methods of BufferMal<C, R>
		Looper(const shape::Section section) :
			yb_top(Pallete::getOffset(section.topLeft.y())),
			yb_bot(Pallete::getOffset(section.bottomRight.y() - 1)),
			lshift_top(Pallete::getLShift(section.topLeft.y())),
			lshift_bot(Pallete::getLShift(section.bottomRight.y()) ? Pallete::getLShift(section.bottomRight.y()) : Pallete::digits),

			keepmask_top(~(Pallete::bitmask << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | Pallete::bitmask << lshift_bot : Pallete::bitmask << lshift_bot)
		{}

		Looper(const shape::VLine vline) :
			yb_top(Pallete::getOffset(vline.start.y())),
			yb_bot(Pallete::getOffset(vline.end_y - 1)),
			lshift_top(Pallete::getLShift(vline.start.y())),
			lshift_bot(Pallete::getLShift(vline.end_y) ? Pallete::getLShift(vline.end_y) : Pallete::digits),

			keepmask_top(~(Pallete::bitmask << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | Pallete::bitmask << lshift_bot : Pallete::bitmask << lshift_bot)
		{}
	};

	template<color::Color CO, template<typename> class Accessor>
	T
	palletizeByte(ImageAccessor<CO, Accessor>& accessor, int lshift, const int lshift_max) {
		T byte(0);

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

	template<class, shape::Size>
	friend class BufferMal;
};
}  // namespace modm

#include "buffer_mal_palletized_impl.hpp"