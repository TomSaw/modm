/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "sh1106.hpp"

namespace modm {

template<class I2cMaster, uint16_t H>
ResumableResult<bool>
Sh1106<I2cMaster, H>::initializeMemoryMode()
{
	RF_BEGIN();
	// Default on Power-up - can be omitted
	this->commandBuffer[0] = ssd1306::AdressingCommands::MemoryMode;
	this->commandBuffer[1] = ssd1306::MemoryMode::PAGE;
	RF_END_RETURN_CALL(this->writeCommands(2));
}

template<class I2cMaster, uint16_t H>
ResumableResult<bool>
Sh1106<I2cMaster, H>::updateClipping()
{
	RF_BEGIN();

	// SH1106-Screen wiring has 2 pixels offset on left
	this->commandBuffer[0] =
		ssd1306::AdressingCommands::HigherColumnStartAddress | ((this->clipping.topLeft.x() + 2) >> 4);
	this->commandBuffer[1] =
		ssd1306::AdressingCommands::LowerColumnStartAddress | ((this->clipping.topLeft.x() + 2) & 0x0F);

	col_first = this->clipping.topLeft.y() / 8;
	col_last = (this->clipping.bottomRight.y() / 8) - 1;

	RF_END_RETURN(true);
}

template<class I2cMaster, uint16_t H>
template<graphic::GraphicBuffer B>
requires std::is_same<typename B::GddramType, typename Sh1106<I2cMaster, H>::GddramType>::value
ResumableResult<bool>
Sh1106<I2cMaster, H>::write(B& buffer, Point placement) {
	RF_BEGIN();
	// this->clipping = this->getIntersection(accessor.getSection());
	this->clipping = {{0, 0}, {128, H}};

	// if (this->pointIntersects(this->clipping.bottomRight - Point(1, 1)))
	// {
		RF_CALL(updateClipping());

		this->transaction_success = true;
		col_buffer = 0;

		for (col = col_first; col <= col_last; col++)
		{
			this->commandBuffer[2] = ssd1306::AdressingCommands::PageStartAddress | col;
			this->transaction_success &= RF_CALL(this->writeCommands(3));

			// TODO pass buffer.colspan(col_buffer++)
			RF_WAIT_UNTIL(this->transaction.configureDisplayWrite(buffer.rowspan(col_buffer++)));
			RF_WAIT_UNTIL(this->startTransaction());
			RF_WAIT_WHILE(this->isTransactionRunning());
			this->transaction_success &= this->wasTransactionSuccessful();
		};
	// } else {
	// 	MODM_LOG_ERROR << "buffer exceeds display border in " << __FUNCTION__ << endl;
	// 	this->transaction_success = false;
	// }
	RF_END_RETURN(this->transaction_success);
}

#if 0
template<class I2cMaster, uint16_t H>
template<template<typename> class Accessor>
ResumableResult<bool>
Sh1106<I2cMaster, H>::writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor) {
	RF_BEGIN();
	this->clipping = this->getIntersection(accessor.getSection());

	if (this->pointIntersects(this->clipping.bottomRight - shape::Point(1, 1)))
	{
		RF_CALL(updateClipping());

		this->transaction_success = true;
		col_buffer = 0;

		for (col = col_first; col <= col_last; col++)
		{
			this->commandBuffer[2] = ssd1306::AdressingCommands::PageStartAddress | col;
			this->transaction_success &= RF_CALL(this->writeCommands(3));

			RF_WAIT_UNTIL(this->transaction.configureDisplayWrite(accessor.getPointer() + (this->clipping.getWidth() * col_buffer++), this->clipping.getWidth()));
			RF_WAIT_UNTIL(this->startTransaction());
			RF_WAIT_WHILE(this->isTransactionRunning());
			this->transaction_success &= this->wasTransactionSuccessful();
		};
	} else {
		MODM_LOG_ERROR << "buffer exceeds display border in " << __FUNCTION__ << endl;
		this->transaction_success = false;
	}
	RF_END_RETURN(this->transaction_success);
}
#endif

} // namespace modm