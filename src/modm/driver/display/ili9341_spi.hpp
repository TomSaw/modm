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
protected:
	Ili9341InterfaceSpi()
	{
		this->attachConfigurationHandler([]() {
			Spi::setDataMode(Spi::DataMode::Mode0);
			Spi::setDataOrder(Spi::DataOrder::MsbFirst);
		});

		Cs::setOutput(modm::Gpio::High);
		Dc::setOutput();
	}

	modm::ResumableResult<void>
	writeCommand(Command command)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, uint8_t data)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transmit(data));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint8_t *data, std::size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transmit(data, (uint8_t*)(nullptr), length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint16_t *data, std::size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit16);
		RF_CALL(Spi::transmit(data, (uint16_t*)(nullptr), length));

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

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit16);
		RF_CALL(Spi::transmit(pixel.value()));

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

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit16);
		RF_CALL(Spi::transmit((uint16_t*)(pixel), repeat));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeData(const color::Rgb565* pixels, std::size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit16);
		RF_CALL(Spi::transmit((uint16_t*)(pixels), (uint16_t*)(nullptr), length));

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

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		read = RF_CALL(Spi::transmit(0)).getResult();

		if (this->releaseMaster())
			Cs::set();

		RF_END_RETURN(read);
	}

	modm::ResumableResult<void>
	readData(Command command, uint8_t *buffer, std::size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		if constexpr ( spi::Support_DataSize_Bit16<Spi> )
			Spi::setDataSize(Spi::DataSize::Bit8);
		Dc::reset();
		RF_CALL(Spi::transmit(uint8_t(command)));
		Dc::set();

		RF_CALL(Spi::transmit(nullptr, buffer, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

private:
	uint8_t read;
};

}  // namespace modm