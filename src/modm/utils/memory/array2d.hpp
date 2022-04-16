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

#include <array>
#include <ranges>
#include <span>

#include "strided_span.hpp"
#include <modm/math/geometry/dimension.hpp>

namespace modm
{
	/**
	 * @brief 			2D Array-type with selectable major dimension. Iterator interface for the whole array
	 * 					or sole rows or columns.
	 * 
	 * 					- Base to graphic::Buffer: A proper memory layout is critical when sending Buffers via DMA.
	 * 					- May also become the Base to modm::Matrix.
	 * 					- May evolve to a Tensor.
	 * 
	 * @see				https://github.com/Lingxi-Li/Modern_CPP_Challenge_Solution/blob/master/17_array2d.hpp
	 * @see				https://www.youtube.com/watch?v=8P-sDH9XJAc
	 * 
	 * @tparam T		Element Type
	 * 
	 * @tparam Cols		Number of Columns
	 * @tparam Rows		Number of Rows
	 * 
	 * @tparam Major	Major dimension of the Memory Layout. This is especially important when the memory needs
	 * 					DMA transmission to a pre-defined memory layout.
	 * @see				https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays
	 * 
	 */

	template<typename T, std::size_t Rows, std::size_t Cols, Dimension D>
	class array2d
	{
		std::array<std::array<T, (D == Y) ? Cols : Rows>, (D == Y) ? Rows : Cols> data_;

	public:
		static constexpr std::tuple sizes{Rows, Cols};
		static constexpr int size = Rows * Cols;
		static constexpr std::ptrdiff_t minorFreq = (D == X) ? Rows : Cols;

		using RowSpanType = std::conditional_t<D == X, std::span<T>, modm::strided_span<T, minorFreq>>;
		using ColSpanType = std::conditional_t<D == Y, std::span<T>, modm::strided_span<T, minorFreq>>;

		/// constructors
		constexpr array2d() = default;

		constexpr array2d(T value)
		{ std::ranges::fill(span(), value); }

		template<typename U>
		constexpr array2d(const array2d<U, Rows, Cols, D>& other) {
			std::ranges::copy(other.span(), span().begin());
		}

		template<typename U, Dimension E>
		constexpr array2d(const array2d<U, Rows, Cols, E>& other)
		requires (D != E && D == X)
		{
			auto begin = span().begin();
			std::size_t row = 0;
			while(begin != span().end()) {
				// TODO something is missing for std::ranges::copy
				// begin = std::ranges::copy(other.rowspan(row++), begin);
				begin = std::copy(other.rowspan(row).begin(), other.rowspan(row).end(), begin);
				++row;
			}
		}

		template<typename U, Dimension E>
		constexpr array2d(array2d<U, Rows, Cols, E>& other)
		requires (D != E && D == Y)
		{
			auto begin = span().begin();
			std::size_t col = 0;
			while(begin != span().end()) {
				// TODO something is missing for std::ranges::copy
				// begin = std::ranges::copy(other.colspan(col++), begin);
				begin = std::copy(other.colspan(col).begin(), other.colspan(col).end(), begin);
				++col;
			}
		}

		template<class DmaChannel>
		array2d(array2d& other) {
			// TODO
			// MODM_LOG_ERROR << "DMA copy constructor not implemented";
		}

		/// assignment operators
		template<typename U>
		constexpr array2d& operator=(array2d<U, Rows, Cols, D>& other) {
			if(this == &other) return *this;
			std::ranges::copy(other.span(), span().begin());
			return *this;
		}

		template<typename U, Dimension E>
		constexpr array2d& operator=(array2d<U, Rows, Cols, D> &other) requires (D != E) {
			// TODO Iterate over column and rows
			// MODM_LOG_ERROR << "assigning from flipped Major Dimension not implemented";
			return *this;
		}

		template<class DmaChannel>
		array2d& operator=(array2d& other) {
			if(this == &other) return *this;
			// TODO
			// MODM_LOG_ERROR << "DMA copy assignment not implemented";
			return *this;
		}

		constexpr void fill(T value)
		{ std::ranges::fill(span(), value); }

		// access specified element with bounds checking
		constexpr T&
		at(std::size_t row, std::size_t col)
		{
			if constexpr(D == X)
				return data_.at(col).at(row);
			else // D == Y
				return data_.at(row).at(col);
		}

		// access specified element with bounds checking
		constexpr T
		at(std::size_t row, std::size_t col) const
		{
			if constexpr(D == X)
				return data_.at(col).at(row);
			else // D == Y
				return data_.at(row).at(col);
		}

		constexpr T*
		data()
		{ return data_[0].data(); }

		constexpr auto
		span()
		{ return std::span(data_[0].data(), size); }

		constexpr auto
		colspan(std::size_t row)
		{
			T* begin = &at(row, 0);
			return ColSpanType(begin, (D == Y) ? Cols : size);
		}

		constexpr auto
		rowspan(std::size_t col)
		{
			T* begin = &at(0, col);
			return RowSpanType(begin, (D == X) ? Rows : size);
		}

		constexpr auto columns(std::size_t first_row, std::size_t last_row) {
			// TODO return iteratable over column
			// MODM_LOG_ERROR << "Not implemented";
			return nullptr;
		}

		constexpr auto rows(std::size_t first_row, std::size_t last_row) {
			// TODO return iteratable over row
			// MODM_LOG_ERROR << "Not implemented";
			return nullptr;
		}
	};
}