#ifndef MODM_COLOR_GRAPHIC_DISPLAY_HPP
#define MODM_COLOR_GRAPHIC_DISPLAY_HPP

#include <modm/platform.hpp>

#include "graphic_display.hpp"

using namespace modm::platform;

namespace modm
{

template <uint16_t Width, uint16_t Height>
class ColorGraphicDisplay : public GraphicDisplay<Width, Height>
{
public:
	ColorGraphicDisplay() : foregroundColor(color::html::White), backgroundColor(color::html::Black)
	{}

	color::Rgb565
	getPixel(glcd::Point pos) const {
		if(this->pointOnScreen(pos))
			return getPixelFast(pos);
		else
			return this->backgroundColor;
	};

	/**
	 * Set a new foreground color.
	 * Used for drawing operations. Default is white.
	 */
	inline void
	setColor(const color::Rgb565 color)
	{
		foregroundColor = color;
	}

	inline color::Rgb565
	getColor() const
	{
		return foregroundColor;
	}

	/**
	 * Set background color.
	 * Used when clearing the screen. Default is black.
	 */
	inline void
	setBackgroundColor(const color::Rgb565 color)
	{
		backgroundColor = color;
	}

	inline color::Rgb565
	getBackgroundColor() const
	{
		return backgroundColor;
	}

protected:
	virtual color::Rgb565
	getPixelFast(glcd::Point pos) const = 0;

	color::Rgb565 foregroundColor;
	color::Rgb565 backgroundColor;
};
}  // namespace modm

#endif  // MODM_COLOR_GRAPHIC_DISPLAY_HPP
