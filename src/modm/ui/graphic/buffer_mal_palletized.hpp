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
template<color::ColorPalletized C, Size R>
class BufferMal<C, R> : public BufferInterface<C>, public Canvas<C, R>
{
public:
	using TPallete = C::TPallete;

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
	// TODO check if it works with uint16_t and uint32_t
	static constexpr int digitsPallete = std::numeric_limits<TPallete>::digits;
	static constexpr TPallete allBits = std::numeric_limits<TPallete>::max();

	static constexpr int ppb = digitsPallete / C::Digits;
	static constexpr std::size_t RY = (R.height() + digitsPallete - 1) / ppb;

	union {
		TPallete buffer[RY][R.width()];
		// algorithms dislike 2 dimensional arrays
		TPallete buffer_1d[RY * R.width()];
	};

	TPallete clearValue(C color = 0) const {
		return color::GrayT<TPallete>(color).value();
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
	{ return (y * C::Digits) & (digitsPallete - 1); } // x & (digitsPallete - 1) coresponds x % digitsPallete with support for negative int

	TPallete&
	getByte(const shape::Point& point)
	{ return buffer[getY(point.y())][point.x()]; }

	TPallete
	getByte(const shape::Point& point) const
	{ return buffer[getY(point.y())][point.x()]; }

	/**
	 * @brief 	Calculates a ton of constants for
	 * 			looping palletized 2D memories (images)
	 */
	struct Looper {
		const std::size_t yb_top, yb_bot;
		const int lshift_top, lshift_bot;
		const TPallete keepmask_top, keepmask_bot;

		// Maybe this is the beginning of a general service-class
		// including the // Top End, Middle part, Bottom end code-blocks in various methods of BufferMal<C, R>
		Looper(const shape::Section section) :
			yb_top(getY(section.topLeft.y())),
			yb_bot(getY(section.bottomRight.y() - 1)),
			lshift_top(getYlshift(section.topLeft.y())),
			lshift_bot(getYlshift(section.bottomRight.y()) ? getYlshift(section.bottomRight.y()) : digitsPallete),
			keepmask_top(~(allBits << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | allBits << lshift_bot : allBits << lshift_bot)
		{}

		Looper(const shape::VLine vline) :
			yb_top(getY(vline.start.y())),
			yb_bot(getY(vline.end_y - 1)),
			lshift_top(getYlshift(vline.start.y())),
			lshift_bot(getYlshift(vline.end_y) ? getYlshift(vline.end_y) : digitsPallete),
			keepmask_top(~(allBits << lshift_top)),
			keepmask_bot(yb_top == yb_bot ? keepmask_top | allBits << lshift_bot : allBits << lshift_bot)
		{}
	};

	template<color::Color CO, template<typename> class Accessor>
	TPallete
	palletizeByte(ImageAccessor<CO, Accessor>& accessor, int lshift, const int lshift_max) {
		TPallete byte(0);

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
	friend class BufferMal;
};
}  // namespace modm

#include "buffer_mal_palletized_impl.hpp"