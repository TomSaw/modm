#pragma once

#include <modm/math/geometry/section.hpp>

namespace modm::graphic {

/**
 * @brief 		Simple and fast Painter for Line, Rectangle, Circle, Ellipse
 */
template<class TB>
class View {
	TB& buffer;
	Section section;
public:
	// IMPLEMENT
	// SpanType colspan();
	// SpanType rowspan();

};

} // namespace modm::graphic