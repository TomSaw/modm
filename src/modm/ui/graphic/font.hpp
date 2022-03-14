/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
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

#include <modm/architecture/interface/accessor.hpp>
#include <modm/ui/font.hpp>

#include "buffer.hpp"

namespace modm::graphic
{

template<color::Color, Size>
class Buffer;

/**
 * @brief 		Represents a loaded Font.
 * 				- Can be used by multiple Buffer
 * 				- Provides Font and String Metadata required for Rendering
 *
 */
class Font
{
public:
	Font() { setFont(modm::font::FixedWidth5x8); }

	Font(const auto *font) { setFont(font); }

	void
	setFont(const modm::accessor::Flash<uint8_t> *font)
	{
		setFont(font->getPointer());
	}

	void
	setFont(const uint8_t *font)
	{
		if (modm::accessor::asFlash(font).isValid())
		{
			fontset = modm::accessor::asFlash(font);

			height = fontset[3];
			hspace = fontset[4];
			vspace = fontset[5];
			first = fontset[6];
			count = fontset[7];

			// calculated
			last = first + count;
		}
	}

	bool
	charExists(char c) const
	{
		return c >= first and c < last;
	}

	/// getters

	uint8_t
	getCharHeight() const
	{
		return height;
	}

	uint8_t
	getLineHeight() const
	{
		return height + hspace;
	}

	uint8_t
	getCharWidth(char c) const
	{
		const std::size_t position = offsetCharWidths + c - first;
		return fontset[position];
	}

	uint8_t
	getTabWidth() const {
		// TODO Imrove this tab feature
		return height;
	}

	Size
	getCharSize(char c) const
	{
		return {getCharWidth(c), height};
	}

	Size
	getStringSize(char *s) const
	{
		Size size = {0, height};
		while (*s) size.x() += getCharWidth(*s++) + vspace;

		// size.x() -= vspace;
		return size;
	}

	ImageAccessor<color::Monochrome, modm::accessor::Flash>
	getImageAccessor(char c, shape::Point placement) {
		const std::size_t position = offsetCharWidths + c - first;
		const uint8_t usedRows = (height + 7) / 8;  // round up

		auto ptr = fontset.getPointer() + offsetCharWidths + count;
		for (uint8_t i = offsetCharWidths; i < position; i++)
			ptr += fontset[i] * usedRows;

		// TODO Add support for more ColorTypes by adding the fonts ColorType to metadata and switch over this here:
		return ImageAccessor<color::Monochrome, modm::accessor::Flash>(ptr, getCharSize(c), placement);
	}

private:
	modm::accessor::Flash<uint8_t> fontset;

	/// Cached metadata. Updates on font-change
	uint8_t height, hspace, vspace, first, count, last;

	static constexpr uint8_t offsetCharWidths = 8;

	template<color::Color, Size>
	friend class Buffer;
};
}  // namespace modm::graphic