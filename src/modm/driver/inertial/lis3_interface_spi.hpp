/*
 * Copyright (c) 2014-2016, Niklas Hauser
 * Copyright (c) 2021		Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{
/**
 * LIS3xx SPI Transport Layer.
 *
 * This class manages communication with the accelerometer via the SPI bus.
 * The SPI interface can be clocked with up to 10MHz and requires Mode3.
 *
 * @see Lis302dl
 * @see Lis3dsh
 *
 * @tparam	Cs	connected Chip Select Pin
 *
 * @ingroup modm_driver_lis3_transport
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Lis3InterfaceSpi : public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<2>
{
public:
	Lis3InterfaceSpi(uint8_t /*address*/) : whoAmI(0)
	{ Cs::setOutput(modm::Gpio::High); }

	/// pings the sensor
	modm::ResumableResult<bool>
	ping() {
		RF_BEGIN();

		whoAmI = 0;

		RF_CALL(read(0x0F, whoAmI));

		RF_END_RETURN(whoAmI != 0);
	}

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value) {
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(SpiMaster::transfer(reg | Write));
		RF_CALL(SpiMaster::transfer(value));

		if (this->releaseMaster())
			Cs::set();

		RF_END_RETURN(true);
	}

	/// read a 8bit value
	modm::ResumableResult<bool> modm_always_inline
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length) {
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(SpiMaster::transfer(reg | Read));

		RF_CALL(SpiMaster::transfer(nullptr, buffer, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END_RETURN(true);
	}

	// increment address or not?
	/// @cond
	static constexpr uint8_t AddressIncrement = 0x40;
	static constexpr uint8_t AddressStatic = 0x00;
	/// @endcond

private:
	uint8_t whoAmI;

	// write read bit on the address
	static constexpr uint8_t Read = 0x80;
	static constexpr uint8_t Write = 0x00;
};

} // namespace modm