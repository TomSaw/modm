/*
 * Copyright (c) 2019, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef MODM_ILI9341_SPI_HPP
#define MODM_ILI9341_SPI_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/platform/spi/spi_base.hpp>

#include "ili9341.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class SPI, class Cs, class Dc>
class Ili9341SPIInterface : public ili9341, public modm::SpiDevice<SPI>
{
public:
	Ili9341SPIInterface()
	{
		this->attachConfigurationHandler([]() {
			SPI::setDataMode(SPI::DataMode::Mode0);
			SPI::setDataOrder(SPI::DataOrder::MsbFirst);
		});
		Cs::setOutput(modm::Gpio::High);
		Dc::setOutput();
	}

	inline void
	writeCommand(Command command) const
	{
		Dc::reset();  // enable command
		SPI::transferBlocking(uint8_t(command));
		Dc::set();  // reset to data
	}

	inline void
	writeCommand(Command command, const uint8_t *data, std::size_t length) const
	{
		writeCommand(command);
		if (length != 0) { SPI::transferBlocking(data, nullptr, length); }
	}

	inline void
	writeData(uint8_t data) const
	{
		SPI::transferBlocking(data);
	}

	inline void
	writeData(color::Rgb565 rgb565) const
	{
		SPI::transferBlocking(rgb565.color >> 8);
		SPI::transferBlocking(rgb565.color);
	}

	template<typename T>
	void
	writeData(T data, size_t length) const
	{
		while (length--) writeData(data);
	}

	inline void
	writeData(const uint8_t *data, std::size_t length) const
	{
		SPI::transferBlocking(data, nullptr, length);
	}

	void
	readData(Command command, uint8_t *buffer, std::size_t length)
	{
		using modm::platform::SpiBase;
		uint8_t b[4];

		Dc::reset();  // enable command
		// SPI::Hal::setDataSize(SpiBase::DataSize::Bit9);
		SPI::transferBlocking(uint8_t(command) << 1);
		SPI::Hal::setDataSize(SpiBase::DataSize::Bit8);
		Dc::set();  // reset to data
		SPI::transferBlocking(b /*nullptr*/, buffer, length);
	}

	uint8_t
	readData(Command command)
	{
		writeCommand(command);
		return SPI::transferBlocking(0x00);
	}

public:
	struct BatchHandle
	{
		Ili9341SPIInterface &i;
		BatchHandle(Ili9341SPIInterface &iface) : i(iface)
		{
			i.acquireMaster();
			Cs::reset();
		}
		~BatchHandle()
		{
			if (i.releaseMaster()) Cs::set();
		}
	};
};

/// @ingroup modm_driver_ili9341
template<class SPI, class Cs, class Dc, class Reset, class Backlight>
using Ili9341Spi = Ili9341<Ili9341SPIInterface<SPI, Cs, Dc>, Reset, Backlight>;

}  // namespace modm

#endif  // MODM_ILI9341_SPI_HPP
