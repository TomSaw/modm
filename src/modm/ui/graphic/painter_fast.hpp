#pragma once

#include "buffer_memory_definition.hpp"
#include <modm/math/geometry/shape.hpp>
#include <modm/ui/color/rgb_html.hpp>

#include "pencils.hpp"
#include "renderer_plain.hpp"

namespace modm::graphic {

/**
 * @brief 		Simple fast Painter for Lines, Rectangle, Circle, Ellipse
 */
template<class TB>
class PainterFast {
public:
	using ColorType = TB::ColorType;
	using CursorType = TB::CursorType;

	using ColorEmitter = ColorType;

	Point position;
private:
	TB& buffer;
	ColorType color{color::html::White};

public:
	PainterFast(TB& buffer, Point position)
		: buffer(buffer), position(position)
	{}

	/// Change color
	PainterFast& operator<<(ColorType color) {
		this->color = color;
		return *this;
	}

	/// Draw Points
	void operator=(ColorType color) {
		buffer(position) = color;
		// Experimental subpixels
		// this->x() + 1 = this->x() - 1 = this->y() + 1 = this->y() - 1 = color / 2U;
	}

	template<ColorPencil CP>
	void operator=(CP cp)
	{
		buffer(position) = cp();
	}
	
	/// Draw Lines
	template<modm::Dimension D>
	PainterFast& operator=(shape::OLine<D> oline) {
		CursorType start = buffer(position);
		CursorType end = start.template axis<D>() + oline.delta;

		if(start > end)
			std::swap(start, end);
		
		render<CursorType, ColorEmitter, detail::iterOrtho<D>>(
			start, end, std::forward<ColorEmitter>(color), {}
		);
		return *this;
	}

	PainterFast& operator=(shape::DLine dline) {
		const Point delta(dline.delta, std::signbit(dline.dir) ? -dline.delta : dline.delta);
		// TODO crop bounds

		CursorType start = buffer(position);
		CursorType end = buffer(position + delta);

		if(start > end)
			std::swap(start, end);
		
		if (dline.dir == 1) {
			render<CursorType, ColorEmitter, detail::iterOrtho<Y>, detail::iterOrtho<X, 1> >(
				start, end, std::forward<ColorEmitter>(color)
			);
		}
		else // (dline.dir == -1)
		{
			render<CursorType, ColorEmitter, detail::iterOrtho<Y>, detail::iterOrtho<X, -1>>(
				start, end, std::forward<ColorEmitter>(color)
			);
		}

		return *this;
	}

	PainterFast& operator=(shape::Line line) {
		if(line.delta.y() == 0) {
			operator=(shape::HLine(line.delta.x()));
		}
		else if(line.delta.x() == 0)
		{
			operator=(shape::VLine(line.delta.y()));
		}
		
		// TODO crop bounds

		CursorType start = buffer(position);
		CursorType end = buffer(position + line.delta);

		const Point delta_abs = {
			std::abs(line.delta.x()),
			std::abs(line.delta.y())
		};
		
		Vector<int8_t, 2> dir = {
			std::signbit(line.delta.x()) ? -1 : 1,
			std::signbit(line.delta.y()) ? -1 : 1
		};

		/* if(delta_abs.x() < delta_abs.y()) {
			// TODO draw diag
		} */

		// OPTIMIZE all these conditions :/ ... maybe have one big switch with instead?
		if(delta_abs.x() > delta_abs.y()) {
			if(dir.x() == -1) {
				dir.y() *= -1;
				std::swap(start, end);
			}

			if(dir.y() == 1)
				render<CursorType, ColorEmitter, detail::iterOrtho<X>, detail::iterBresenham<Y, 1>>(
					start, end, std::forward<ColorEmitter>(color), {}, {delta_abs}
				);
			else
				render<CursorType, ColorEmitter, detail::iterOrtho<X>, detail::iterBresenham<Y, -1>>(
					start, end, std::forward<ColorEmitter>(color), {}, {delta_abs}
				);
		}
		else
		{
			if(dir.y() == -1) {
				dir.x() *= -1;
				std::swap(start, end);
			}

			if(dir.x() == 1)
				render<CursorType, ColorEmitter, detail::iterOrtho<Y>, detail::iterBresenham<X, 1>>(
					start, end, std::forward<ColorEmitter>(color), {}, {delta_abs.swapped()}
				);
			else
				render<CursorType, ColorEmitter, detail::iterOrtho<Y>, detail::iterBresenham<X, -1>>(
					start, end, std::forward<ColorEmitter>(color), {}, {delta_abs.swapped()}
				);
		}

		return *this;
	}

	/// Draw Shapes
	PainterFast& operator=(shape::Rectangle rectangle) {
		// IMPLEMENT
/* 		CursorType start = buffer(position);
		CursorType end = buffer(position + rectangle.delta);

		if(start > end) {
			// End is on positive side of start in minor Dimension
			CursorType start2 = start + 

			render<CursorType, ColorEmitter, detail::iterOrtho<TB::Dim>>(
				start, start.template axis<, std::forward<ColorEmitter>(color), {}
			);
		} else {

		} */

		return *this;
	}

	PainterFast& operator=(shape::Circle circle) {
		// IMPLEMENT
		return *this;
	}

	// using stream operator also translates painters position. Useful for polylines.
	template<class Shape>
	PainterFast& operator<<(Shape shape) {
		operator=(shape);
		position += shape.delta;
		return *this;
	}

	PainterFast& operator<<(shape::HLine hline) {
		operator=(hline);
		position.x() += hline.delta;
		return *this;
	}

	PainterFast& operator<<(shape::VLine vline) {
		operator=(vline);
		position.y() += vline.delta;
		return *this;
	}

	PainterFast& operator<<(char* string) {
		// TODO crop bounds
		// MODM_LOG_INFO << "TODO operator<<(char* string)" << modm::endl;
		// position += char-width + char-gap
		return *this;
	}
};
}