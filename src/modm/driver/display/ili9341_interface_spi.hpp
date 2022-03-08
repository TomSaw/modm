/*
 * Copyright (c) 2019, Mike Wolfram
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once
#include "ili9341.hpp"

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>

#include "ili9341_defines.hpp"

namespace modm
{

// TODO Forward Nesting Level from PainterRemote: template<class Spi, class Cs, class Dc, int Levels = 4>
/// @ingroup modm_driver_ili9341
template<class Spi, class Cs, class Dc>
class Ili9341InterfaceSpi : public ili9341_register, public modm::SpiDevice< Spi >, protected modm::NestedResumable<5>
{
	uint8_t read;

	static constexpr auto configuration8bit = []() {
		Spi::setDataMode(Spi::DataMode::Mode0);
		Spi::setDataOrder(Spi::DataOrder::MsbFirst);
		Spi::setDataSize(Spi::DataSize::Bit8);
	};

	static constexpr auto configuration16bit = []() {
		Spi::setDataMode(Spi::DataMode::Mode0);
		Spi::setDataOrder(Spi::DataOrder::MsbFirst);
		Spi::setDataSize(Spi::DataSize::Bit16);
	};
protected:
	Ili9341InterfaceSpi()
	{
		this->attachConfigurationHandler(configuration16bit);
		Cs::setOutput(modm::Gpio::High);
		Dc::setOutput();
	}

	modm::ResumableResult<void>
	writeCommand(Command command)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(Spi::transfer(uint16_t(command))); // It's Ok to send sole commands in 16bit
		Dc::set();

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint8_t data)
	{
		RF_BEGIN();
		this->attachConfigurationHandler(configuration8bit);
		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(Spi::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transfer(data));

		if (this->releaseMaster())
			Cs::set();

		this->attachConfigurationHandler(configuration16bit);
		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint8_t *data, uint16_t length)
	{
		RF_BEGIN();
		this->attachConfigurationHandler(configuration8bit);
		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(Spi::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transfer(data, (uint8_t*)(nullptr), length));

		if (this->releaseMaster())
			Cs::set();

		this->attachConfigurationHandler(configuration16bit);
		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint16_t *data, uint16_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		// It's OK to send sole 8bit commands in 16bit
		RF_CALL(Spi::transfer(uint16_t(command)));
		Dc::set();

		RF_CALL(Spi::transfer(data, (uint16_t*)(nullptr), length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeData(color::Rgb565 pixel)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(Spi::transfer(pixel.value()));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeDataRepeat(const color::Rgb565* pixel, uint16_t repeat)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(Spi::transfer((uint16_t*)(pixel), repeat));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeData(const color::Rgb565* pixels, uint16_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(Spi::transfer((uint16_t*)(pixels), (uint16_t*)(nullptr), length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<uint8_t>
	readData(Command command)
	{
		RF_BEGIN();
		
		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(Spi::transfer(uint8_t(command)));
		Dc::set();

		read = RF_CALL(Spi::transfer(0x00)).getResult();
		
		if (this->releaseMaster())
			Cs::set();

		RF_END_RETURN(read);
	}

	modm::ResumableResult<void>
	readData(Command command, uint8_t *buffer, uint16_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(Spi::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transfer(nullptr, buffer, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}
};

}  // namespace modm