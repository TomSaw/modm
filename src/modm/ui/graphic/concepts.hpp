#pragma once
#include <modm/ui/color/concepts.hpp>

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