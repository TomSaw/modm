
#pragma once

#include <modm/math/geometry/shape.hpp>
#include <modm/ui/graphic/buffer.hpp>

#include <functional>

namespace modm::graphic
{

/**
 * @brief 		Basic render strategy for basic lines
 * 				
 */
template<class TC, ColorPencil CE, class FuncIter>
void
render(TC start, TC end, CE&& color_emitter, FuncIter iter = FuncIter())
{
	// Always iterate to positive per convention
	if(start > end)
		std::swap(start, end);

	for(int i = 0; i < 6; ++i)
	// while (start != end)
	{
		start = color_emitter();
		iter(start);
	};

	start = color_emitter();
}

template<class TC, color::Color C, class FuncIter>
void render(TC start, TC end, C color, FuncIter iter = FuncIter()) {
	// Always iterate to positive per convention

	// while (start != end)
	for(int i = 0; i < 5; ++i)
	{
		start = color;
		iter(start);
	};

	start = color;
}

namespace detail
{
// Line drawing iterators
template<modm::Dimension D>
struct iterOrtho
{
	template<typename CursorType>
	void
	operator()(CursorType& cursor)
	{
		++cursor.template axis<D>();
	}
};

template<modm::Dimension D>
struct iterDiag
{
	const int dir;

	template<typename CursorType>
	void
	operator()(CursorType& cursor)
	{
		cursor.template axis<D>() += dir;
		++cursor.template axis<modm::DimensionFlip<D>::value>();
	}
};

template<modm::Dimension D>
struct iterBresenham : public iterDiag<D>
{
public:
	const uint16_t long_axis, short_axis;
	std::ptrdiff_t error{long_axis / 2};

	template<typename CursorType>
	void
	operator()(CursorType& cursor)
	{
		++cursor.template axis<D>();

		error += short_axis;
		if (error >= long_axis)
		{
			error -= long_axis;
			cursor.template axis<modm::DimensionFlip<D>::value>() += this->dir;
		}
	}
};
}  // namespace detail
}  // namespace modm::graphic