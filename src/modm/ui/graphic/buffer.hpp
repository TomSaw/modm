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
#include "buffer_mal.hpp"

#include "accessor_image.hpp"
#include "font.hpp"

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>
#endif

namespace modm::graphic
{

#if __has_include(<modm/io/iostream.hpp>)
template<color::Color C, shape::Size R>
class Buffer : public BufferMal<C, R>, public IOStream
{
public:
	Buffer()
		: IOStream(writer), writer(this), font(nullptr)
	{}
#else
template<color::Color C, shape::Size R>
class Buffer : public BufferMal<C, R>
{
public:
#endif

	using ColorType = C;
	static constexpr shape::Size Resolution = R;

	/// Same Color and Size: use std::copy or DMA
	constexpr Buffer(const Buffer &other)
	{
		/* this->colormap = other.colormap;
		#if __has_include(<modm/io/iostream.hpp>)
		this->font = other.font;
		#endif */

		// OPTIMIZE add DMA support
		std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
	}

	Buffer& operator=(const Buffer &other)
	{
		if(this != &other) {
			/* this->colormap = other.colormap;
			#if __has_include(<modm/io/iostream.hpp>)
			this->font = other.font;
			#endif */

			// OPTIMIZE add DMA support
			std::copy(std::begin(other.buffer_1d), std::end(other.buffer_1d), std::begin(this->buffer_1d));
		}

		return *this;
	}

	constexpr Buffer(Buffer &&other)
	{
		/* this->colormap = other.colormap;
		#if __has_include(<modm/io/iostream.hpp>)
		this->font = other.font;
		#endif */

		this->buffer = other.buffer;
	}

	/// Different color but same size: deligate copying to BufferMem
 	template<color::Color CO>
	constexpr Buffer(const Buffer<CO, R> &other) : BufferMal<C, R>(other), IOStream(writer), writer(this)
	{
		this->colormap = other.colormap;
		#if __has_include(<modm/io/iostream.hpp>)
		font = other.font;
		#endif
	}

	template<color::Color CO>
	Buffer& operator=(const Buffer<CO, R> &other)
	{
		this->colormap = other.colormap;
		#if __has_include(<modm/io/iostream.hpp>)
		this->font = other.font;
		#endif

		BufferMal<C, R>::operator=(other);
		return *this;
	}

	/// Different color and size: Use writeImage cause handles Intersection
	template<color::Color CO, shape::Size RO>
	constexpr Buffer(const Buffer<CO, RO> &other) : IOStream(writer), writer(this)
	{
		this->colormap = other.colormap;
		#if __has_include(<modm/io/iostream.hpp>)
		this->font = other.font;
		#endif

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

	C get(const shape::Point& point) const;

	void clear(C color = color::html::Black);

	void invert();

	// Idea for vertical and horizontal shift with <<= and >>=
	// https://stackoverflow.com/questions/486099/can-inner-classes-access-private-variables
 		void operator<<=(const std::size_t shift);
		void operator>>=(const std::size_t shift);

	const void* virtualBuffer() const final
	{ return reinterpret_cast<const void*>(this->buffer); }

	shape::Size virtualSize() const final
	{ return R; }

	std::tuple<shape::Size, const void*>
	getSizeAndBuffer() const final
	{ return std::make_tuple(R, reinterpret_cast<const void*>(this->buffer)); }

private:
	template<color::Color, shape::Size>
	friend class Buffer;

#if __has_include(<modm/io/iostream.hpp>)
public:
	void
	writeChar(char c);

private:
	void wrap_cursor()
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
public:
	// TODO Clever way to set default font?
	Font* font{nullptr};

	/// When streaming chars to the buffer, they are drawn at the cursor position
	shape::Point cursor{0, 0};

	// This is a very simple 'charwrap' feature
	// TODO Improve to also support 'wordwrap'
	bool cursor_autowrap{true};
#endif

	template <modm::graphic::GraphicBuffer GB>
	friend modm::IOStream &
	operator<<(modm::IOStream &, const GB&);
};

#if __has_include(<modm/io/iostream.hpp>)
// TODO Exclude for production
template <modm::graphic::GraphicBuffer GB>
modm::IOStream &
operator<<(modm::IOStream &os, const GB& buffer)
{
	os << "Size: " << GB::Resolution;
	modm::shape::Point scanner;
	while(scanner.y() < GB::Resolution.y()) {
		os << modm::endl;
		while(scanner.x() < GB::Resolution.x()) {
			modm::color::Gray2 gray2 = buffer.get(scanner);
			
			const char utf8[4] = {' ', '.', 'o', 'O'};
			os << utf8[gray2.value()];

			// TODO possible to transmit ░, ▒, ▓, █ via terminal?
			// @see: https://de.wikipedia.org/wiki/Unicodeblock_Blockelemente
			// const char* unicode_block[4] = {"\u2591", "\u2592", "\u2593", "\u2588"};
			// os << unicode_block[gray2.value()];

			scanner.x()++;
		}
		scanner.x() = 0;
		scanner.y()++;
	}
	return os;
}
#endif

}  // namespace modm::graphic

#include "buffer_impl.hpp"