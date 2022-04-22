#pragma once
#include "cursor.hpp"

#include "buffer.hpp"

#include <cmath>
#include <modm/utils/strided_span.hpp>

namespace modm::graphic::detail {

template<ColorPlanar C, Dimension MD, std::size_t ML>
class CursorBase<C, MD, ML> {
public:
	using ColorType = C;

protected:
	ColorType* ptr;

	constexpr explicit CursorBase(ColorType* ptr)
		: ptr(ptr)
	{}

	constexpr CursorBase(const CursorBase& other)
		: ptr(other.ptr)
	{}

	// Convert to cartesian coordinates
	// from known offset to beginning of Buffer

	// TODO let's see, if access to components is useful at all...
	uint16_t getX(std::size_t offset) const {
		if constexpr(MD == X)
			return offset % ML;
		else // (MD == Y)
			return offset / ML;
	}

	uint16_t getY(std::size_t offset) const {
		if constexpr(MD == X)
			return offset / ML;
		else // (MD == Y)
			return offset % ML;
	}

	Point getPoint(std::size_t offset) const {
		return Point(getX(offset), getY(offset));
	}

public:

	ColorType operator=(ColorType color)
	{
		*ptr = color;
		return color; // Allow chained assignment: Cursor = Cursor = C;
	}

	// comparison
	auto operator<=>(const CursorBase& other) const = default;

	// translators (iterators)
	
	/// iterator-like cartesion translation of the cursor in X or Y dimension
	/// Apllied for hyperfast buffer manipulations like drawing Lines, Rectangles, ...
	template <Dimension D>
	struct translator {
		ColorType*& ptr;

		translator(CursorBase& cursor)
			: ptr(cursor.ptr)
		{}
		
		static constexpr std::size_t Stride = (MD == D) ? 1 : ML;

		void operator++() { ptr += Stride; };
		void operator--() { ptr -= Stride; };
		
		void operator+=(int n) { ptr += n * Stride; };
		void operator-=(int n) { ptr -= n * Stride; };

		auto operator+(int n) { return Cursor<ColorType, MD, ML>(ptr + n * Stride); }
		auto operator-(int n) { return Cursor<ColorType, MD, ML>(ptr - n * Stride); }
	};

	// Returns an iterator going through this->ptr
	template<Dimension D>
	auto axis_iterator() {
		if constexpr(MD == D)
			return typename std::span<C>::iterator(ptr);
		else // (MD == Flip<D>::value)
			return typename modm::strided_span<C, ML>::iterator(ptr);
	}
}; // CursorBase

} // namespace modm::graphic;