// TODO Reconstruct the copyrights
/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2011, 2013, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Niclas Rohrer
 *
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

#include <algorithm>

#include <modm/math/geometry/shape/point.hpp>
#include <modm/ui/color/concepts.hpp>
#include <modm/ui/font/fixed_width_5x8.hpp>

#include "buffer_interface.hpp"
#include "buffer_memory.hpp"

#include "accessor_image.hpp"
#include "font.hpp"

namespace modm::graphic
{
template<color::Color C, Size R>
class Buffer : public BufferMemory<C, R>, public IOStream
{
public:
	using ColorType = C;

	Buffer(C* colormap, Font* font)
		: BufferMemory<C, R>(colormap), IOStream(writer), font(font), writer(this)
	{}

	/// Same Color and Size: use std::copy or DMA
	constexpr Buffer(const Buffer &other)
	{
		/* this->colormap = other.colormap;
		this->font = other.font; */

		// OPTIMIZE add DMA support
		std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
	}

	Buffer& operator=(const Buffer &other)
	{
		if(this != &other) {
			/* this->colormap = other.colormap;
			this->font = other.font; */

			// OPTIMIZE add DMA support
			std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
		}

		return *this;
	}

	constexpr Buffer(Buffer &&other)
	{
		/* this->colormap = other.colormap;
		this->font = other.font; */

		this->buffer = other.buffer;
	}

	/// Different color but same size: deligate copying to BufferMem
 	template<color::Color CO>
	constexpr Buffer(const Buffer<CO, R> &other) : BufferMemory<C, R>(other), IOStream(writer), writer(this)
	{
		this->colormap = other.colormap;
		font = other.font;
	}

	template<color::Color CO>
	Buffer& operator=(const Buffer<CO, R> &other)
	{
		this->colormap = other.colormap;
		this->font = other.font;

		BufferMemory<C, R>::operator=(other);
		return *this;
	}

	/// Different color and size: Use writeImage cause handles Intersection
	template<color::Color CO, Size RO>
	constexpr Buffer(const Buffer<CO, RO> &other) : IOStream(writer), writer(this)
	{
		this->colormap = other.colormap;
		this->font = other.font;

		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	/**
	 * @brief 			Writes another buffer to this buffer. If the other buffer is palettized,
	 * 					pixels will be mapped using the colormap.
	 *
	 * @tparam CO		Colortype of other buffer
	 *
	 * @param buffer 	Other buffer
	 * @param placement Target position for other buffer
	 */
	template<color::Color CO>
	void write(const BufferInterface<CO> &buffer, shape::Point placement) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&buffer, placement));
	}

	template<color::Color CO>
	void write(const BufferInterface<CO> &buffer) {
		// no placement passed? take a shortcut
		this->operator=(buffer);
	}

	/**
	 * @brief 			Writes an image from flash to this buffer. If the image is palettized,
	 * 					pixels will be mapped using the colormap.
	 *
	 * @tparam CO		Colortype of image
	 *
	 * @param buffer 	Address of the image in flash
	 * @param placement 	Target position for image
	 */
	template<color::Color CO>
	void write(const uint8_t *addr, shape::Point placement = {0, 0}) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Flash>(addr, placement));
	}

	void clear(ColorType color = 0);

	void invert();

	// Idea for vertical and horizontal shift with <<= and >>=
	// https://stackoverflow.com/questions/486099/can-inner-classes-access-private-variables
 		void operator<<=(const std::size_t shift);
		void operator>>=(const std::size_t shift);

	// TODO Maybe possible to get this typesafe
	const uint8_t* virtualBuffer() const final
	{ return (uint8_t*)(this->buffer); }

	Size virtualSize() const final
	{ return R; }

private:
	template<color::Color, Size>
	friend class Buffer;

/// IOStream
public:
	Font* font;
	shape::Point cursor;
	bool linebreak = true;

	void
	writeChar(char c);

private:
	void do_linebreak()
	{
		cursor.x() = 0;
		cursor.y() += font->getLineHeight();
	}

	class Writer : public IODevice
	{
		Buffer *parent;
	public:
		Writer(Buffer *parent) : parent(parent) {}

		virtual void
		write(char c)
		{ this->parent->writeChar(c); }

		using IODevice::write;

		// unused
		virtual void
		flush() {}

		// unused
		virtual bool
		read(char&)
		{ return false; }
	};

	Writer writer;
};
}  // namespace modm::graphic

#include "buffer_impl.hpp"