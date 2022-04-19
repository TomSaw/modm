/*
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <concepts>
#include <limits>

#include <modm/math/utils/integer_traits.hpp>
namespace modm
{

// TODO Find better home
// Hint: ::value() is very common for drivers and extended types
template <class T>
concept has_value_method = requires(T e) {
	{ e.value() };
};

/**
 * @brief 			Accessor for individual elements of palletized types.
 * 					It handles shifting and masking when reading and writing.
 * 
 * @tparam TS		Type of the pallete.
 * @tparam TE 		Type of individual Element. May provide TE::bitmask
 */
template<typename TP, typename TE>
// TODO requires sanity checks?
class pallete_element_accessor {
	TP& value;
	
	const std::size_t shift;
	const TP bitmask;

public:
	// OPTIMIZE confirm that lazy "bitmask << shift" is not faster
	constexpr explicit pallete_element_accessor(TP& value, std::size_t shift, TP bitmask)
		: value(value), shift(shift), bitmask(bitmask)
	{}
	
	constexpr explicit pallete_element_accessor(TP& value, std::size_t shift)
		: pallete_element_accessor(value, shift, TE::bitmask << shift)
	{}

	constexpr operator TE()
	{ return (value & bitmask) >> shift; }

	constexpr void operator=(TE element)
	{ value = (value & ~bitmask) | TP(element) << shift; }

	constexpr void operator=(TE element) requires has_value_method<TE>
	{ value = (value & ~bitmask) | TP(element.value()) << shift; }
};



/**
 * @brief 					A pallete Container for very small element "types" with typical 1, 2 or 4 digits.
 * 							For bigger or fundamental element types, prefer a union.
 * 
 * @tparam TP 				Type of pallete (constraint to unsigned intergral for now)
 * @tparam DigitsElement	Digits of individual element
 * @tparam TE(optional)		Type of individual element
 * 
 * @author 					Thomas Sommer
 * @ingroup					???
 */
template<std::unsigned_integral TP, int DigitsElement, typename TE = least_uint<DigitsElement>>
requires(
	std::numeric_limits<TP>::digits > DigitsElement
	&& std::numeric_limits<TP>::digits % DigitsElement == 0
)
class pallete
{
public:
	using ValueType = TP;
	using ElementType = TE;

	static constexpr int digits = std::numeric_limits<TP>::digits;
	static constexpr std::size_t size = digits / DigitsElement;
	static constexpr TP bitmask_element = bitmask<DigitsElement>();
	static constexpr TP bitmask_pallete = pallete(bitmask_element).value();
	
private:
	// recursive fill
	template <int I>
	requires (I == 1)
	constexpr TP rfill(TE element) requires (!has_value_method<TE>)
	{ return element; }

	template <int I>
	requires (I == 1)
	constexpr TP rfill(TE element) requires has_value_method<TE>
	{ return element.value(); }

	template<int I> 
	constexpr TP rfill(TE element) requires (!has_value_method<TE>)
	{ return rfill<I - 1>(element) << DigitsElement | TP(element); }

	template<int I>
	constexpr TP rfill(TE element) requires has_value_method<TE>
	{ return rfill<I - 1>(element) << DigitsElement | TP(element.value()); }

	// variadic push
	constexpr TP vpush(auto element)
	{ return TE(element); }

	constexpr TP vpush(auto element) requires has_value_method<TE>
	{ return TE(element).value(); }

	template<typename... Elements>
	constexpr TP vpush(auto element, Elements... elements)
	{ return vpush(elements...) << DigitsElement | TP(TE(element)); }

	template<typename... Elements>
	constexpr TP vpush(auto element, Elements... elements) requires has_value_method<TE>
	{ return vpush(elements...) << DigitsElement | TP(TE(element).value()); }

public:

	constexpr pallete() = default;

	// constexpr pallete(TP value)
	// 	: value_(value)
	// {}

	/// Passing a single arg fills up the pallete
	constexpr explicit pallete(TE element)
	{ fill(element); }

	/// Passing multiple arguments inits individual elements
 	template<typename... Elements>
	constexpr explicit pallete(TE element, Elements... elements)
	{
		static_assert(sizeof...(Elements) < size, "Number of arguments do not fit palette!");
		value_ = vpush(element, elements...);
	}


	constexpr void fill(TE element)
	{ value_ = rfill<size>(element); }

	// operators

	// Shifting ops are required for higher order palletizing and unpalletizing algorithms.
 	pallete operator>>(int shift)
	{
		pallete ret(*this);
		ret.value_ >>= shift * DigitsElement;
		return ret;
	}

	pallete operator<<(int shift)
	{
		pallete ret(*this);
		ret.value_ <<= shift * DigitsElement;
		return ret;
	}

	pallete& operator>>=(int shift)
	{
		value_ >>= shift * DigitsElement;
		return *this;
	}

	pallete& operator<<=(int shift)
	{
		value_ <<= shift * DigitsElement;
		return *this;
	}

	pallete& operator&=(const pallete& other) {
		value_ &= other.value_;
	}

	pallete& operator|=(const pallete& other) {
		value_ |= other.value_;
	}

	// accessors

	constexpr TP value() const { return value_; }
	constexpr TP& value() { return value_; }

	constexpr TE operator[](std::size_t i) const
	{ return value_ >> i * DigitsElement & bitmask_element; }

	constexpr auto operator[](std::size_t i)
	{ return pallete_element_accessor<TP, TE>(value_, i * DigitsElement); }

	// iterators
	struct iterator
	{
		// TODO check iterator_category!
		using iterator_category = std::contiguous_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = TP;
    	using pointer           = TP*;
	    using reference         = TP&;

		reference value_;
		uint8_t shift;

		void operator++() { shift += DigitsElement; }
		void operator--() { shift -= DigitsElement; }

		// post increment is inefficient and shouldn't be used
		// void operator++(int);
		// void operator--(int);

		void operator+=(int n) { shift += n * DigitsElement; }
		void operator-=(int n) { shift -= n * DigitsElement; }

		// OPTIMIZE Benchmark, if there's any performance difference of this "more direct access"
		TE operator*() const
		{ return value_ >> shift & bitmask_element; }

		auto operator*()
		{ return pallete_element_accessor<TP, TE>(value_, shift, bitmask_element << shift); } 

		// TODO implement 3-way comparison
		// bool operator<=>(const iterator &other) const
		bool operator==(const iterator &other) const
		{ return shift == other.shift; }
	};

	constexpr auto begin() { return iterator{value_, 0}; }
	constexpr auto end() { return iterator{value_, digits}; }

	// TODO pallete reverse iteration if any needed
	// constexpr auto rbegin()
	// constexpr auto rend()

protected:
	TP value_{0};
};
}  // namespace modm

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<std::unsigned_integral TP, int DigitsElement, typename TE = modm::least_uint<DigitsElement>>
modm::IOStream &
// TODO add support for const iterator in pallete
// operator<<(modm::IOStream &os, const modm::pallete<TP, DigitsElement, TE> &pallete)
operator<<(modm::IOStream &os, modm::pallete<TP, DigitsElement, TE> &pallete)
{
	for(TE element : pallete)
		os << element << "|";

	return os;
}
#endif