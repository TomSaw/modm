/*
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_L3GD20_HPP
#	error  "Don't include this file directly, use 'l3gd20.hpp' instead!"
#endif

#include <cstring>

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class Transport >
modm::L3gd20<Transport>::L3gd20(Data &data, uint8_t address)
:	Transport(address), data(data)
{
	// Power-up default
	control.control1 = Control1::XEN | Control1::YEN | Control1::ZEN;
}

template < class Transport >
modm::ResumableResult<bool>
modm::L3gd20<Transport>::readRotation()
{
	RF_BEGIN();
	RF_END_RETURN_CALL(this->read(uint8_t(Register::OUT_TEMP) | Transport::AddressIncrement, data.raw, 8));
}

template < class Transport >
modm::ResumableResult<bool>
modm::L3gd20<Transport>::updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask)
{
	RF_BEGIN();

	raw[index] &= ~clearMask.value;
	raw[index] |= setMask.value;
	
	RF_END_RETURN_CALL(this->write(0x20 + index, raw[index]));
}

template < class Transport >
modm::ResumableResult<bool>
modm::L3gd20<Transport>::updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	RF_BEGIN();

	if (RF_CALL(this->read(reg, buffer)))
	{
		buffer &= ~clearMask;
		buffer |= setMask;
		RF_RETURN_CALL(this->write(reg, buffer));
	}

	RF_END_RETURN(false);
}
