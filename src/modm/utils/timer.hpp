#pragma once

#include <chrono>
#include <concepts>
#include <modm/math/units.hpp>
#include <modm/board.hpp>

/**
 * @brief  Common used Timer configurations
 * 
 */
namespace modm::timer {
	template<class Timer, class Rep, class Period>
	inline void
	initPeriodicInterrupt(std::chrono::duration<Rep, Period> period)
	{
		Timer::enable();
		Timer::setMode(Timer::Mode::UpCounter);

		Timer::template setPeriod<Board::SystemClock>(period);
		
		if constexpr (std::derived_from<Timer, AdvancedControlTimer>)
			Timer::enableInterruptVector(Timer::Interrupt::Update, true, 1);
		else
			Timer::enableInterruptVector(true, 1);
		
		Timer::enableInterrupt(Timer::Interrupt::Update);

		Timer::applyAndReset();
		Timer::start();
	}

		template<class Timer, class... Signals>
	void
	initInputCapture() {
		static_assert(sizeof...(Signals) > 0, "At least one Channel is required for InputCapture");

		Timer::enable();
		Timer::setMode(Timer::Mode::UpCounter);

		// 1Mhz InputCapture clock, 2
		Timer::setPrescaler(Board::SystemClock::Apb1Timer / (1_MHz * 2));

		Timer::template connect<Signals...>();

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch1) or ...)) {
			Timer::configureInputChannel(1,
				Timer::InputCaptureMapping::InputOwn,
				Timer::InputCapturePrescaler::Div2,
				Timer::InputCapturePolarity::Rising,
				4
			);
		}

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch2) or ...)) {
			Timer::configureInputChannel(2,
				Timer::InputCaptureMapping::InputOwn,
				Timer::InputCapturePrescaler::Div2,
				Timer::InputCapturePolarity::Rising,
				4
			);
		}
		
		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch3) or ...)) {
			Timer::configureInputChannel(3,
				Timer::InputCaptureMapping::InputOwn,
				Timer::InputCapturePrescaler::Div2,
				Timer::InputCapturePolarity::Rising,
				4
			);
		}

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch4) or ...)) {
			Timer::configureInputChannel(4,
				Timer::InputCaptureMapping::InputOwn,
				Timer::InputCapturePrescaler::Div2,
				Timer::InputCapturePolarity::Rising,
				4
			);
		}

		Timer::enableInterruptVector(true, 10);

		if constexpr (((Signals::Signal == platform::Gpio::Signal::Ch1) or ...))
			Timer::enableInterrupt(Timer::Interrupt::CaptureCompare1);

		if constexpr (((Signals::Signal == platform::Gpio::Signal::Ch2) or ...))
			Timer::enableInterrupt(Timer::Interrupt::CaptureCompare2);

		if constexpr (((Signals::Signal == platform::Gpio::Signal::Ch3) or ...))
			Timer::enableInterrupt(Timer::Interrupt::CaptureCompare3);

		if constexpr (((Signals::Signal == platform::Gpio::Signal::Ch4) or ...))
			Timer::enableInterrupt(Timer::Interrupt::CaptureCompare4);

		Timer::applyAndReset();
		Timer::start();
	}

	template<class Timer, class... Signals>
	static inline void
	initPwm(typename Timer::Value compare_max, modm::frequency_t pwm_f) {
		static_assert(sizeof...(Signals) > 0, "At least one Channel is required for Pwm");
		
		Timer::enable();
		Timer::setMode(Timer::Mode::UpCounter);

		((GpioStatic<typename Signals::Data>::setOutput()), ...);
		Timer::template connect<Signals...>();

		uint16_t prescaler = Board::SystemClock::Apb1Timer / (compare_max * pwm_f);
		// static_assert(prescaler > 0, "Prescaler is too small");

		Timer::setPrescaler(prescaler);
		Timer::setOverflow(compare_max - 1);

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch1) or ...))
			Timer::configureOutputChannel(1, Timer::OutputCompareMode::Pwm, 0);

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch2) or ...))
			Timer::configureOutputChannel(2, Timer::OutputCompareMode::Pwm, 0);

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch3) or ...))
			Timer::configureOutputChannel(3, Timer::OutputCompareMode::Pwm, 0);

		if constexpr(((Signals::Signal == platform::Gpio::Signal::Ch4) or ...))
			Timer::configureOutputChannel(4, Timer::OutputCompareMode::Pwm, 0);

		Timer::applyAndReset();

		Timer::start();
		Timer::enableOutput();
	}
}