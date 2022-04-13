#pragma once

#include <modm/ui/graphic/concepts.hpp>
#include <modm/math/geometry/shape.hpp>

#include "concepts.hpp"
#include "pencils.hpp"
#include "renderer_plain.hpp"
// #include "renderer_palletized.hpp"

namespace modm::graphic {

namespace painter {

/// Experimental, very fast line painter working on Cursors
/// Is Affected by 'MajorDim-Cursor-Issue'
template<class CursorType, class ColorEmitter>
requires color::Color<ColorEmitter> || ColorPencil<ColorEmitter>
void
drawLine(CursorType start, CursorType end, auto& buffer, ColorEmitter&& ce)
{
	using diff_type = std::ptrdiff_t;

	if (start > end) std::swap(start, end);

	const diff_type diff = &end - &start;
	const diff_type remMajor = diff % buffer.minorFreq;

	if (remMajor == 0)
	{
		// Minor line
		render<CursorType, ColorEmitter, detail::iterOrtho<modm::Dimension::Col>>(
			start, end,
			std::forward<ColorEmitter>(ce)
		);
		return;
	}

	const diff_type diffMajor_end = (&end - buffer.data()) % buffer.minorFreq;
	const int dir = remMajor > diffMajor_end ? -1 : 1;

	if (remMajor == diff && dir == 1)
	{
		// Major line
		render<CursorType, ColorEmitter, detail::iterOrtho<modm::Dimension::Row>>(
			start, end,
			std::forward<ColorEmitter>(ce)
		);
		return;
	}

	const uint16_t diffMajor = (dir == -1) ? buffer.minorFreq - remMajor: remMajor;
	const uint16_t diffMinor = (dir == -1) ? diff / buffer.minorFreq + 1 : diff / buffer.minorFreq;

	if (diffMinor == diffMajor) {
		// Diagonal line
		render<CursorType, ColorEmitter, detail::iterDiag<modm::Dimension::Row>>(
			start, end,
			std::forward<ColorEmitter>(ce),
			{dir}
		);
		return;
	}

	// Bresenham line
 	if(diffMajor > diffMinor)
		render<CursorType, ColorEmitter, detail::iterBresenham<Dimension::Row>>(
			start, end,
			std::forward<ColorEmitter>(ce),
			{dir, diffMajor, diffMinor}
		);
	else
		render<CursorType, ColorEmitter, detail::iterBresenham<Dimension::Col>>(
			start, end,
			std::forward<ColorEmitter>(ce),
			{dir, diffMinor, diffMajor}
		);
}

} // painter

	
} // namespace modm::graphic

// #include "painter_impl.hpp"