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
#include "debug.hpp"

#include <modm/math/utils/integer_pallete.hpp>

static uint8_t const *data_global;

namespace modm::graphic
{

template<color::ColorPalletized C>
class Image<C, Encoding::Uncompressed> : public ImageBase<typename C::TPallete>
{
	using T = C::TPallete;
	using Pallete = math::Pallete<T, C::Digits>;

	struct Span {
		const int begin, end;
	};

	static constexpr std::size_t rowInc = 1; // TODO MemoryLayout

public:
	using ImageBase<T>::ImageBase;

	class Iterable {
		T const *data;
		
	public:
		// OPTIMIZE move meta to global space and don't pass it through all these
		// nested constructors. Let's check the benchmark then.
		// At least for synchrounous ops this should be an acceptable solution.
		struct Meta {
			const std::size_t colIncr;
			const int row_begin, row_end; // OPTIMIZE Benchmark with other types
			const int shift_top, shift_bot; // OPTIMIZE Benchmark with other types
			const T bitmask_top, bitmask_bot;

			Meta(std::size_t colIncr, uint8_t row_begin, uint8_t row_end)
				: colIncr(colIncr),
				row_begin(row_begin),
				row_end(row_end),
				shift_top(Pallete::getLShift(row_begin)),
				shift_bot(Pallete::getLShift(row_end)),
				bitmask_top(Pallete::bitmask >> shift_top),
				bitmask_bot(Pallete::bitmask << shift_bot)
			{
				// MODM_LOG_DEBUG_ASCII(row_begin);
				// MODM_LOG_DEBUG_ASCII(row_end);
				// MODM_LOG_DEBUG_ASCII(shift_top);
				// MODM_LOG_DEBUG_ASCII(shift_bot);
				// MODM_LOG_DEBUG_BIN(bitmask_top);
				// MODM_LOG_DEBUG_BIN(bitmask_bot);
			}
		} const meta;

		T const *data_end;

	private:
		Iterable(T const *data, shape::Size size, shape::Section clipping)
			: data(data + Pallete::getOffset(clipping.topLeft.y() + clipping.topLeft.x() * size.height())),
			meta(
				Pallete::getOffset(size.height()), // TODO MemoryLayout
				clipping.topLeft.y(),
				clipping.bottomRight.y()
			),
			data_end(this->data + clipping.getWidth() * meta.colIncr)
		{
			// if ("column spans just one T") {
			// 	bitmask_bot |= bitmask_top;
			// }

			data_global = data;
			// MODM_LOG_DEBUG << "data offset:\t" << this->data - data_global << modm::endl;
		}

		friend Image;
	
	public:

		class ColumnIteratorUnmutable
		{
		protected:
			T const *data;

			ColumnIteratorUnmutable(T const *data) : data(data)
			{}

			friend Iterable;

		public:
			bool
			operator==(const ColumnIteratorUnmutable &other) const
			{ return data == other.data; }
		};

		class ColumnIterator : public ColumnIteratorUnmutable
		{
			const Meta& meta;

			// Only modm::graphic::Image can construct Iterators
			ColumnIterator(T const *data, const Meta& meta)
				: ColumnIteratorUnmutable(data),
				meta(meta)
			{}
			
			friend class Iterable;

		public:
			void operator++()
			{ this->data += meta.colIncr; }

			class Row {
				T const *data;
				const Meta& meta;

				Row(T const *data, const Meta& meta)
					: data(data),
					meta(meta)
				{
					// MODM_LOG_WARNING << "data offset:\t" << this->data - data_global << modm::endl;
				}
				
				friend ColumnIterator;

			public:

				/**
				 * @brief 	Pixel Iterator splices each Pixel from the Pallete.
				 * 			Used when target memory is of different ColorType.
				 * 
				 */
				class RowIteratorUnmutable {
				protected:
					uint16_t row;

					RowIteratorUnmutable(uint16_t row)
						: row(row)
					{}

					friend Row;

				public:
					
					bool
					operator==(const RowIteratorUnmutable& other) const
					{
						// Need >= because RowIteratorPallete::operator++ overshots
						// FIXME Is order sensitive :/
						// FIXME Requires <= if running reversed iterators
						return row >= other.row;
					}
				};

				class RowIterator : public RowIteratorUnmutable
				{
					T const *data;
					T pallete;

					// TODO initial rotate is missing
					RowIterator(uint16_t row, T const *data, int shift_top)
						: RowIteratorUnmutable(row),
						data(data)
						// pallete(*data >> shift_top)
					{
						// Load just in time!
						// Start state sanitizer
						if(palleteTransition())
							this->data -= rowInc;
						else
							pallete = *data >> (shift_top - C::Digits);
					}
					friend Row;

					bool palleteTransition() {
						return this->row % Pallete::digits == 0;
					}

					void loadNext() {
						// Time for next pallete?
						if (palleteTransition()) {
							data += rowInc;
							pallete = *data;
							// MODM_LOG_WARNING << "| ";
						}
						else
							pallete >>= C::Digits;
					}

				public:

					void
					operator++()
					{
						// Do not Load here
						// loadNext();
						++this->row;
					}

					C operator*()
					{
						// Load just in time!
						loadNext();
						return pallete & C::max;
					}
				};

				auto begin() const
				{ return RowIterator(meta.row_begin, data, meta.shift_top); }

				auto end() const
				{ return RowIteratorUnmutable(meta.row_end); }

				/**
				 * @brief 	Pallete Iterator splits the Pallete max once.
				 * 			Used when the target memory is of same palletized ColorType.
				 * 
				 */
				class RowIteratorPallete : public RowIteratorUnmutable
				{
					T const *data;
					
					const int shift_top, shift_bot;

					T pallete;
					T temp;

					RowIteratorPallete(uint16_t row, T const *data, int shift_top, int shift_bot)
						: RowIteratorUnmutable(row),
						data(data),
						shift_top(shift_top),
						shift_bot(shift_bot),
						temp(*this->data)
					{}

					friend Row;

					void loadNext() {
						this->pallete = temp >> shift_top;
						this->data += rowInc;
						temp = *this->data;
						this->pallete |= temp << shift_bot;
					}

				public:

					void
					operator++()
					{
						// Do not Load here
						// loadNext();
						this->row += Pallete::digits;
					}

					T operator*()
					{
						// Load just in time!
						loadNext();
						return this->pallete;
					}
				};

				auto beginPallete()
				{ return RowIteratorPallete(meta.row_begin, data, meta.shift_top, meta.shift_bot); }
			};
		
			auto
			operator*() const
			{
				return Row(this->data, meta);
			}

		};

		auto begin() const
		{ return ColumnIterator(data, meta); }

		auto end() const
		{ return ColumnIteratorUnmutable(data_end); }

		// TODO implement for mirroring
		// ColumnIterator rbegin() {
		// 	return ColumnIterator(data, size.height(), clipping.getHeight());
		// }

		// TODO Specialize for various Storages and Decoders

		// template <Accessor, Decoder>
		// class ColumnIterator {
	};

	Iterable
	iterable()
	{ return Iterable(this->data, this->size, shape::Section(this->size)); }

	Iterable
	iterable(shape::Section clipping)
	{ return Iterable(this->data, this->size, clipping); }
};

}