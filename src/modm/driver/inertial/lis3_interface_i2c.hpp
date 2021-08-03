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
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * LIS3xx I2C Transport Layer.
 *
 * This class manages communication with the accelerometer via the I2C bus.
 *
 * To enable I2C mode in the LIS3xx devices, the CS pin must be tied high.
 * However, it is internally pulled-up, so just leaving the CS pin disconnected should work too.
 *
 * The I2C interface is compliant with Fast Mode (up to 400kHz).
 *
 * @see Lis302dl
 * @see Lis3dsh
 *
 * @ingroup modm_driver_lis3_transport
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Lis3InterfaceI2c : public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Lis3InterfaceI2c(uint8_t address) :	I2cDevice<I2cMaster, 2>(address)
	{}

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value) {
		RF_BEGIN();

		buffer[0] = reg;
		buffer[1] = value;

		this->transaction.configureWrite(buffer, 2);

		RF_END_RETURN_CALL( this->runTransaction() );
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

		this->buffer[0] = reg;
		this->transaction.configureWriteRead(this->buffer, 1, buffer, length);

		RF_END_RETURN_CALL( this->runTransaction() );
	}

	// increment address or not?
	/// @cond
	static constexpr uint8_t AddressIncrement = 0x80;
	static constexpr uint8_t AddressStatic = 0x00;
	/// @endcond

private:
	uint8_t buffer[2];
};

} // namespace modm
