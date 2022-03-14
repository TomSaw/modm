#pragma once
#include <modm/ui/color/concepts.hpp>

namespace modm::color {
	/**
	 * @brief 	Concept to ident palettizing colortype instances
	 *			Targets: Monochrome, Gray2, Gray4
	 *
	 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 */
	template<class C>
	concept ColorPalletized = ColorGray<C> and C::Digits < 8 and std::popcount(unsigned(C::Digits)) == 1;

	/**
	 * @brief 	Concept to ident planar colortype instances
	 *
	 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
	 */
	template<class C>
	concept ColorPlanar = !ColorPalletized<C>;

	/**
	 * @brief 	Concept to ident monochrome colortype
	 */
	template<class C>
	concept ColorMonochrome = std::is_same_v<C, Monochrome>;
	// concept ColorMonochrome = ColorGray<C> and C::Digits == 1; // Alternative implementation
}
namespace modm::graphic
{



template<class B>
concept GraphicBuffer = color::Color<typename B::ColorType>;
// TODO check B for existence of Size

template<class D>
concept GraphicDisplay = color::Color<typename D::ColorType>;
// TODO check D for existence of Size

/**
 * @brief 	Function or functor comcept, returning a Color for each shape::Point you pass
 *
 * @author 	Thomas Sommer
 */
template<class F>
concept ColorPattern = requires(F p)
{
	{ p.operator()(shape::Point()) } -> color::Color;  // -> std::convertible_to<color::Rgb888>;
};

}  // namespace modm::graphic