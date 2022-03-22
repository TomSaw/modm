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
#include "image.hpp"

namespace modm::graphic
{

template<color::ColorPlanar C>
class Image<C, Encoding::Uncompressed> : public ImageBase<C>
{
public:
	using ImageBase<C>::ImageBase;

	static constexpr int rowInc = 1; // TODO MemoryLayout

	class Iterable {
		C const *data;
		const std::size_t colIncr;

		const shape::Section clipping;
		
		Iterable(C const *data, shape::Size size, shape::Section clipping)
			: data(data + clipping.topLeft.y() + clipping.topLeft.x() * size.height()),
			colIncr(size.height()),
			clipping(clipping)
		{}

		friend class Image;

	public:
		class ColumnIteratorSentinel
		{
		protected:
			C const *data;

			ColumnIteratorSentinel(C const *data)
				: data(data)
			{}

			friend class Iterable;

		public:

			bool
			operator==(const ColumnIteratorSentinel& other) const
			{ return data == other.data; }
		};

		class ColumnIterator : public ColumnIteratorSentinel
		{
			const std::size_t colIncr;
			const uint16_t rows;

			ColumnIterator(C const *data, const std::size_t colIncr, const uint16_t rows)
				: ColumnIteratorSentinel(data), colIncr(colIncr), rows(rows)
			{}

			friend class Iterable;

		public:
			void operator++()
			{
				this->data += colIncr;
			}

			auto
			operator*() const
			{
				return std::span<const C>(this->data, this->rows); // TODO MemoryLayout, iterator requires += rowIncr
			}
		};

		auto begin()
		{ return ColumnIterator(data, colIncr, clipping.getHeight()); }

		auto end()
		{ return ColumnIteratorSentinel(data + clipping.getWidth() * colIncr); }

		// IMPLEMENT reverse iterators for mirroring
		// @see std::make_reverse_iterator
		#if 0
		auto rbegin()
		{ return ColumnIterator<true>(data + clipping.getWidth() * colIncr, size.height(), clipping.getHeight()); }

		auto rend()
		{ return ColumnIteratorSentinel(data, size.height(), clipping.getHeight()); }
		#endif
	};

	Iterable
	iterable()
	{ return Iterable(this->data, this->size, shape::Section(this->size)); }

	Iterable
	iterable(shape::Section clipping)
	{ return Iterable(this->data, this->size, clipping); }
};

}