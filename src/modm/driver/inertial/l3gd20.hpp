/*
 * Copyright (c) 2015-2016, Niklas Hauser
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
#define MODM_L3GD20_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/math/utils/endianness.hpp>
#include "lis3_transport.hpp"

namespace modm
{

// forward declaration for friending with l3gd20::Data
template < class Transport >
class L3gd20;

/// @ingroup modm_driver_l3gd20
struct l3gd20
{
	enum class
	Register : uint8_t
	{
		WHO_AM_I = 0x0F,		///< Device identification (0x3F): r

		CTRL_REG1 = 0x20,		///< Control register 1: r/w
		CTRL_REG2 = 0x21,		///< Control register 2: r/w
		CTRL_REG3 = 0x22,		///< Control register 3: r/w
		CTRL_REG4 = 0x23,		///< Control register 4: r/w
		CTRL_REG5 = 0x24,		///< Control register 5: r/w

		REFERENCE = 0x25,
		OUT_TEMP = 0x26,			///< Temperature Output
		STATUS_REG = 0x27,		///< Status Data Register

		OUT_X_L = 0x28,			///< Output x-axis register low
		OUT_X_H = 0x29,			///< Output x-axis register high
		OUT_Y_L = 0x2A,			///< Output y-axis register low
		OUT_Y_H = 0x2B,			///< Output y-axis register high
		OUT_Z_L = 0x2C,			///< Output z-axis register low
		OUT_Z_H = 0x2D,			///< Output z-axis register high

		FIFO_CTRL = 0x2E,		///< FIFO control register
		FIFO_SRC = 0x2F,		///< FIFO source register

		INT1_CFG = 0x30,		///< Interrupt config register
		INT1_SRC = 0x31,		///< Interrupt source register

		INT1_TSH_XH = 0x32,		///< Interrupt threshold X register high
		INT1_TSH_XL = 0x33,		///< Interrupt threshold X register low
		INT1_TSH_YH = 0x34,		///< Interrupt threshold Y register high
		INT1_TSH_YL = 0x35,		///< Interrupt threshold Y register low
		INT1_TSH_ZH = 0x36,		///< Interrupt threshold Z register high
		INT1_TSH_ZL = 0x37,		///< Interrupt threshold Z register low

		INT1_DURATION = 0x38,	///< Interrupt duration register
	};

public:
	/// CTRL_REG1 default value is 0x07
	enum class
	Control1 : uint8_t
	{
		DR1 = Bit7,
		DR0 = Bit6,

		BW1 = Bit5,
		BW0 = Bit4,

		PD = Bit3,		///< Power-down mode enable
		ZEN = Bit2,		///< Z axis enable
		YEN = Bit1,		///< Y axis enable
		XEN = Bit0,		///< X axis enable
	};
	MODM_FLAGS8(Control1);

	/// CTRL_REG2 default value is 0x00
	enum class
	Control2 : uint8_t
	{
		HPM1 = Bit5,
		HPM0 = Bit4,

		HPCF3 = Bit3,
		HPCF2 = Bit2,
		HPCF1 = Bit1,
		HPCF0 = Bit0,
	};
	MODM_FLAGS8(Control2);

	/// CTRL_REG3 default value is 0x00
	enum class
	Control3 : uint8_t
	{
		I1_Int1 = Bit7,		///< Interrupt enable on INT1 pin.
		I1_Boot = Bit6,		///< Boot status available on INT1.
		H_Lactive = Bit5,	///< Interrupt active configuration on INT1.
		PP_OD = Bit4,		///< Push-pull / Open drain.
		I2_DRDY = Bit3,		///< Data-ready on DRDY/INT2.
		I2_WTM = Bit2,		///< FIFO watermark interrupt on DRDY/INT2.
		I2_ORun = Bit1,		///< FIFO overrun interrupt on DRDY/INT2.
		I2_Empty = Bit0,	///< FIFO empty interrupt on DRDY/INT2.
	};
	MODM_FLAGS8(Control3);

	/// CTRL_REG4 default value is 0x00
	enum class
	Control4 : uint8_t
	{
		BDU = Bit7,		///< Block data update. (0: continuos update; 1: output registers not updated until MSb and LSb reading)
		BLE = Bit6,		///< Big/little endian data selection. (0: Data LSb @ lower address; 1: Data MSb @ lower address)
		FS1 = Bit5,
		FS0 = Bit4,

		SIM = Bit0,		///<SPI serial interface mode selection. (0: 4-wire interface; 1: 3-wire interface).
	};
	MODM_FLAGS8(Control4);

	/// CTRL_REG5 default value is 0x00
	enum class
	Control5 : uint8_t
	{
		BOOT = Bit7,		///< Force reboot, cleared as soon as the reboot is finished. Active high.
		FIFO_EN = Bit6,		///< FIFO enable.

		HPen = Bit4,		///< High-pass filter enable.
		INT1_Sel1 = Bit3,	///< Enable FIFO Empty indication on int1.
		INT1_Sel0 = Bit2,	///< FIFO Watermark interrupt on int1.
		Out_Sel1 = Bit1,	///< FIFO overrun interrupt on int1.
		Out_Sel0 = Bit0,	///< PI serial interface mode selection.
	};
	MODM_FLAGS8(Control5);

	typedef struct {
		Control1_t control1;
		Control2_t control2;
		Control3_t control3;
		Control4_t control4;
		Control5_t control5;
	} ControlStruct_t;



	

	/// FIFO_CTRL default value is 0x00
	enum class
	FifoControl : uint8_t
	{
		FM2 = Bit7,
		FM1 = Bit6,
		FM0 = Bit5,

		WTM4 = Bit4,
		WTM3 = Bit3,
		WTM2 = Bit2,
		WTM1 = Bit1,
		WTM0 = Bit0,
	};
	MODM_FLAGS8(FifoControl);

	/// FIFO_SRC is read-only
	enum class
	FifoSource : uint8_t
	{
		WTM = Bit7,
		OVRN = Bit6,
		EMPTY = Bit5,

		FSS4 = Bit4,
		FSS3 = Bit3,
		FSS2 = Bit2,
		FSS1 = Bit1,
		FSS0 = Bit0,
	};
	MODM_FLAGS8(FifoSource);

	/// INT1_CFG default value is 0x00
	enum class
	IntConfig : uint8_t
	{
		AND = Bit7,		///< And/or combination of Interrupt events. (0: OR combination of interrupt events; 1: AND combination of interrupt events)
		LIR = Bit6,		///< Latch Interrupt request into FF_WU_SRC reg with the FF_WU_SRC reg cleared by reading FF_WU_SRC_1 reg.
		ZHIE = Bit5,	///< Enable interrupt generation on Z high event.
		ZLIE = Bit4,	///< Enable interrupt generation on Z low event.
		YHIE = Bit3,	///< Enable interrupt generation on Y high event.
		YLIE = Bit2,	///< Enable interrupt generation on Y low event.
		XHIE = Bit1,	///< Enable interrupt generation on X high event.
		XLIE = Bit0,	///< Enable interrupt generation on X low event.
	};
	MODM_FLAGS8(IntConfig);

	typedef FlagsGroup<Control1_t, Control2_t, Control3_t, Control4_t, Control5_t,FifoControl_t, IntConfig_t> Control_t;

	/// INT1_SRC default value is 0x00
	enum class
	IntSource : uint8_t
	{
		IA = Bit6,	///< Interrupt Active.
		ZH = Bit5,	///< Z high.
		ZL = Bit4,	///< Z low.
		YH = Bit3,	///< Y high.
		YL = Bit2,	///< Y low.
		XH = Bit1,	///< X high.
		XL = Bit0,	///< X low.
	};
	MODM_FLAGS8(IntSource);


	enum class
	FifoMode : uint8_t
	{
		Bypass = 0,
		Fifo = int(FifoControl::FM0),
		Stream = int(FifoControl::FM1),
		StreamTriggerFifo = int(FifoControl::FM1) | int(FifoControl::FM0),
		BypassTriggerStream = int(FifoControl::FM2),
		BypassTriggerFifo = int(FifoControl::FM2) | int(FifoControl::FM1) | int(FifoControl::FM0),
	};

	/// STATUS is read-only
	enum class
	Status : uint8_t
	{
		ZYXOR = Bit7,	///< X, Y, and Z axis data overrun.
		ZOR = Bit6,		///< Z axis data overrun.
		YOR = Bit5,		///< Y axis data overrun.
		XOR = Bit4,		///< X axis data overrun.
		ZYXDA = Bit3,	///< X, Y, and Z axis new data available.
		ZDA = Bit2,		///< Z axis new data available.
		YDA = Bit1,		///< Y axis new data available.
		XDA = Bit0,		///< X axis new data available.
	};
	MODM_FLAGS8(Status);

public:
	struct modm_packed
	Data
	{
		template < class Transport >
		friend class L3gd20;

		Data() = default;

		/// returns the rotation in g
		///@{
		float inline
		getX() { return modm::fromLittleEndian(data.rotation[0]) * 0.00875f * scale; }

		float inline
		getY() { return modm::fromLittleEndian(data.rotation[1]) * 0.00875f * scale; }

		float inline
		getZ() { return modm::fromLittleEndian(data.rotation[2]) * 0.00875f * scale; }
		///@}

		int8_t inline
		getTemperature() { return data.temperature; }

		Status inline
		getStatus() { return data.status; }

		inline int16_t
		operator [](uint8_t index)
		{ return (index < 3) ? modm::fromLittleEndian(data.rotation[index]) : 0; }

		inline uint8_t*
		getPointer()
		{ return raw; }
	private:
		// rotation-scale in multiples of 200dps
		uint8_t scale;
		union {
			// xl,xh,yl,yh,zl,zh
			uint8_t raw[8];
			struct {
				int8_t temperature;
				Status status;
				int16_t rotation[3];
			} data;
		};
	};
}; // struct l3gd20

/**
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Lis3TransportI2c
 * @see Lis3TransportSpi
 *
 * @ingroup modm_driver_l3gd20
 * @author	Niklas Hauser
 */
template < class Transport >
class L3gd20 : public l3gd20, public Transport
{
public:
	/// Constructor, requires a l3gd20::Data object.
	/// For I2c this also sets the address to 0b110101 (alternative: 0x1C).
	L3gd20(Data &data, uint8_t address=0x6B);

	static constexpr Control1_t Enable_Hz95 = Control1::PD | Control1::BW1 | Control1::BW0;					///< 95Hz with 25Hz Cut-Off
	static constexpr Control1_t Enable_Hz190 = Control1::PD | Control1::DR0 | Control1::BW1 | Control1::BW0;	///< 190Hz with 70Hz Cut-Off
	static constexpr Control1_t Enable_Hz380 = Control1::PD | Control1::DR1 | Control1::BW1 | Control1::BW0;	///< 380Hz with 100Hz Cut-Off
	static constexpr Control1_t Enable_Hz760 = Control1::PD | Control1::DR1 | Control1::DR0 | Control1::BW1 | Control1::BW0;	///< 760Hz with 100Hz Cut-Off

	static constexpr Control1_t Enable_clearMask = Enable_Hz760;

	// MARK: Control Registers
	modm::ResumableResult<bool> inline
	updateControl(Control1_t setMask, Control1_t clearMask = Control1_t(0xFF))
	{
		return updateControlRegister(0, setMask, clearMask);
	}

	modm::ResumableResult<bool> inline
	updateControl(Control2_t setMask, Control2_t clearMask = Control2_t(0xFF))
	{
		return updateControlRegister(1, setMask, clearMask);
	}

	modm::ResumableResult<bool> inline
	updateControl(Control3_t setMask, Control3_t clearMask = Control3_t(0xFF))
	{
		return updateControlRegister(2, setMask, clearMask);
	}

	modm::ResumableResult<bool> inline
	updateControl(Control4_t setMask, Control4_t clearMask = Control4_t(0xFF))
	{
		RF_BEGIN();

		if (RF_CALL(updateControlRegister(3, setMask, clearMask))) {
			// update the scale in the data object
			uint8_t scale[] = {1, 2, 8};
			data.scale = scale[(setMask & (Control4::FS1 | Control4::FS0)).value >> 4];
			RF_RETURN(true);
		}

		RF_END_RETURN(false);
	}

	static constexpr Control4_t	Scale_Dps245 = Control4_t(0);
	static constexpr Control4_t	Scale_Dps500 = Control4::FS0;
	static constexpr Control4_t	Scale_Dps2000 = Control4::FS1;

	static constexpr Control4_t	Scale_clearMask = Scale_Dps2000;

	modm::ResumableResult<bool> inline
	updateControl(Control5_t setMask, Control5_t clearMask = Control5_t(0xFF))
	{
		return updateControlRegister(4, setMask, clearMask);
	}

	modm::ResumableResult<bool> inline
	updateFifoControl(FifoControl_t setMask, FifoControl_t clearMask = FifoControl_t(0xFF))
	{
		return updateControlRegister(14, setMask, clearMask);
	}

	modm::ResumableResult<bool> inline
	updateInterruptConfiguration(IntConfig_t setMask, IntConfig_t clearMask = IntConfig_t(0xFF))
	{
		return updateControlRegister(16, setMask, clearMask);
	}

	// MARK: Read access
	modm::ResumableResult<bool>
	readRotation();

	inline Data&
	getData()
	{ return data; }

	inline ControlStruct_t& getControl()
	{ return control; }

	// FifoControl_t getFifoControl()
	// { return FifoControl_t(rawBuffer[14]); }

	// FifoSource_t getFifoSource()
	// { return FifoSource_t(rawBuffer[15]); }

	// IntConfig_t getInterruptConfig()
	// { return IntConfig_t(rawBuffer[16]); }

	// IntSource_t getInterruptSource()
	// { return IntSource_t(rawBuffer[17]); }

private:
	modm::ResumableResult<bool>
	updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask = static_cast<Control_t>(0xFF));

	modm::ResumableResult<bool>
	updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask = 0xFF);

	Data &data;

	// Local copy of control registers
	union {
		uint8_t raw[6];
		ControlStruct_t control;
	};

	uint8_t buffer;
};

} // namespace modm

#include "l3gd20_impl.hpp"

#endif	// MODM_L3GD20_HPP
