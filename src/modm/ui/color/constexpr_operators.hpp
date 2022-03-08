/*
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "concepts.hpp"

/**
 * @brief		A few useful compile time operators
 *
 * @example		const auto purple = modm::color::Red + modm::color::blue;
 *
 * @author 		Thomas Sommer
 * @ingroup 	modm_ui_color
 */
template <class C>
requires modm::color::ColorRgb<C> || modm::color::ColorRgbStacked<C>
constexpr C operator+(C const& lrgb, C const& rrgb) {
	return {
		lrgb.red() + rrgb.red(),
		lrgb.green() + rrgb.green(),
		lrgb.blue() + rrgb.blue()
	};
}

template <class C>
requires modm::color::ColorRgb<C> || modm::color::ColorRgbStacked<C>
constexpr C operator-(C const& lrgb, C const& rrgb) {
	return {
		lrgb.red() - rrgb.red(),
		lrgb.green() - rrgb.green(),
		lrgb.blue() - rrgb.blue()
	};
}

template <class C>
requires modm::color::ColorRgb<C> || modm::color::ColorRgbStacked<C>
constexpr C operator*(C const& lrgb, C const& rrgb) {
	return {
		lrgb.red() * rrgb.red(),
		lrgb.green() * rrgb.green(),
		lrgb.blue() * rrgb.blue()
	};
}

template <class C>
requires modm::color::ColorRgb<C> || modm::color::ColorRgbStacked<C>
constexpr C operator/(C const& lrgb, C const& rrgb) {
	return {
		lrgb.red() / rrgb.red(),
		lrgb.green() / rrgb.green(),
		lrgb.blue() / rrgb.blue()
	};
}