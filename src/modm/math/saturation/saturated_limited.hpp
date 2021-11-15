#pragma once

#include "saturated.hpp"

namespace modm
{
/**
 * @brief 					Saturated type with custom boundaries.
 *
 * @tparam T 				Type of saturated. plain or reference
 * @tparam ConstLimits		true: max and min are const, false: max and min may be adjusted runtime
 * 
 * @author	Thomas Sommer
 *
 * @ingroup modm_math_saturated
 */
template<typename T, bool ConstLimits = false>
requires std::integral<std::remove_reference_t<T>>
class SaturatedLimited : public Saturated<T>
{
protected:
	using TP = Saturated<T>::TP;
	using TB = conditional_t<ConstLimits, const TP, TP>;

public:
	TB min = numeric_limits<TB>::min();
	TB max = numeric_limits<TB>::max();

	constexpr SaturatedLimited() = default;

	constexpr SaturatedLimited(T value, TB min, TB max) : Saturated<T>(value), min(min), max(max) {}

	// ### operator++, operator-- [Post]
	SaturatedLimited&
	operator++()
	{
		if (Saturated<T>::value < max) Saturated<T>::value++;
		return *this;
	}

	SaturatedLimited&
	operator--()
	{
		if (Saturated<T>::value > min) Saturated<T>::value--;
		return *this;
	}

	// operator++(int), operator--(int) [Pre]
	SaturatedLimited
	operator++(int)
	{
		SaturatedLimited tmp(*this);
		if (Saturated<T>::value < max) Saturated<T>::value++;
		return tmp;
	}

	SaturatedLimited
	operator--(int)
	{
		SaturatedLimited tmp(*this);
		if (Saturated<T>::value > min) Saturated<T>::value--;
		return tmp;
	}

	// ### operator=

	// Unrequired. generic version below is equivalent
	void
	operator=(const T& v) {
		Saturated<T>::value = std::clamp<T>(v, min, max);
	}

 	template<typename U>
	requires integral<remove_reference_t<U>>
	void
	operator=(const U& v)
	{
		Saturated<T>::value = std::clamp< modm::fits_any_t<TP, U> >(v, min, max);
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	void
	operator=(const Saturated<U>& other)
	{
		Saturated<T>::value = std::clamp< modm::fits_any_t<TP, U> >(other.getValue(), min, max);
	}

	template<typename U, bool ConstLimits_>
	requires integral<remove_reference_t<U>>
	void
	operator=(const SaturatedLimited<T, ConstLimits_>& other)
	{
		if constexpr(ConstLimits) {
			Saturated<T>::value = std::clamp< modm::fits_any_t<TP, U> >(other.value, min, max);
		} else {
			Saturated<T>::value = other.value;
			min = other.min;
			max = other.max;
		}
	}

	// ### operator+=, operator-=, operator*=
	template<typename U>
	requires integral<remove_reference_t<U>>
	SaturatedLimited&
	operator+=(const U& v)
	{
		Saturated<T>::template operator+=<U>(v);
		Saturated<T>::value = std::clamp<T>(Saturated<T>::value, min, max);
		return *this;
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	SaturatedLimited&
	operator-=(const U& v)
	{
		Saturated<T>::template operator-=<U>(v);
		Saturated<T>::value = std::clamp<T>(Saturated<T>::value, min, max);
		return *this;
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	SaturatedLimited&
	operator*=(const U& v)
	{
		Saturated<T>::template operator*=<U>(v);
		Saturated<T>::value = std::clamp<T>(Saturated<T>::value, min, max);
		return *this;
	}

	// ### operator+, operator-, operator*
	template<typename U>
	requires integral<remove_reference_t<U>>
	TP
	operator+(const U& v)
	{
		SaturatedLimited<TP, ConstLimits> tmp(*this);
		tmp += v;
		return tmp.value;
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	TP
	operator-(const U& v)
	{
		SaturatedLimited<TP, ConstLimits> tmp(*this);
		tmp -= v;
		return tmp.value;
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	TP
	operator*(const U& v)
	{
		SaturatedLimited<TP, ConstLimits> tmp(*this);
		tmp *= v;
		return tmp.value;
	}

	TB&
	getMin() const
	{ return min; }

	TB&
	getMax() const
	{ return max; }

private:
	template<typename, bool>
	friend class SaturatedLimited;

	template<typename T_, bool ConstLimits_>
	friend modm::IOStream&
	operator<<(modm::IOStream&, const SaturatedLimited<T_, ConstLimits_>&);
};

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<typename T_, bool ConstLimits_>
modm::IOStream&
operator<<(modm::IOStream& os, const SaturatedLimited<T_, ConstLimits_>& sl)
{
	// An illustrative asci-slider
	const T_ step = std::max((sl.max - sl.min) / 32, 1);
	T_ i = sl.min;
	while(i < sl.value) { os << '-'; i += step; }
	os << '|';
	while(i < sl.max) { os << '-'; i += step; }
	os << modm::endl;

	os << "min: " << sl.min << "\tval: " << sl.value << "\tmax: " << sl.max;
	return os;
}
#endif

}  // namespace modm