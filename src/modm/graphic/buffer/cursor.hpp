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

#include <modm/graphic/color/concepts.hpp>
#include <modm/math/geometry/point.hpp>

namespace modm::graphic {

template<Gddram GDDRAM, Size R>
class Buffer;

namespace detail {
template<class T, Dimension MD, std::size_t ML>
class CursorBase;
}

/**
 * @brief 			Uniform Buffer interface to for manipulation algorithms
 * 
 * @tparam		T	Type the Cursor points to
 * @tparam		MD	Major Dimension of the Buffer
 * @tparam		ML	Number of pixels within Major Dimension
 */
template<class T, Dimension MD, std::size_t ML>
requires ColorPlanar<T> or ColorPalletized<typename T::ColorType>
class Cursor : public detail::CursorBase<T, MD, ML> {
	using CursorBaseType = detail::CursorBase<T, MD, ML>;

	// Only Buffer factorizes Cursor
	template<Gddram GDDRAM, Size R>
	friend class ::modm::graphic::Buffer;

public:
	static constexpr std::size_t majorLength = ML;
	// TODO Allow arbitrary MLs and warn the user about performance issues, if std::has_single_bit isn't satisified.
	static_assert(std::has_single_bit(ML));
	static constexpr std::size_t majorMask = majorLength - 1;


	using CursorBaseType::CursorBase;
	using CursorBaseType::operator=;

	// accessors
	T* operator&() { return this->ptr; }
	T* operator->() { return this->ptr; }

	T operator*() const { return *this->ptr; }
	T& operator*() { return *this->ptr; }

	// 1D translations
	template <Dimension D>
	auto axis() { return typename CursorBaseType::translator<D>(*this); }
	auto x() { return typename CursorBaseType::translator<X>(*this); }
	auto y() { return typename CursorBaseType::translator<Y>(*this); }

	// 2D translations
	void operator+=(const Point& delta) {
		x() += delta.x();
		y() += delta.y();
	}

	void operator-=(const Point& delta) {
		x() -= delta.x();
		y() -= delta.y();
	}

	constexpr Cursor operator+(const Point& delta) {
		Cursor ret(*this);
		ret += delta;
		return ret;
	}

	constexpr Cursor operator-(const Point& delta) {
		Cursor ret(*this);
		ret -= delta;
		return ret;
	}

	#if 0
	// ##################################################
	// # Cheap Cursor predicates

	bool in(BufferType& buffer) const {
		return ptr >= buffer.data() && ptr < buffer.data() + buffer.size;
	}

	bool in(BufferType::array2dT::RowSpanType rowspan) const
	{
		const int offset = ptr - rowspan.data();

		if constexpr(MD == X)
			return offset >= 0 && offset < R.width();
		else // MD == Y
			return std::abs(offset) % ML == 0;
	}

	bool in(BufferType::array2dT::ColSpanType colspan) const
	{
		const int offset = ptr - colspan.data();

		if constexpr(MD == Y)
			return offset >= 0 && offset < R.height();
		else // MD == X
			return std::abs(offset) % ML == 0;
	}

	Buffer<X>: Cursors are vertical to each other in cartesian space
	Buffer<Y>: Cursors are horizontal to each other in cartesian space
	bool minorColinear(const Cursor& other)
	{
		const diff_type diff = ptr - other.ptr;
		return diff % ML == 0;
	}
	#endif
};

} // namespace modm::namespace

#include "cursor_planar.hpp"
#include "cursor_palletized.hpp"