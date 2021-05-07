/*
 * Copyright (c) 2019, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_ILI9341_HPP
#error "Don't include this file directly, use 'ili9341.hpp' instead!"
#endif

#include <bit>

namespace modm
{

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::initialize()
{
	constexpr uint8_t pwrCtrlA[]{0x39, 0x2c, 0x00, 0x34, 0x02};
	constexpr uint8_t pwrCtrlB[]{0x00, 0xc1, 0x30};
	constexpr uint8_t timCtrlA[]{0x85, 0x10, 0x7a};
	constexpr uint8_t timCtrlB[]{0x00, 0x00};
	constexpr uint8_t pwrOnSeqCtrl[]{0x64, 0x03, 0x12, 0x81};
	constexpr uint8_t pwrCtrl1[]{0x1b};
	constexpr uint8_t pwrCtrl2[]{0x12};
	constexpr uint8_t vcomCtrl1[]{0x08, 0x26};  // 0x3e, 0x28 };
	constexpr uint8_t vcomCtrl2[]{0xb7};
	// constexpr uint8_t pumpRatioCtrl[] { 0x20 };
	constexpr uint8_t pixelFormat[]{0x55};
	constexpr uint8_t frameCtrl[]{0x00, 0x1a};
	constexpr uint8_t dispFuncCtrl[]{0x0a, 0xa2, 0x27, 00};  //{ 0x08, 0x82, 0x27, 0x00 };
	constexpr uint8_t enable3G[]{0x00};
	constexpr uint8_t gammaSet[]{0x01};
	//	constexpr uint8_t positiveGammaCorr[] { 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08,
	//			0xe4, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 };
	//	constexpr uint8_t negativeGammaCorr[] { 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07,
	//			0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f };
	constexpr uint8_t positiveGammaCorr[]{0x0f, 0x1d, 0x1a, 0x0a, 0x0d, 0x07, 0x49, 0x66,
										  0x3b, 0x07, 0x11, 0x01, 0x09, 0x05, 0x04};
	constexpr uint8_t negativeGammaCorr[]{0x00, 0x18, 0x1d, 0x02, 0x0f, 0x04, 0x36, 0x13,
										  0x4c, 0x07, 0x13, 0x0f, 0x2e, 0x2f, 0x05};

	reset();

	{
		BatchHandle h(*this);

		this->writeCommand(Command::DisplayOff);

		this->writeCommand(Command::PowerCtrlA, pwrCtrlA, sizeof(pwrCtrlA));
		this->writeCommand(Command::PowerCtrlB, pwrCtrlB, sizeof(pwrCtrlB));
		this->writeCommand(Command::TimingCtrlA, timCtrlA, sizeof(timCtrlA));
		this->writeCommand(Command::TimingCtrlB, timCtrlB, sizeof(timCtrlB));
		this->writeCommand(Command::PowerOnSequenceCtrl, pwrOnSeqCtrl, sizeof(pwrOnSeqCtrl));
		this->writeCommand(Command::PowerCtrl1, pwrCtrl1, sizeof(pwrCtrl1));
		this->writeCommand(Command::PowerCtrl2, pwrCtrl2, sizeof(pwrCtrl2));
		this->writeCommand(Command::VComCtrl1, vcomCtrl1, sizeof(vcomCtrl1));
		this->writeCommand(Command::VComCtrl2, vcomCtrl2, sizeof(vcomCtrl2));
		this->writeCommand(Command::PixelFormatSet, pixelFormat, sizeof(pixelFormat));
		this->writeCommand(Command::FrameCtrlNormalMode, frameCtrl, sizeof(frameCtrl));
		this->writeCommand(Command::DisplayFunctionCtrl, dispFuncCtrl, sizeof(dispFuncCtrl));
		this->writeCommand(Command::Enable3Gamma, enable3G, sizeof(enable3G));
		this->writeCommand(Command::GammaSet, gammaSet, sizeof(gammaSet));
		this->writeCommand(Command::PositiveGammaCorrection, positiveGammaCorr,
						   sizeof(positiveGammaCorr));
		this->writeCommand(Command::NegativeGammaCorrection, negativeGammaCorr,
						   sizeof(negativeGammaCorr));

		this->writeCommand(Command::LeaveSleep);
		modm::delay(120ms);
		this->writeCommand(Command::InversionOff);
		this->writeCommand(Command::DisplayOn);

		setOrientation(orientation);
	}
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::reset(bool hardReset /* = false */)
{
	if (hardReset)
	{
		Reset::set();
		modm::delay(5ms);
		Reset::reset();
		modm::delay(5ms);
		Reset::set();
		modm::delay(5ms);
	} else
	{
		BatchHandle h(*this);
		this->writeCommand(Command::SwReset);
		modm::delay(5ms);
	}
}

template<class Interface, class Reset, class Backlight>
uint16_t
Ili9341<Interface, Reset, Backlight>::getIcModel()
{
	BatchHandle h(*this);

	uint8_t buffer[4]{0};
	this->readData(Command::ReadId4, buffer, 4);
	return (buffer[2] << 8) | buffer[3];
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setOrientation(glcd::Orientation orientation)
{
	using MemoryAccessCtrl_t = ili9341::MemoryAccessCtrl_t;
	using MemoryAccessCtrl = ili9341::MemoryAccessCtrl;

	MemoryAccessCtrl_t madCtrl{MemoryAccessCtrl::BGR};

	switch (orientation)
	{
		case glcd::Orientation::Portrait90:
			madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MX;
			break;
		case glcd::Orientation::Landscape180:
			madCtrl |= MemoryAccessCtrl::MX | MemoryAccessCtrl::MY;
			break;
		case glcd::Orientation::Portrait270:
			madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MY;
			break;
		default:
			//			madCtrl |= ili9341::MemoryAccessCtrl::ML;
			break;
	}

	this->orientation = orientation;

	BatchHandle h(*this);
	this->writeCommand(Command::MemoryAccessCtrl);
	this->writeData(madCtrl.value);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::turnOn()
{
	BatchHandle h(*this);
	this->writeCommand(Command::DisplayOn);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::turnOff()
{
	BatchHandle h(*this);
	this->writeCommand(Command::DisplayOff);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setIdle(bool enable)
{
	BatchHandle h(*this);
	this->writeCommand(enable ? Command::IdleModeOn : Command::IdleModeOff);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::enableSleep(bool enable)
{
	BatchHandle h(*this);
	this->writeCommand(enable ? Command::EnterSleep : Command::LeaveSleep);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setBrightness(uint8_t level)
{
	BatchHandle h(*this);
	this->writeCommand(Command::WriteBrightness, &level, 1);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setInvert(bool invert)
{
	BatchHandle h(*this);
	this->writeCommand(invert ? Command::InversionOn : Command::InversionOff);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::clear()
{
	auto const saveForegroundColor{foregroundColor};
	foregroundColor = backgroundColor;
	fillRectangle(glcd::Point(0, 0), this->getWidth(), this->getHeight());
	foregroundColor = saveForegroundColor;
}

template<class Interface, class Reset, class Backlight>
inline void
Ili9341<Interface, Reset, Backlight>::drawHorizontalLine(glcd::Point start, int16_t length)
{
	fillRectangle(start, length, 1);
}

template<class Interface, class Reset, class Backlight>
inline void
Ili9341<Interface, Reset, Backlight>::drawVerticalLine(glcd::Point start, int16_t length)
{
	fillRectangle(start, 1, length);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::fillRectangle(glcd::Point pos, int16_t width,
													int16_t height)
{
	BatchHandle h(*this);
	setClipping(pos, width, height);
	this->writeData(foregroundColor, width * height);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::fillCircle(glcd::Point center, int16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	BatchHandle h(*this);

	drawHorizontalLine(glcd::Point{int16_t(center.x - radius), center.y}, 2 * y);
	while (x++ < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		ddF_x += 2;
		f += ddF_x + 1;

		drawHorizontalLine(glcd::Point{int16_t(center.x - x), int16_t(center.y - y)}, 2 * x);
		drawHorizontalLine(glcd::Point{int16_t(center.x - y), int16_t(center.y - x)}, 2 * y);
		drawHorizontalLine(glcd::Point{int16_t(center.x - x), int16_t(center.y + y)}, 2 * x);
		drawHorizontalLine(glcd::Point{int16_t(center.x - y), int16_t(center.y + x)}, 2 * y);
	}
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::drawImageRaw(glcd::Point pos, uint16_t width,
												   uint16_t height,
												   modm::accessor::Flash<uint8_t> data)
{
	BatchHandle h(*this);

	setClipping(pos, width, height);

	// OPTIMIZE like in MonochromeGraphicDisplayVertical
	uint8_t bit = Bit0;
	for (uint16_t r = 0; r < height; ++r)
	{
		for (uint16_t w = 0; w < width; ++w)
		{
			uint8_t byte = data[(r / 8) * width + w];
			if (byte & bit)
				this->writeData(foregroundColor);
			else
				this->writeData(backgroundColor);
		}
		bit = std::rotl(bit, 1);
	}
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::drawRaw(glcd::Point pos, uint16_t width,
											  uint16_t height, color::Rgb565 *data)
{
	BatchHandle h(*this);

	setClipping(pos, width, height);
	for (uint16_t i = 0; i < width * height; i++) { this->writeData(data[i]); }
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setScrollArea(uint16_t topFixedRows, uint16_t bottomFixedRows,
													uint16_t firstRow)
{
	BatchHandle h(*this);

	uint8_t arg[]{uint8_t(topFixedRows >> 8),    uint8_t(topFixedRows),
				  uint8_t(firstRow >> 8),        uint8_t(firstRow),
				  uint8_t(bottomFixedRows >> 8), uint8_t(bottomFixedRows)};
	this->writeCommand(Command::VerticalScrollDefinition, arg, 6);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::scrollTo(uint16_t row)
{
	BatchHandle h(*this);

	uint8_t arg[]{uint8_t(row >> 8), uint8_t(row)};
	this->writeCommand(Command::VerticalScrollStartAddr, arg, 2);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::setClipping(glcd::Point start, int16_t width,
												  int16_t height)
{
	uint8_t buffer[4];

	const uint16_t xw = start.x + width - 1;
	buffer[0] = start.x >> 8;
	buffer[1] = start.x;
	buffer[2] = xw >> 8;
	buffer[3] = xw;
	this->writeCommand(Command::ColumnAddressSet, buffer, 4);

	const uint16_t yh = start.y + height - 1;
	buffer[0] = start.y >> 8;
	buffer[1] = start.y;
	buffer[2] = yh >> 8;
	buffer[3] = yh;

	this->writeCommand(Command::PageAddressSet, buffer, 4);
	this->writeCommand(Command::MemoryWrite);
}

template<class Interface, class Reset, class Backlight>
void
Ili9341<Interface, Reset, Backlight>::drawBitmap(glcd::Point pos, uint16_t width,
												 uint16_t height,
												 modm::accessor::Flash<uint8_t> data)
{
	uint8_t buffer[2];
	BatchHandle h(*this);

	setClipping(pos, width, height);
	for (int i = 0; i < width * height; ++i)
	{
		buffer[0] = data[i * 2 + 1];
		buffer[1] = data[i * 2];
		this->writeData(buffer, 2);
	}
	//	this->writeData(data.getPointer(), width * height * 2);
}

}  // namespace modm
