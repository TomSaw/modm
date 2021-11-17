/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
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
#include "vector.hpp"

namespace modm
{
/**
 * \brief	Class for handling common vector operations (3D)
 *
 *	+ : addition of points
	*	- : different of points
	*	* : dot product or scalar multiplication
	*	/ : scalar division
	*	^ : cross product (determinant)
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
class Vector<T, 3>
{
	using VecT1 = Vector<T, 1>;
	using VecT2 = Vector<T, 2>;
	using VecT3 = Vector;

public:
	using WideType = GeometricTraits<T>::WideType;
	using WideWideType = GeometricTraits<WideType>::WideType;
	using FloatType = GeometricTraits<T>::FloatType;

	T x{0}, y{0}, z{0};

	// fundamental constructors
	constexpr Vector() =  default;

	constexpr explicit Vector(T xyz)
		: x(xyz), y(xyz), z(xyz) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr explicit Vector(T xyz)
		: x(std::round(xyz)), y(std::round(xyz)), z(std::round(xyz)) {}

	constexpr Vector(T x, T y, T z)
		: x(x), y(y), z(z) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr Vector(U x, U y, U z)
		: x(std::round(x)), y(std::round(y)), z(std::round(z)) {}

	template<typename U>
	constexpr Vector(const Vector<U, 3> &v) : Vector(v.x, v.y, v.z) {}

	// vector constructors
	constexpr Vector(VecT1 vx, T y, T z) : Vector(vx.x, y, z) {}
	constexpr Vector(T x, VecT1 vy, T z) : Vector(x, vy.x, z) {}
	constexpr Vector(T x, T y, VecT1 vz) : Vector(x, y, vz.x) {}

	constexpr Vector(VecT1 vx, T y, VecT1 vz) : Vector(vx.x, y, vz.x) {}
	constexpr Vector(VecT1 vx, VecT1 vy, T z) : Vector(vx.x, vy.x, z) {}
	constexpr Vector(T x, VecT1 vy, VecT1 vz) : Vector(x, vy.x, vz.x) {}

	constexpr Vector(VecT1 vx, VecT1 vy, VecT1 vz) : Vector(vx.x, vy.x, vz.x) {}

	constexpr Vector(Vector<T,2> vxy, T z) : Vector(vxy.x, vxy.y, z) {}
	constexpr Vector(T x, VecT2 vyz) : Vector(x, vyz.x, vyz.y) {}

	constexpr Vector(VecT2 vxy, VecT1 vz) : Vector(vxy.x, vxy.y, vz.x) {}
	constexpr Vector(VecT1 vx, VecT2 vyz) : Vector(vx.x, vyz.x, vyz.y) {}

	// matrix constructors
	constexpr Vector(const Matrix<T, 3, 1> &rhs)
		: x(reinterpret_cast<const T*>(&rhs)[0]),
		y(reinterpret_cast<const T*>(&rhs)[1]),
		z(reinterpret_cast<const T*>(&rhs)[2])
	{}

	// array constructors
	template<typename U>
	constexpr explicit Vector(U *array) : Vector(array[0], array[1], array[2]) {}

	// as matrix convertors
	Matrix<T, 3, 1>&
	asMatrix() { return *(Matrix<T, 3, 1>*)this; }

	const Matrix<T, 3, 1>&
	asMatrix() const { return *(Matrix<T, 3, 1>*)this; }

	Matrix<T, 1, 3>&
	asTransposedMatrix() { return *(Matrix<T, 1, 3>*)this; }

	const Matrix<T, 1, 3>&
	asTransposedMatrix() const { return *(Matrix<T, 1, 3>*)this; }

	// matrix assignment
	Vector& operator= (const Matrix<T, 3, 1> &rhs) {
		x = reinterpret_cast<const T*>(&rhs)[0];
		y = reinterpret_cast<const T*>(&rhs)[1];
		z = reinterpret_cast<const T*>(&rhs)[2];

		return *this;
	}

	// getters
	T getX() const { return x; }
	T getY() const { return y; }
	T getZ() const { return z; }

	// setters
	void setX(T x) { this->x = x; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setX(U x) { this->x = std::round(x); }


	void setY(T y) { this->y = y; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setY(U y) { this->x = std::round(y); }


	void setZ(T z) { this->z = z; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setZ(U z) { this->z = std::round(z); }


	void set(T x, T y, T z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void set(U x, U y, U z) {
		this->x = std::round(x);
		this->y = std::round(y);
		this->z = std::round(z);
	}

	// accessors
	T& operator [] (std::size_t index)
	{ return reinterpret_cast<T*>(this)[index]; }

	const T& operator [] (std::size_t index) const
	{ return reinterpret_cast<const T*>(this)[index]; }

	T* ptr() { return reinterpret_cast<T*>(this); }

	const T* ptr() const { return reinterpret_cast<const T*>(this); }

	// operators
	auto operator<=>(const Vector&) const = default;

	constexpr Vector operator+ (const Vector &rhs) const
	{ return Vector(x + rhs.x, y + rhs.y, z + rhs.z); }

	constexpr Vector operator- (const Vector &rhs) const
	{ return Vector(x - rhs.x, y - rhs.y, z - rhs.z); }

	constexpr T operator* (const Vector &rhs) const
	{ return x * rhs.x + y * rhs.y + z * rhs.z; }

	constexpr Vector operator^ (const Vector &rhs) const
	{ return Vector(y * rhs.z-z * rhs.y, z * rhs.x-x * rhs.z, x * rhs.y-y * rhs.x); }

	template <typename U>
	constexpr Vector operator* (U scale) const
	{ return Vector<U, 3>(x * scale, y * scale, z * scale); }

	template <typename U>
	constexpr Vector operator/ (U scale) const
	{ return Vector<U, 3>(x / scale, y / scale, z / scale); }

	Vector& operator += (const Vector &rhs)
	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

	Vector& operator -= (const Vector &rhs)
	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

	template <typename U>
	Vector& operator *= (U scale)
	{ x *= scale; y *= scale; z *= scale; return *this; }

	template <typename U>
	Vector& operator /= (U scale)
	{ x /= scale; y /= scale; z /= scale; return *this; }

	// template <typename>
	// requires std::is_signed<T>::value
	constexpr Vector operator- () const
	{ return Vector(-x, -y, -z); }

	// additional methods
	T getLength() const
	{ return std::sqrt(getLengthSquared()); }

	T getLength() const
	requires std::integral<T>
	{ return round(std::sqrt(getLengthSquared())); }

	WideWideType getLengthSquared() const
	{ return std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2); }

	Vector scaled(float newLength) const {
		float scale = newLength / getLength();
		return *this * scale;
	}

	void scale(float newLength)
	{ *this = scaled(newLength); }

	Vector normalized() const { return scaled(1.0f); }
	void normalize() { scale(1.0); }

	bool hasNan() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
	bool hasInf() const { return std::isinf(x) || std::isinf(y) || std::isinf(z); }

#ifndef __DOXYGEN__
	IMPLEMENT_VECTOR_ACCESSOR2(x,x); IMPLEMENT_VECTOR_ACCESSOR2(x,y); IMPLEMENT_VECTOR_ACCESSOR2(x,z);
	IMPLEMENT_VECTOR_ACCESSOR2(y,x); IMPLEMENT_VECTOR_ACCESSOR2(y,y); IMPLEMENT_VECTOR_ACCESSOR2(y,z);
	IMPLEMENT_VECTOR_ACCESSOR2(z,x); IMPLEMENT_VECTOR_ACCESSOR2(z,y); IMPLEMENT_VECTOR_ACCESSOR2(z,z);

	IMPLEMENT_VECTOR_ACCESSOR3(x,x,x); IMPLEMENT_VECTOR_ACCESSOR3(x,x,y); IMPLEMENT_VECTOR_ACCESSOR3(x,x,z);
	IMPLEMENT_VECTOR_ACCESSOR3(x,y,x); IMPLEMENT_VECTOR_ACCESSOR3(x,y,y); IMPLEMENT_VECTOR_ACCESSOR3(x,y,z);
	IMPLEMENT_VECTOR_ACCESSOR3(x,z,x); IMPLEMENT_VECTOR_ACCESSOR3(x,z,y); IMPLEMENT_VECTOR_ACCESSOR3(x,z,z);
	IMPLEMENT_VECTOR_ACCESSOR3(y,x,x); IMPLEMENT_VECTOR_ACCESSOR3(y,x,y); IMPLEMENT_VECTOR_ACCESSOR3(y,x,z);
	IMPLEMENT_VECTOR_ACCESSOR3(y,y,x); IMPLEMENT_VECTOR_ACCESSOR3(y,y,y); IMPLEMENT_VECTOR_ACCESSOR3(y,y,z);
	IMPLEMENT_VECTOR_ACCESSOR3(y,z,x); IMPLEMENT_VECTOR_ACCESSOR3(y,z,y); IMPLEMENT_VECTOR_ACCESSOR3(y,z,z);
	IMPLEMENT_VECTOR_ACCESSOR3(z,x,x); IMPLEMENT_VECTOR_ACCESSOR3(z,x,y); IMPLEMENT_VECTOR_ACCESSOR3(z,x,z);
	IMPLEMENT_VECTOR_ACCESSOR3(z,y,x); IMPLEMENT_VECTOR_ACCESSOR3(z,y,y); IMPLEMENT_VECTOR_ACCESSOR3(z,y,z);
	IMPLEMENT_VECTOR_ACCESSOR3(z,z,x); IMPLEMENT_VECTOR_ACCESSOR3(z,z,y); IMPLEMENT_VECTOR_ACCESSOR3(z,z,z);

	IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,z);

	IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,z);

	IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,z);
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,z);
#endif

	// depricated methods
	template<typename U>
	[[deprecated("Use common constructor instead!")]]
	Vector<U, 3>
	convert() const { return {*this}; }

protected:
	template<typename U>
	friend IOStream&
	operator<<(IOStream& os, const Vector<U, 3>& c);
};

template<typename U, typename T>
static inline Vector<T, 3> operator * (const U &lhs, const Vector<T, 3> &rhs)
{
	return rhs * lhs;
}

template<typename T, typename U>
static inline Vector<U, 3> operator * (const Matrix<T, 3, 3> &lhs, const Vector<U, 3> &rhs)
{
	return lhs * rhs.asMatrix();
}

// TODO Whats with these?
// template<typename T>
// static inline Vector<T,3>
// operator * (const T &lhs, const Vector<T,3> &rhs)
// {
// 	return rhs * lhs;
// }

// template<typename T, typename U>
// static inline Vector<T,3>
// operator * (const Matrix<T, 3, 3> &lhs, const Vector<U,3> &rhs)
// {
// 	return lhs * rhs.asTMatrix();
// }

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<typename U>
IOStream&
operator<< (IOStream& os, const Vector<U, 3>& v) {
	os << "x=" << v.x << "\ty=" << v.y << "\tz=" << v.z;
	return os;
}
#endif

using Vector3f = Vector<float, 3>;
using Vector3i = Vector<int16_t, 3>;
using Vector3u = Vector<uint16_t, 3>;

} // namespace modm