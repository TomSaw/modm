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
#pragma once
#include "buffer_mal_planar.hpp"

namespace modm::graphic {

template<color::ColorPlanar C, shape::Size R>
void
BufferMal<C, R>::drawBlind(const shape::Point& point)
{
	buffer[point.y()][point.x()] = this->color;
}

template<color::ColorPlanar C, shape::Size R>
void
// TODO make const HLine ... but benchmark with const and
BufferMal<C, R>::drawBlind(const shape::HLine& hline)
{
	std::fill(&buffer[hline.start.y()][hline.start.x()], &buffer[hline.start.y()][hline.end_x], this->color);
}

template<color::ColorPlanar C, shape::Size R>
void
BufferMal<C, R>::drawBlind(const shape::VLine& vline)
{
	shape::Point scanner = vline.start;
	while (scanner.y() < vline.end_y)
	{
		drawBlind(scanner);
		scanner.y()++;
	}
}

template<color::ColorPlanar C, shape::Size R>
void
BufferMal<C, R>::drawBlind(const shape::Section& section)
{
	shape::Point scanner = section.topLeft;
	while (scanner.y() < section.bottomRight.y())
	{
		this->drawBlind(shape::HLine(scanner, section.bottomRight.x()));
		scanner.y()++;
	}
}

template<color::ColorPlanar C, shape::Size R>
template<color::Color CO, template<typename> class Accessor>
void
BufferMal<C, R>::writeImage(ImageAccessor<CO, Accessor> accessor)
{
	const shape::Section clipping = this->getIntersection(accessor.getSection());

	shape::Point scanner = clipping.topLeft;
	while (scanner.x() < clipping.bottomRight.x())
	{
		accessor.incrementRow_preparePixel();
		while (scanner.y() < clipping.bottomRight.y())
		{
			if constexpr (color::ColorPalletized<CO>) {
				// Apply colormap
				buffer[scanner.y()][scanner.x()] = this->colormap[ accessor.nextPixel().value() ];
			} else {
				// convert color
				buffer[scanner.y()][scanner.x()] = accessor.nextPixel();
			}
			scanner.y()++;
		}
		scanner.x()++;
		scanner.y() = clipping.topLeft.y();
	}
}

template<color::ColorPlanar C, shape::Size R>
template<template<typename> class Accessor>
void
BufferMal<C, R>::writeImage(ImageAccessor<C, Accessor> accessor)
{
	// IMPLEMENT. see ili9341 equivalent

	// std::copy
}

} // namespace modm::graphic