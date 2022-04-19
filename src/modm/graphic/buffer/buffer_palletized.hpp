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
#include "buffer.hpp"

#include <modm/utils/array2d.hpp>

namespace modm::graphic {

/**
 * @brief 		Palletized Buffer ads combines the iteration logic of modm::array2d and modm::pallete
 * 
 * @tparam GDDRAM	Memory Definition 
 * @tparam R 	Resolution
 */
template<Gddram GDDRAM, Size R>
requires ColorPalletized<typename GDDRAM::ColorType>
class BufferBase<GDDRAM, R>
	: public array2d<typename GDDRAM::PalleteType,
	GDDRAM::PalleteType::Dim == Y ? R.width() : GDDRAM::arrSize(R.width()),
	GDDRAM::PalleteType::Dim == X ? R.height() : GDDRAM::arrSize(R.height()),
	GDDRAM::Major>
{
public:
	using PalleteType = GDDRAM::PalleteType;
	using ColorType = PalleteType::ColorType;
	using CursorType = Cursor<PalleteType, GDDRAM::Major, (GDDRAM::Major == X) ? R.width() : R.height()>;

protected:
	using array2dT = array2d<PalleteType,
		PalleteType::Dim == Y ? R.width() : GDDRAM::arrSize(R.width()),
		PalleteType::Dim == X ? R.height() : GDDRAM::arrSize(R.height()),
		GDDRAM::Major>;
	
	// constructors
	using array2dT::array2d;
public:

	// Neccessary because pallete::pallete(TE) is explicit
	BufferBase(PalleteType::ColorType color) : array2dT(PalleteType(color)) {}

	void fill(PalleteType::ColorType color) { array2dT::fill(PalleteType(color)); };
	void fill(PalleteType pallete) { array2dT::fill(pallete); };

	template<class SpanType>
	struct iterable_colinear {
		SpanType span;
		const uint16_t length;

		class const_iterator {
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type   = std::ptrdiff_t;

		protected:
			std::size_t idx;

		public:
			const_iterator(std::size_t idx) : idx(idx) {}

			bool operator<=>(const const_iterator& other) const = default;
		};

		// OPTIMIZE optimize this for input_iterator_tag
		// pallete >>= 1
		// ... in operator++ and ...
		// return pallete[0];
		// ... in operator* is cheaper than making a pixel_accessor for each iteration
		class iterator // : public const_iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = ColorType;
			using pointer           = ColorType*;
			using reference         = ColorType&;

		protected:
			std::size_t idx;
			SpanType::iterator span_iter;
			// PalleteType pallete; // OPTIMIZE Required for input_iterator_tag

		public:
			iterator(std::size_t idx, SpanType::iterator span_iter)
				: idx(idx), span_iter(span_iter) // , pallete(*span_iter) // TODO move to std::input_iterator
			{}

			// ColorType operator*() const { return pallete[0]; }
			// pixel_accessor operator*() { return pallete[0]; } // TODO move to std::input_iterator
			PalleteType::pixel_accessor operator*()
			{ return (*span_iter)[idx % PalleteType::size]; }

			// TODO implement 3-way comparison
			// bool operator<=>(const_iterator& other) const
			bool operator==(iterator& other) const
			{ return idx == other.idx; }

			void operator++() {
				if(++this->idx % PalleteType::size == 0)
					++span_iter;
			}

			void operator--() {
				if(--this->idx % PalleteType::size == 0)
					--span_iter;
			}

			// ATTENTION!!!! span_iter is not yet developed
			iterator operator+(int n)
			{ return iterator(idx + n, span_iter); }
			
			// ATTENTION!!!! span_iter is not yet developed
			iterator operator-(int n)
			{ return iterator(idx - n, span_iter); }

			void operator+=(int n) {
				this->idx += n;
				span_iter += n / PalleteType::size;
			}

			void operator-=(int n) {
				this->idx -= n;
				span_iter -= n / PalleteType::size;
			}
		};

		auto begin() { return iterator(0, span.begin()); }
		auto end() { return iterator(length, span.begin()); }
	};

	// TODO range-concept SpanType
	template<class SpanType>
	struct iterable_perpendicular {
		SpanType span;
		const std::size_t pallete_i;

		class iterator
		{
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = ColorType;
			using pointer           = ColorType*;
			using reference         = ColorType&;

		protected:
			SpanType::iterator span_iter;
			const std::size_t pallete_i;

		public:
			iterator(SpanType::iterator span_iter, std::size_t pallete_i)
				: span_iter(span_iter), pallete_i(pallete_i)
			{}

			// TODO want 3-way comparison
			// bool operator<=>(iterator& other) const
			bool operator==(iterator& other) const
			{ return span_iter == other.span_iter; }
			
			// ColorType operator*() const { return (*span_iter)[pallete_i]; }
			PalleteType::pixel_accessor operator*() { return (*span_iter)[pallete_i]; }

			void operator++() { ++span_iter; }
			void operator--() { --span_iter; }

			iterator operator+(int n) { return iterator(span_iter + n, pallete_i); }
			iterator operator-(int n) { return iterator(span_iter - n, pallete_i); }

			void operator+=(int n) { span_iter += n; }
			void operator-=(int n) { span_iter -= n; }
		};

		auto begin() { return iterator(span.begin(), pallete_i); }
		auto end() { return iterator(span.end(), pallete_i); }
	};

	auto xspan(std::size_t col)
	{
		if constexpr (PalleteType::Dim == X)
			return iterable_colinear{array2dT::rowspan(col), R.width()};
		else // PalleteType::Dim == Y
			return iterable_perpendicular{array2dT::rowspan(col / PalleteType::size), col % PalleteType::size};
	}

	auto yspan(std::size_t row)
	{
		if constexpr (PalleteType::Dim == Y)
			return iterable_colinear{array2dT::colspan(row), R.height()};
		else // PalleteType::Dim == X
			return iterable_perpendicular{array2dT::colspan(row / PalleteType::size), row % PalleteType::size};
	}
};
} // modm::graphic