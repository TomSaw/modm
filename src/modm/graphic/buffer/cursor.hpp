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

template<class T, Dimension BD, std::size_t MajorLength>
class CursorBuffer;

/**
 * @brief 		Uniform Buffer interface to for manipulation algorithms
 */
template<class T, Dimension BD, std::size_t MajorLength>
class Cursor : public CursorBuffer<T, BD, MajorLength> {
public:
	static constexpr std::size_t majorLength = MajorLength;
	// TODO Allow arbitrary MajorLengths and warn the user about performance issues, if std::has_single_bit isn't satisified.
	static_assert(std::has_single_bit(MajorLength));
	static constexpr std::size_t majorMask = majorLength - 1;

	using CursorBuffer<T, BD, MajorLength>::CursorBuffer;
	using CursorBuffer<T, BD, MajorLength>::operator=;

	// accessors
	T* operator&() { return this->ptr; }
	T* operator->() { return this->ptr; }

	T operator*() const { return *this->ptr; }
	T& operator*() { return *this->ptr; }

	// 1D Translation
	template <Dimension D>
	auto axis() { return typename CursorBuffer<T, BD, MajorLength>::mover<D>(*this); }
	
	auto x() { return typename CursorBuffer<T, BD, MajorLength>::mover<X>(*this); }
	auto y() { return typename CursorBuffer<T, BD, MajorLength>::mover<Y>(*this); }

	// 2D Translation
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

		if constexpr(BD == X)
			return offset >= 0 && offset < R.width();
		else // BD == Y
			return std::abs(offset) % MajorLength == 0;
	}

	bool in(BufferType::array2dT::ColSpanType colspan) const
	{
		const int offset = ptr - colspan.data();

		if constexpr(BD == Y)
			return offset >= 0 && offset < R.height();
		else // BD == X
			return std::abs(offset) % MajorLength == 0;
	}

	Buffer<X>: Cursors are vertical to each other in cartesian space
	Buffer<Y>: Cursors are horizontal to each other in cartesian space
	bool minorColinear(const Cursor& other)
	{
		const diff_type diff = ptr - other.ptr;
		return diff % MajorLength == 0;
	}
	#endif
};

} // namespace modm::namespace

#include "cursor_planar.hpp"
#include "cursor_palletized.hpp"