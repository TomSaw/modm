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
template<class TC, ColorPencil CE, class... FuncIter>
void
render(TC start, TC end, CE&& color_emitter, FuncIter... iter)
{
	// if(start > end)
	// 	std::swap(start, end);

	while (start != end)
	{
		start = color_emitter();

		(iter(start), ...);
	};

	start = color_emitter();
}

template<class TC, color::Color C, class... FuncIter>
void render(TC start, TC end, C color, FuncIter... iter)
{
	// if(start > end)
	// 	std::swap(start, end);

	/**
	 * @brief 	This end condition is very fast and also very dangerous
	 * 			If your drawing algorithm is buggy, start never reaches end and crashes
	 *
	 * 			For debugging, replace '''while (start != end)'''
	 * 			with something like '''for(int i = 0; i < 20; i++)'''
	 * 
	 */
	while (start != end)
	{
		start = color;

		(iter(start), ...);
	};

	start = color;
}

namespace detail
{
// Line drawing iterators
template<modm::Dimension D, int Dir = 1>
struct iterOrtho
{
	template<typename CursorType>
	void
	operator()(CursorType& cursor)
	{
		if(Dir == 1)
			++cursor.template axis<D>();
		else
			--cursor.template axis<D>();
	}
};

template<modm::Dimension D, int Dir = 1>
struct iterBresenham
{
public:
	const Point m;
	std::ptrdiff_t error{m.x() / 2};

	template<typename CursorType>
	void
	operator()(CursorType& cursor)
	{
		error += m.y();
		if (error >= m.x())
		{
			error -= m.x();
			
			if(Dir == 1)
				++cursor.template axis<D>();
			else
				--cursor.template axis<D>();
		}
	}
};
}  // namespace detail
}  // namespace modm::graphic