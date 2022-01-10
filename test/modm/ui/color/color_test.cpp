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

#include "color_test.hpp"
#include <modm/ui/color.hpp>
#include <modm/math/tolerance.hpp>
#include <modm/debug/logger.hpp>
#include <algorithm>

using namespace modm;
using namespace modm::color;

void ColorTest::testGray() {
	Gray8 gray8(127);
	Gray8 gray8_B(gray8);
	TEST_ASSERT_EQUALS(gray8, gray8_B);

	Gray4 gray4(7);

	gray4 += 3;
	TEST_ASSERT_EQUALS(gray4, 10U);

	gray4 -= 1;
	TEST_ASSERT_EQUALS(gray4, 9U);

	gray4 -= 44; // under-saturation
	TEST_ASSERT_EQUALS(gray4, 0U);

	gray4 += 66; // over-saturation
	TEST_ASSERT_EQUALS(gray4, (unsigned int)(0b1111));

	gray4 -= 3;
	TEST_ASSERT_EQUALS(gray4, (unsigned int)(0b1100));

	// IMPLEMENT -= -
	// gray4 -= -2;

	// IMPLEMENT += -
	// gray4 += -3;

	gray8 = gray4; // upscaling
	TEST_ASSERT_EQUALS(gray8, (unsigned int)(0b11001100));

	GrayD<13> gray13 = gray4; // further upscaling
	TEST_ASSERT_EQUALS(gray13, (unsigned int)(0b0001100110011000)); // last digit rounds down for odd D

	gray4 = gray13; // downscaling
	TEST_ASSERT_EQUALS(gray4, (unsigned int)(0b00001100));
}

void ColorTest::testRgb() {
	Rgb888 rgb888_A(0, 100, 200);
	Rgb888 rgb888_B(rgb888_A);
	TEST_ASSERT_EQUALS(rgb888_A, rgb888_B);

	Rgb161616 rgb16(rgb888_A);

	Rgb888 rgb888_C(rgb16);
	TEST_ASSERT_EQUALS(rgb888_A, rgb888_C);

	Rgb888 rgb888_d(1, 2, 3);

	rgb888_A += rgb888_d;
	TEST_ASSERT_EQUALS(rgb888_A, Rgb888(1, 102, 203));

	Rgb666 rgb666_A(1, 2, 3);
}

void ColorTest::testHsv() {
	Hsv888 hsv888(html::Orchid);
	Hsv888 hsv888_B(hsv888);
	TEST_ASSERT_EQUALS(hsv888, hsv888_B);

	Hsv161616 hsv161616(hsv888);

	Hsv888 hsv888_C(hsv161616);
	TEST_ASSERT_EQUALS(hsv888, hsv888_C);
}

void ColorTest::testConvertion_8bit()
{
	Rgb888 rgb(124, 128, 10);

	Hsv888 hsv(rgb);
	TEST_ASSERT_EQUALS(hsv.hue(), 43);
	TEST_ASSERT_EQUALS(hsv.saturation(), 235);
	TEST_ASSERT_EQUALS(hsv.value(), 128);

	Gray8 gray(rgb);
	TEST_ASSERT_EQUALS(gray, 118);
}

void ColorTest::testConvertion_16bit()
{
	Rgb888 rgb888(html::Orchid);
	Hsv888 hsv888(rgb888);
	Hsv161616 hsv161616(hsv888);

	Rgb161616 rgb16(rgb888);
	Hsv161616 hsv161616_B(rgb16);

	// Test, if rgb->hsv conversion produces the same result for 8 and 16bits
	// FIXME test fails
	// TEST_ASSERT_EQUALS(hsv161616, hsv161616_B);
}

void ColorTest::testRgbHsvPingPongConvertion_8bit()
{
	Rgb888 rgb888(html::Orchid);
	Hsv888 hsv888(rgb888);
	Rgb888 rgb888_B(hsv888);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb888.red(), rgb888_B.red(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb888.green(), rgb888_B.green(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb888.blue(), rgb888_B.blue(), 1_pct));
}

void ColorTest::testRgbHsvPingPongConvertion_16bit()
{
	// Rgb->Hsv->Rgb, both 16 bit
	Rgb161616 rgb16(html::Orchid);
	Hsv161616 hsv16(rgb16);
	Rgb161616 rgb16_B(hsv16);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.red(), rgb16_B.red(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.green(), rgb16_B.green(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.blue(), rgb16_B.blue(), 1_pct));
}