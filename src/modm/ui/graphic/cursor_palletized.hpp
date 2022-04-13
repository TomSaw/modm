#pragma once
#include "cursor.hpp"

#include <cmath>

namespace modm::graphic {

template<class TP, Dimension BD, std::size_t MajorLength>
requires color::ColorPalletized<typename TP::ColorType>
class CursorBuffer<TP, BD, MajorLength> {
protected:
	TP* ptr;
	int16_t pallete_i{0};

	constexpr explicit CursorBuffer(TP* ptr)
		: ptr(ptr)
	{}

	constexpr explicit CursorBuffer(TP* ptr, int16_t pallete_i)
		: ptr(ptr), pallete_i(pallete_i)
	{}

	constexpr explicit CursorBuffer(const CursorBuffer& other)
		: ptr(other.ptr), pallete_i(other.pallete_i)
	{}

	template<BufMemDef BMD, Size R>
	friend class Buffer;

public:
	using PalleteType = TP;
	using ColorType = TP::ColorType;

	ColorType operator=(ColorType color)
	{
		(*ptr)[pallete_i] = color;
		return color; // Allow chained assignment: Cursor = Cursor = C;
	}

	// comparison
	// TODO Damn, these are expensive to evaluate.
	// May need to iterate via start-end POINTS for cursor_palletized
	bool operator==(const CursorBuffer& other) const {
		return ptr == other.ptr && pallete_i == other.pallete_i;
	}

	bool operator>(const CursorBuffer& other) const {
		return (ptr == other.ptr) ? pallete_i > other.pallete_i : ptr > other.ptr;
	}

	bool operator<(const CursorBuffer& other) const {
		// TODO damn, this is very expensive to evaluate :O
		return (ptr == other.ptr) ? pallete_i < other.pallete_i : ptr < other.ptr;
	}

	template <Dimension D>
	struct mover {
		TP*& ptr;
		int16_t& pallete_i;

		mover(CursorBuffer& cursor)
			: ptr(cursor.ptr), pallete_i(cursor.pallete_i)
		{}
		
		static constexpr std::size_t Stride = (BD == D) ? 1 : (PalleteType::Dim == D) ? MajorLength : MajorLength / PalleteType::size;

		void operator++() {
			if constexpr (TP::Dim == D) {
				if(++pallete_i >= int16_t(PalleteType::size)) {
					ptr += Stride;
					pallete_i = 0;
				}
			}
			else // TP::Dim == Col
			{ 
				ptr += Stride;
			}
		};

		void operator--() {
			if constexpr (TP::Dim == D) {
				if(--pallete_i < 0) {
					ptr -= Stride;
					pallete_i = PalleteType::size;
				}
			}
			else // TP::Dim == Col
			{ 
				ptr -= Stride;
			}
		};

		// OPTIMIZE there must a smarter, faster solution
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
			 if constexpr (TP::Dim == D) {
				ptr += (n / int16_t(PalleteType::size)) * int16_t(Stride);
				pallete_i += (n % int16_t(PalleteType::size));
				normalizePallete();
			}
			else // TP::Dim == Col
			{ 
			 	ptr += n * Stride;
			}
		};

		void operator-=(int n) {
			if constexpr (TP::Dim == D) {
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
			Cursor<PalleteType, BD, MajorLength> ret(ptr, pallete_i);
			ret.template axis<D>() += n;
			return ret;

		}

		auto operator-(int n) {
			Cursor<PalleteType, BD, MajorLength> ret(ptr, pallete_i);
			ret.template axis<D>() -= n;
			return ret;
		}
	};

	#if 0
	// TODO this is horrible sketchy, delete or rewrite
	template<Dimension D>
	auto axis_iterator()
	{
		if constexpr(BD == Row) {
			if constexpr (PalleteType::Dim == Row)
				return iterable_colinear<typename array2dT::RowSpanType, typename array2dT::RowIndexType>{array2dT::rowspan(col), R.width()};
			else // PalleteType::Dim == Col
				return iterable_perpendicular<typename array2dT::RowSpanType>{array2dT::rowspan(col / PalleteType::size), col % PalleteType::size};
		}
		else // BD == Col
		{
			if constexpr (PalleteType::Dim == Col)
				return iterable_colinear<typename array2dT::ColSpanType, typename array2dT::ColIndexType>{array2dT::colspan(row), R.height()};
			else // PalleteType::Dim == Row
				return iterable_perpendicular<typename array2dT::ColSpanType>{array2dT::colspan(row / PalleteType::size), row % PalleteType::size}
		}
	}
	#endif
};

} // namespace modm::graphic