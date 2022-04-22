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

#include "array2d.hpp"

namespace modm
{

template<typename T, std::size_t Rows, std::size_t Cols, Dimension D, class DmaChannel>
class array2d_Dma : public array2d<T, Rows, Cols, D>
{
private:
	T value;
	static inline bool dmaCopyComplete{true};

public:
	void
	initializeDma()
	{
		using namespace modm::platform;

		DmaChannel::configure(
			DmaBase::DataTransferDirection::MemoryToMemory,
			DmaBase::MemoryDataSize(0),      // set below using configureDataSize()
			DmaBase::PeripheralDataSize(0),  // set below using configureDataSize()
			DmaBase::MemoryIncrementMode::Increment,
			DmaBase::PeripheralIncrementMode::Increment
		);

		// TODO Support for fundamental types
		DmaChannel::template configureDataSize<typename T::ValueType>();

		DmaChannel::setTransferCompleteIrqHandler(handleDmaComplete);
		DmaChannel::enableInterruptVector();
		DmaChannel::enableInterrupt(DmaBase::InterruptEnable::TransferComplete);
	}

	array2d_Dma&
	operator=(array2d<T, Rows, Cols, D>& other)
	{
		if (this == &other or !dmaCopyComplete) return *this;

		dmaCopyComplete = false;

		DmaChannel::setPeripheralIncrementMode(modm::platform::DmaBase::PeripheralIncrementMode::Increment);
		DmaChannel::setMemoryAddress(uintptr_t(this->data()));
		DmaChannel::setPeripheralAddress(uintptr_t(other.data()));
		DmaChannel::setDataLength(this->size);  // Caution! Max 65536
		DmaChannel::start();

		return *this;
	}

	void
	fill(T value)
	{
		if (!dmaCopyComplete) return;

		this->value = value;
		dmaCopyComplete = false;

		DmaChannel::setPeripheralIncrementMode(modm::platform::DmaBase::PeripheralIncrementMode::Fixed);
		DmaChannel::setMemoryAddress(uintptr_t(this->data()));
		DmaChannel::setPeripheralAddress(uintptr_t(&this->value));
		DmaChannel::setDataLength(this->size);  // Caution! Max 65536
		DmaChannel::start();
	}

	static void
	handleDmaComplete()
	{
		dmaCopyComplete = true;
		DmaChannel::stop();
	}
};
}  // namespace modm