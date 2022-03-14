/*
 * Copyright (c) 2019, Mike Wolfram
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <concepts>

#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/processing/resumable.hpp>

#include <modm/ui/graphic/display.hpp>
#include <modm/ui/graphic/buffer.hpp>
#include <modm/ui/color/rgb_stacked.hpp>
#include <modm/math/geometry/shape/point.hpp>
#include <modm/math/geometry/shape/line.hpp>
#include <modm/math/geometry/shape/section.hpp>
#include <modm/ui/graphic/style.hpp>

#include "ili9341_parallel.hpp"
#include "ili9341_spi.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341

template<class Transport, class Reset, size_t BC = 512>
class Ili9341 : public Transport, public graphic::Display<color::Rgb565, {320, 240}, true>
{
	// OPTIMIZE determine good contraints
	static_assert(BC >= 32, "Conversion Buffer < 64 pixels produces too much overhead.");
	static_assert(BC <= 2048, "Conversion Buffer > 2048 pixels doesn't make it any better.");

	using Toggle = ili9341_register::Toggle;
	using ReadWrite = ili9341_register::ReadWrite;
	using Command = ili9341_register::Command;
	using ReadCommand = ili9341_register::ReadCommand;

public:
	using ColorType = color::Rgb565;

	template<color::Color = ColorType>
	using BufferLandscape = graphic::Buffer<ColorType, {320, 240}>;
	
	template<color::Color = ColorType>
	using BufferPortrait = graphic::Buffer<ColorType, {240, 320}>;

	template<typename... Args>
	Ili9341(Args &&...args)
		: Transport(std::forward<Args>(args)...)
	{ Reset::setOutput(modm::Gpio::High); }

	~Ili9341(){};

	ResumableResult<void>
	initialize();

	ResumableResult<void>
	reset(bool hardReset = false);

	ResumableResult<uint16_t>
	getIcModel();

	/* ResumableResult<uint32_t>
	getStatus(); // 5 bytes, Datasheet P92

	ResumableResult<uint16_t>
	getPowerMode(); // 2 bytes, Datasheet P94

	ResumableResult<uint16_t>
	getMadCtl(); // 2 bytes, Datasheet P95

	ResumableResult<uint16_t>
	getPixelFormat(); // 2 bytes, Datasheet P96 */

	ResumableResult<void>
	set(Toggle toggle, bool state);

	ResumableResult<void>
	set(ReadWrite reg, uint8_t value);

	ResumableResult<uint8_t>
	get(ReadWrite reg);

	ResumableResult<void>
	setOrientation(graphic::Orientation orientation);

	ResumableResult<void>
	setScrollArea(uint16_t topFixedRows, uint16_t firstRow, uint16_t bottomFixedRows);

	ResumableResult<void>
	scrollTo(uint16_t row);

	template<graphic::ColorPattern P>
	ResumableResult<void>
	writePattern(shape::Rectangle rectangle, P pattern);

	ResumableResult<void>
	clear(ColorType color = color::html::Black);
private:
	// Static variables for resumable functions
	shape::Section section;
	shape::Point cursor;
	// ##################################################################

protected:
	ResumableResult<void>
	updateClipping();

	ResumableResult<void>
	setClipping(shape::Point point);

	/**
	 * Write Image with same Color
	 * Transmits an image as is.
	 *
	 * @param accessor	ImageAccessor with underlying Flash or Ram Accessor
	 * @param placement	Placement for the image
	 */
	template<template<typename> class Accessor>
	ResumableResult<void>
	writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor);

	/**
	 * Write Image with foreign Color
	 * Uses local buffer for color conversion before transmission.
	 *
	 * @param accessor	ImageAccessor with underlying Flash or Ram Accessor
	 * @param placement	Placement for the image
	 */
 	template<color::Color CO, template<typename> class Accessor>
	ResumableResult<void>
	writeImage(graphic::ImageAccessor<CO, Accessor> accessor);

	ResumableResult<void> drawBlind(const shape::Point& point);
	ResumableResult<void> drawBlind(const shape::Section& section);
	ResumableResult<void> drawBlind(const shape::HLine& hline);
	ResumableResult<void> drawBlind(const shape::VLine& vline);

	ResumableResult<ColorType>
	getBlind(const shape::Point& point) const;

private:
	// Static variables for resumable functions
	// OPTIMIZE Closing check if the union is well formed
	union {
		// Buffers for commands & configuration
		uint8_t buff_cmd8[15];
		uint16_t buff_cmd16[3];
		ili9341_register::MemoryAccessCtrl_t madCtrl;
		modm::ShortTimeout timeout;

		// Parallel use in resumable function: Don't overlap !
		struct
		{
			uint16_t buff_cmd_clipping[2];

			ColorType buffer[BC]; // Conversion buffer
			size_t i; // index in conversion buffer
			shape::Point scanner; // index on display

			uint64_t pixels; // Must fit R.x() * R.y() = 76800
			size_t pixels_bulk; // Number of pixels of current bulk

			ColorType temp_color; // Temporary storage for a color
		} p;  // p for parallel
	};
};
}  // namespace modm

#include "ili9341_impl.hpp"