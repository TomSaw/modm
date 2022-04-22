#pragma once
#include "cursor.hpp"

#include "buffer.hpp"

#include <cmath>

namespace modm::graphic::detail {

template<class P, Dimension MD, std::size_t ML>
requires ColorPalletized<typename P::ColorType>
class CursorBase<P, MD, ML> {
public:
	using PalleteType = P;
	using ColorType = P::ColorType;

protected:
	PalleteType* ptr;
	int16_t pallete_i{0};

	constexpr explicit CursorBase(PalleteType* ptr)
		: ptr(ptr)
	{}

	constexpr explicit CursorBase(PalleteType* ptr, int16_t pallete_i)
		: ptr(ptr), pallete_i(pallete_i)
	{}

	constexpr explicit CursorBase(const CursorBase& other)
		: ptr(other.ptr), pallete_i(other.pallete_i)
	{}

public:
	ColorType operator=(ColorType color)
	{
		(*ptr)[pallete_i] = color;
		return color; // Allow chained assignment: Cursor = Cursor = C;
	}

	// comparison
	bool operator==(const CursorBase& other) const {
		return ptr == other.ptr && pallete_i == other.pallete_i;
	}

	bool operator>(const CursorBase& other) const {
		return (ptr == other.ptr) ? pallete_i > other.pallete_i : ptr > other.ptr;
	}

	bool operator<(const CursorBase& other) const {
		// TODO damn, this is very expensive to evaluate :O
		return (ptr == other.ptr) ? pallete_i < other.pallete_i : ptr < other.ptr;
	}

	// translators (iterators)
	
	/// iterator-like cartesion translation of the cursor in X or Y dimension
	/// Apllied for hyperfast buffer manipulations like drawing Lines, Rectangles, ...
	template <Dimension D>
	struct translator {
		PalleteType*& ptr;
		int16_t& pallete_i;

		translator(CursorBase& cursor)
			: ptr(cursor.ptr), pallete_i(cursor.pallete_i)
		{}
		
		static constexpr std::size_t Stride = (MD == D) ? 1 : (PalleteType::Dim == D) ? ML : ML / PalleteType::size;

		void operator++() {
			if constexpr (PalleteType::Dim == D) {
				if(++pallete_i >= int16_t(PalleteType::size)) {
					ptr += Stride;
					pallete_i = 0;
				}
			}
			else // PalleteType::Dim == Y
			{ 
				ptr += Stride;
			}
		};

		void operator--() {
			if constexpr (PalleteType::Dim == D) {
				if(--pallete_i < 0) {
					ptr -= Stride;
					pallete_i = PalleteType::size - 1;
				}
			}
			else // PalleteType::Dim == Y
			{ 
				ptr -= Stride;
			}
		};

		// OPTIMIZE there must be a smarter, faster solution
		void normalizePallete() {
			if(pallete_i < 0) {
				--ptr;
				pallete_i += PalleteType::size;
			}
			else if (pallete_i >= int16_t(PalleteType::size))
			{
				++ptr;
				pallete_i -= PalleteType::size;
			}
		}
		
 		void operator+=(int n) {
			 if constexpr (PalleteType::Dim == D) {
				// FIXME Doesn't work for small values!!!
				ptr += (n / int16_t(PalleteType::size)) * int16_t(Stride);
				pallete_i += (n % int16_t(PalleteType::size));
				normalizePallete();
			}
			else
			{
			 	ptr += n * Stride;
			}
		};

		void operator-=(int n) {
			if constexpr (PalleteType::Dim == D) {
				// FIXME Doesn't work for small values!!!
				ptr -= (n / int16_t(PalleteType::size)) * int16_t(Stride);
				pallete_i -= (n % int16_t(PalleteType::size));
				normalizePallete();
			}
			else
			{
				ptr -= n * Stride;
			}
		};

		auto operator+(int n) {
			Cursor<PalleteType, MD, ML> ret(ptr, pallete_i);
			ret.template axis<D>() += n;
			return ret;

		}

		auto operator-(int n) {
			Cursor<PalleteType, MD, ML> ret(ptr, pallete_i);
			ret.template axis<D>() -= n;
			return ret;
		}
	};

	#if 0
	// TODO this looks horrible.m Drop or rewrite

	// Returns an iterator going through this->ptr
	template<Dimension D>
	auto axis_iterator()
	{
		if constexpr(MD == X) {
			if constexpr (PalleteType::Dim == X)
				return iterable_colinear<typename array2dT::RowSpanType, typename array2dT::RowIndexType>{array2dT::rowspan(col), R.width()};
			else // PalleteType::Dim == Y
				return iterable_perpendicular<typename array2dT::RowSpanType>{array2dT::rowspan(col / PalleteType::size), col % PalleteType::size};
		}
		else // MD == Y
		{
			if constexpr (PalleteType::Dim == Y)
				return iterable_colinear<typename array2dT::ColSpanType, typename array2dT::ColIndexType>{array2dT::colspan(row), R.height()};
			else // PalleteType::Dim == X
				return iterable_perpendicular<typename array2dT::ColSpanType>{array2dT::colspan(row / PalleteType::size), row % PalleteType::size}
		}
	}
	#endif
};

} // namespace modm::graphic