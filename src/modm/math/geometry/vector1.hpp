/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
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

#include <cmath>
#include <cstdlib>
#include <concepts>
#include <numeric>

#include "vector.hpp"

namespace modm
{
/**
 * \brief	Class for handling common vector operations (1D)
 *
 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
 *
 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
 *
 * \author	Niklas Hauser
 * \author	Thomas Sommer
 * \ingroup	modm_math_geometry
 */
template<typename T>
class Vector<T, 1>
{
public:
	using WideType = GeometricTraits<T>::WideType;
	using FloatType = GeometricTraits<T>::FloatType;

	T x{0};

	// fundamental constructors
	constexpr Vector() = default;

	constexpr Vector(T x) : x(x) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr Vector(U x) : x(std::round(x)) {}

	template<typename U>
	constexpr Vector(Vector<U, 1> v) : Vector(v.x) {}

	// matrix constructors
	template<typename U>
	constexpr Vector(const Matrix<U, 1, 1> &rhs)
		: Vector(*reinterpret_cast<const T*>(&rhs)) {}

	// as matrix convertors
	Matrix<T, 1, 1>&
	asMatrix() { return *(modm::Matrix<T, 1, 1>*) this; }

	const Matrix<T, 1, 1>&
	asMatrix() const { return *(modm::Matrix<T, 1, 1>*) this; }

	// matrix assignment
	Vector& operator= (const Matrix<T, 1, 1> &rhs) {
		x = *reinterpret_cast<const T*>(&rhs);
		return *this;
	}

	// getters
	T getX() const { return x; }

	// setters
	void setX(T x) { this->x = x; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setX(U x) { this->x = std::round(x); }

	void set(T x) { this->x = x; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void set(U x) { this->x = std::round(x); }

	// accessors
	T& operator[] (std::size_t index)
	{ return reinterpret_cast<T*>(this)[index]; }

	const T& operator[] (std::size_t index) const
	{ return reinterpret_cast<const T*>(this)[index]; }

	T* ptr() { return reinterpret_cast<T*>(this); }

	const T* ptr() const { return reinterpret_cast<const T*>(this);}

	// operators
	auto operator<=>(const Vector &) const = default;

	constexpr Vector operator+ (const Vector &rhs) const
	{ return Vector(x + rhs.x); }

	constexpr Vector operator- (const Vector &rhs) const
	{ return Vector(x - rhs.x); }

	constexpr T operator* (const Vector &rhs) const
	{ return x * rhs.x; }

	template <typename U>
	constexpr Vector operator* (U scale) const
	{ return Vector<U, 1>(x * scale); }

	template <typename U>
	constexpr Vector operator/ (U scale) const
	{ return Vector<U, 1>(x / scale); }

	Vector& operator+= (const Vector &rhs)
	{ x += rhs.x; return *this; }

	Vector& operator-= (const Vector &rhs)
	{ x -= rhs.x; return *this; }

	template <typename U>
	Vector& operator*= (U scale)
	{ x *= scale; return *this; }

	template <typename U>
	Vector& operator/= (U scale)
	{ x /= scale; return *this; }

	// template <typename>
	// requires std::is_signed<T>::value
	constexpr Vector operator- () const
	{ return Vector(-x); }

	// additional methods
	T getLength() const
	{ return std::abs(x); }

	WideType getLengthSquared() const
	{ return std::pow(x, 2); }

	bool hasNan() const { return std::isnan(x); }

	bool hasInf() const { return std::isinf(x); }

#ifndef __DOXYGEN__
	IMPLEMENT_VECTOR_ACCESSOR2(x,x)
	IMPLEMENT_VECTOR_ACCESSOR3(x,x,x)
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x)
#endif

	// depricated methods
	template<typename U>
	[[deprecated("Use common constructor instead!")]]
	Vector<U, 1>
	convert() const { return {*this}; }

protected:
	template<typename U>
	friend IOStream&
	operator<<(IOStream& os, const Vector<U, 1>& c);
};

template<typename U, typename T>
static inline Vector<T, 1> operator * (const U &lhs, const Vector<T, 1> &rhs)
{ return rhs * lhs; }

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<typename U>
IOStream&
operator<< (IOStream& os, const Vector<U, 1>& v) {
	os << "x=" << v.x;
	return os;
}
#endif

using Vector1f = Vector<float, 1>;
using Vector1i = Vector<int16_t, 1>;
using Vector1u = Vector<uint16_t, 1>;

} // namespace modm
