#pragma once

#include "buffer_memory_definition.hpp"
#include <modm/math/geometry/shape.hpp>
#include <modm/ui/color/rgb_html.hpp>

#include "pencils.hpp"
#include "renderer_plain.hpp"

namespace modm::graphic {

/**
 * @brief 		Simple and fast Painter for Line, Rectangle, Circle, Ellipse
 */
template<class TB>
class PainterFast {
public:
	using ColorType = TB::ColorType;
	using CursorType = TB::CursorType;

private:
	TB& buffer;
	Point position;

	ColorType color{color::html::White};

	template<modm::Dimension D>
	void ortholine(uint16_t delta) {
		using ColorEmitter = ColorType;

		// TODO crop bounds
		CursorType start = buffer(position);
		CursorType end = start.template axis<D>() + delta;

		if(start > end)
			std::swap(start, end);
		
		render<CursorType, ColorEmitter, detail::iterOrtho<D>>(
			start, end,
			std::forward<ColorEmitter>(color)
		);
	}

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
	PainterFast& operator=(shape::HLine hline) {
		ortholine<modm::Dimension::Row>(hline.delta);
		return *this;
	}

	PainterFast& operator=(shape::VLine vline) {
		ortholine<modm::Dimension::Col>(vline.delta);
		return *this;
	}

	PainterFast& operator=(shape::DLine dline) {
		using ColorEmitter = ColorType;
		
		const Point delta(dline.delta, std::signbit(dline.dir) ? -dline.delta : dline.delta);
		// TODO crop bounds

		CursorType start = buffer(position);
		CursorType end = buffer(position + delta);

		if(start > end)
			std::swap(start, end);
		
		render<CursorType, ColorEmitter, detail::iterDiag<modm::Dimension::Row>>(
			start, end,
			std::forward<ColorEmitter>(color),
			{dline.dir}
		);

		return *this;
	}

	PainterFast& operator=(shape::Line line) {
		using ColorEmitter = ColorType;

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

		const uint16_t x_abs = std::abs(line.delta.x());
		const uint16_t y_abs = std::abs(line.delta.y());
		
		int x_dir = std::signbit(line.delta.x()) ? -1 : 1;
		int y_dir = std::signbit(line.delta.y()) ? -1 : 1;

		// color = color::html::Gray;

		if(x_abs < y_abs) {
			if(y_dir == -1) {
				x_dir *= -1;
				std::swap(start, end);
			}

			render<CursorType, ColorEmitter, detail::iterBresenham<Dimension::Col>>(
				start, end,
				std::forward<ColorEmitter>(color),
				{x_dir, y_abs, x_abs}
			);
		}
		else
		{
			if(x_dir == -1) {
				y_dir *= -1;
				std::swap(start, end);
			}

			render<CursorType, ColorEmitter, detail::iterBresenham<Dimension::Row>>(
				start, end,
				std::forward<ColorEmitter>(color),
				{y_dir, x_abs, y_abs}
			);
		}

		// start = color::html::White;
		// end = color::html::Black;
		
		return *this;
	}

	// using operator<< also translates position.
	// Good for drawing polylines
	// TODO needs testing
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

	PainterFast& operator<<(shape::DLine dline) {
		operator=(dline);
		position += dline.delta;
		return *this;
	}

	PainterFast& operator<<(shape::Line line) {
		operator=(line);
		position += line.delta;
		return *this;
	}

	/// Draw Shapes
	PainterFast& operator=(shape::Rectangle rectangle) {
		// TODO crop bounds
		// MODM_LOG_INFO << "TODO operator<<(shape::Rectangle rectangle)" << modm::endl;
		return *this;
	}

	PainterFast& operator=(shape::Circle circle) {
		// TODO crop bounds
		// MODM_LOG_INFO << "TODO operator<<(shape::Circle circle)" << modm::endl;
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