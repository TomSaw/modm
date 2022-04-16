#pragma once
#include "cursor.hpp"

#include <cmath>

namespace modm::graphic {

template<color::ColorPlanar C, Dimension BD, std::size_t MajorLength>
class CursorBuffer<C, BD, MajorLength> {
protected:
	C* ptr;

	constexpr explicit CursorBuffer(C* ptr)
		: ptr(ptr)
	{}

	constexpr CursorBuffer(const CursorBuffer& other)
		: ptr(other.ptr)
	{}

	// Convert to cartesian coordinates
	// from known offset to beginning of Buffer

	// TODO let's see, if getX() getY() is needed ...
	uint16_t getX(std::size_t offset) const {
		if constexpr(BD == X)
			return offset % MajorLength;
		else // (BD == Y)
			return offset / MajorLength;
	}

	uint16_t getY(std::size_t offset) const {
		if constexpr(BD == X)
			return offset / MajorLength;
		else // (BD == Y)
			return offset % MajorLength;
	}

	Point getPoint(std::size_t offset) const {
		return Point(getX(offset), getY(offset));
	}

	template<BufMemDef BMD, Size R>
	friend class Buffer;

public:
	using ColorType = C;

	// operator C&() { return *this->ptr; }
	// operator C() const { return *this->ptr; }

	ColorType operator=(ColorType color)
	{
		*ptr = color;
		return color; // Allow chained assignment: Cursor = Cursor = C;
	}

	// comparison
	auto operator<=>(const CursorBuffer& other) const = default;

	// iterators

	// cartesian translations
	// FIXME Principal Design Issue:
	// Exceeding MajorDim wraps Cursor to next minorDim, the information is permanently lost.
	// Thus, Cursors are only reliable, when there's no wrapping.
	// - That's true for drawing algorithms. (they're iterating)
	// - Inbounds check and shape-trimming must happen in Point space!

	/// Iterates the Cursor in 2D. Intentionally not named 'iterator' cause doesn't dereference.
	template <Dimension D>
	struct mover {
		ColorType*& ptr;

		mover(CursorBuffer& cursor)
			: ptr(cursor.ptr)
		{}
		
		static constexpr std::size_t Stride = (BD == D) ? 1 : MajorLength;

		void operator++() { ptr += Stride; };
		void operator--() { ptr -= Stride; };
		
		void operator+=(int n) { ptr += n * Stride; };
		void operator-=(int n) { ptr -= n * Stride; };

		auto operator+(int n) { return Cursor<ColorType, BD, MajorLength>(ptr + n * Stride); }
		auto operator-(int n) { return Cursor<ColorType, BD, MajorLength>(ptr - n * Stride); }
	};

	// Returns an iterator through this->ptr in Dimension D
	template<Dimension D>
	auto axis_iterator() {
		if constexpr(BD == D)
			return typename std::span<C>::iterator(ptr);
		else
			return typename modm::strided_span<C, MajorLength>::iterator(ptr);
	}
}; // CursorBuffer

} // namespace modm::graphic;