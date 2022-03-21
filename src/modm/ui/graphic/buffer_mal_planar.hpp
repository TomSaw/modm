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
#include "buffer_mal.hpp"

#include <algorithm>
#include <modm/math/utils/misc.hpp>

namespace modm::graphic
{

/**
 * @brief 		Memory abstration Layer for Planar colors. Planar -> each pixel has it's own address.
 *
 * @tparam	C	color::Rgb, color::Hsv, color::RgbStacked, color::Gray{>= 8} - (to be expanded in the future)
 * @tparam	R	Resolution - R.width(): horizontal, R.height(): vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template<color::ColorPlanar C, shape::Size R>
class BufferMal<C, R> : public BufferInterface<C>, public Canvas<C, R>
{
public:
	BufferMal() = default;
	
	template<color::ColorPlanar CO>
	constexpr BufferMal(const BufferMal<CO, R> &other)
	{
		std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
	}

	template<color::ColorPalletized CO>
	constexpr BufferMal(const BufferMal<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	template<color::ColorPlanar CO>
	void operator=(const BufferMal<CO, R> &other)
	{
		std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
	}

	template<color::ColorPalletized CO>
	void operator=(const BufferMal<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

protected:
	union {
		C buffer[R.height()][R.width()];
		// algorithms dislike 2 dimensional arrays
		C buffer_1d[R.height() * R.width()];
	};

	C::T clearValue(C color = 0) const {
		return color.value();
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
	 * @brief 				Write Image with same C
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

	C getBlind(const shape::Point& point) const
	{
		return buffer[point.y()][point.x()];
	}

	C& operator[](const shape::Point& point)
	{
		return buffer[point.y()][point.x()];
	}

	template<class, shape::Size>
	friend class BufferMal;
};
}  // namespace modm

#include "buffer_mal_planar_impl.hpp"