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
#include <concepts>

#include "image.hpp"

namespace modm::graphic
{

/**
 * @brief 		Decodes run length encoded images.
 * 				Currently supports only binary encoding.
 */
template<color::Color C>
class Image<C, Encoding::RunLength> : public ImageBase<uint8_t>
{
	using CounterType = uint8_t;

public:
	using ImageBase::ImageBase;


	// TODO need a concept 'ImageIterable'.
	// F.e. directly drwaing images on displays is only supported for ImageIterable

	/**
	 * @brief 	Single Pass Image Iterator
	 * 
	 */
	class Iterable
	{
		CounterType const* data;
		
		const shape::Section clipping;
		// sum of clipped pixels below and above each column
		// OPTIMIZE could be const. Benchmark please!
		uint16_t colGap;
		CounterType counter;
		C color;

		Iterable(CounterType const *data, Size size, shape::Section clipping, C color)
			: data(data),
			clipping(clipping),
			counter(*data),
			color(color)
		{
			// Initial forward to first visible Pixel
			colGap = clipping.topLeft.x() * size.height() + clipping.topLeft.y();
			forwardCol();

			colGap = size.y() - clipping.getHeight();
		}

		friend Image;

	public:

		// OPTIMIZE try inline
		void forwardRow()
		{
			if (--counter == 0)
			{
				color.invert();
				counter = *(++data);
			}
		}

		// OPTIMIZE try inline
		void forwardCol()
		{
			int16_t gap = colGap - counter;
			bool invert = false;

			while (gap >= 0)
			{
				gap -= *(++data);
				invert = !invert;
			}

			if (invert) color.invert();
			counter = -gap;
		}

		class ColumnIteratorSentinel
		{
		protected:
			uint16_t col;

			ColumnIteratorSentinel(uint16_t col)
				: col(col) {}

			friend Iterable;

		public:
			bool
			operator==(const ColumnIteratorSentinel& other) const
			{
				return col == other.col;
			}
		};


		template <bool ColumnIteratorReverse = false>
		class ColumnIterator : public ColumnIteratorSentinel
		{
			Iterable& image;

			ColumnIterator(uint16_t col, Iterable& image)
				: ColumnIteratorSentinel(col), image(image)
			{}

			friend class Iterable;

		public:
			class Row
			{
				Iterable& image;

			public:
				Row(Iterable& image) : image(image) {}

				class RowIteratorSentinel
				{
				protected:
					uint16_t row;

					RowIteratorSentinel(uint16_t row): row(row) {}
					friend ColumnIterator;

				public:
					bool
					operator==(const RowIteratorSentinel& other) const
					{
						return row == other.row;
					}
				};

				template <bool RowIteratorReverse = false>
				class RowIterator : public RowIteratorSentinel
				{
					Iterable& image;

					RowIterator(uint16_t row, Iterable& image)
						: RowIteratorSentinel(row), image(image)
					{}
					friend ColumnIterator;

				public:
				
					C
					operator*() const
					{
						return image.color;
					}

					void
					operator++()
					{
						++this->row;
						image.forwardRow();
					}
				};

				auto begin() const
				{ return RowIterator(image.clipping.topLeft.y(), image); }

				auto end() const
				{ return RowIteratorSentinel(image.clipping.bottomRight.y()); }
			};

			void
			operator++()
			{
				++this->col;				
				image.forwardCol();
			}

			auto
			operator*() const
			{
				return Row(image);
			}

			template<color::Color, Encoding>
			friend class Image;
		};

		auto begin() { return ColumnIterator(clipping.topLeft.x(), *this); }

		auto end() { return ColumnIteratorSentinel(clipping.bottomRight.x()); }

		#if 0
		// TODO support std::views::reverse
		auto rbegin() { return ColumnIterator<true>(clipping.bottomRight.x(), *this); }

		auto rend() { return ColumnIteratorSentinel(clipping.topLeft.x()); }
		#endif
	};

	Iterable
	iterable(C color = color::html::Black)
	{ return Iterable(this->data, this->size, this->size, color); }

	Iterable
	iterable(shape::Section clipping, C color = color::html::Black)
	{ return Iterable(this->data, this->size, clipping, color); }
};
}  // namespace modm::graphic