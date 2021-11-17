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
 * \brief	Class for handling common vector operations (4D)
 *
 *	+ : addition of points
	*	- : different of points
	*	* : dot product or scalar multiplication
	*	/ : scalar division
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
class Vector<T, 4>
{
	using VecT1 = Vector<T, 1>;
	using VecT2 = Vector<T, 2>;
	using VecT3 = Vector<T, 3>;
	using VecT4 = Vector;

public:
	using WideType = GeometricTraits<T>::WideType;
	using WideWideType = GeometricTraits<WideType>::WideType;
	using FloatType = GeometricTraits<T>::FloatType;

	T x{0}, y{0}, z{0}, w{0};

	// basic constructors
	constexpr Vector() = default;

	constexpr explicit Vector(T xyzw)
		: x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr explicit Vector(T xyzw)
		: x(std::round(xyzw)), y(std::round(xyzw)), z(std::round(xyzw)), w(std::round(xyzw)) {}

	constexpr Vector(T x, T y, T z, T w)
		: x(x), y(y), z(z), w(w) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr Vector(U x, U y, U z, U w)
		: x(std::round(x)), y(std::round(y)), z(std::round(z)), w(std::round(w)) {}

	template<typename U>
	constexpr Vector(const Vector<U, 4> &v) : Vector(v.x, v.y, v.z, v.w) {}

	// vector constructors
	constexpr Vector(VecT1 vx, VecT1 vy, VecT1 vz, VecT1 vw) : Vector(vx.x, vy.x, vz.x, vw.x) {}
	constexpr Vector(VecT1 vx, VecT1 vy, VecT1 vz, T w) : Vector(vx.x, vy.x, vz.x, w) {}
	constexpr Vector(VecT1 vx, VecT1 vy, T z, T w) : Vector(vx.x, vy.x, z, w) {}
	constexpr Vector(VecT1 vx, T y, VecT1 vz, T w) : Vector(vx.x, y, vz.x, w) {}
	constexpr Vector(T x, VecT1 vy, VecT1 vz, T w) : Vector(x, vy.x, vz.x, w) {}
	constexpr Vector(VecT1 vx, T y, T z, T w) : Vector(vx.x, y, z, w) {}
	constexpr Vector(T x, VecT1 vy, T z, T w) : Vector(x, vy.x, z, w) {}
	constexpr Vector(VecT1 vx, VecT1 vy, T z, VecT1 vw) : Vector(vx.x, vy.x, z, vw.x) {}
	constexpr Vector(VecT1 vx, T y, T z, VecT1 vw) : Vector(vx.x, y, z, vw.x) {}
	constexpr Vector(T x, VecT1 vy, T z, VecT1 vw) : Vector(x, vy.x, z, vw.x) {}
	constexpr Vector(T x, T y, T z, VecT1 vw) : Vector(x, y, z, vw.x) {}
	constexpr Vector(VecT1 vx, T y, VecT1 vy, VecT1 vz, VecT1 vw) : Vector(vx.x, y, vy.x, vz.x, vw.x) {}
	constexpr Vector(T x, T y, VecT1 vz, VecT1 vw) : Vector(x, y, vz.x, vw.x) {}
	constexpr Vector(T x, VecT1 vy, VecT1 vz, VecT1 vw) : Vector(x, vy.x, vz.x, vw.x) {}

	constexpr Vector(VecT2 vxy, VecT1 vz, VecT1 vw) : Vector(vxy.x, vxy.y, vz.x, vw.x) {}
	constexpr Vector(VecT2 vxy, VecT1 vz, T w) : Vector(vxy.x, vxy.y, vz.x, w) {}
	constexpr Vector(VecT2 vxy, T z, T w) : Vector(vxy.x, vxy.y, z, w) {}
	constexpr Vector(VecT2 vxy, T z, VecT1 w) : Vector(vxy.x, vxy.y, z, w.x) {}

	constexpr Vector(VecT1 vx, VecT2 vyz, VecT1 vw) : Vector(vx.x, vyz.x, vyz.y, vw.x) {}
	constexpr Vector(VecT1 vx, VecT2 vyz, T w) : Vector(vx.x, vyz.x, vyz.y, w) {}
	constexpr Vector(T x, VecT2 vyz, T w) : Vector(x, vyz.x, vyz.y, w) {}
	constexpr Vector(T x, VecT2 vyz, VecT1 vw) : Vector(x, vyz.x, vyz.y, vw.x) {}

	constexpr Vector(VecT1 vx, VecT1 vy, VecT2 vzw) : Vector(vx.x, vy.x, vzw.x, vzw.y) {}
	constexpr Vector(VecT1 vx, T y, VecT2 vzw) : Vector(vx.x, y, vzw.x, vzw.y) {}
	constexpr Vector(T x, T y, VecT2 vzw) : Vector(x, y, vzw.x, vzw.y) {}
	constexpr Vector(T x, VecT1 vy, VecT2 vzw) : Vector(x, vy.x, vzw.x, vzw.y) {}

	constexpr Vector(VecT2 vxy, VecT2 vzw) : Vector(vxy.x, vxy.y, vzw.x, vzw.y) {}

	constexpr Vector(const VecT3 &vxyz, T w) : Vector(vxyz.x, vxyz.y, vxyz.z, w) {}
	constexpr Vector(const VecT3 &vxyz, VecT1 vw) : Vector(vxyz.x, vxyz.y, vxyz.z, vw.x) {}

	constexpr Vector(VecT1 vx, const VecT3 &vyzw) : Vector(vx.x, vyzw.x, vyzw.y, vyzw.z) {}
	constexpr Vector(T x, const VecT3 &vyzw) : Vector(x, vyzw.x, vyzw.y, vyzw.z) {}

	// matrix constructors
	constexpr Vector(const Matrix<T, 4, 1> &rhs)
		: x(reinterpret_cast<const T*>(&rhs)[0]),
		y(reinterpret_cast<const T*>(&rhs)[1]),
		z(reinterpret_cast<const T*>(&rhs)[2]),
		w(reinterpret_cast<const T*>(&rhs)[3])
	{}

	// array constructors
	template<typename U>
	constexpr explicit Vector(U *array) : Vector(array[0], array[1], array[2], array[3]) {}

	// as matrix convertors
	Matrix<T, 4, 1>&
	asMatrix() { return *(Matrix<T, 4, 1>*)this; }

	const Matrix<T, 4, 1>&
	asMatrix() const { return *(Matrix<T, 4, 1>*)this; }

	Matrix<T, 1, 4>&
	asTransposedMatrix() { return *(Matrix<T, 1, 4>*)this; }

	const Matrix<T, 1, 4>&
	asTransposedMatrix() const { return *(Matrix<T, 1, 4>*)this; }

	// matrix assignment
	Vector& operator= (const Matrix<T, 4, 1> &rhs) {
		x = reinterpret_cast<const T*>(&rhs)[0];
		y = reinterpret_cast<const T*>(&rhs)[1];
		z = reinterpret_cast<const T*>(&rhs)[2];
		w = reinterpret_cast<const T*>(&rhs)[3];

		return *this;
	}

	// getters
	T getX() const { return x; }
	T getY() const { return y; }
	T getZ() const { return z; }
	T getW() const { return w; }

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


	void setW(T w) { this->w = w; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setW(U w) { this->w = std::round(w); }


	void set(T x, T y, T z, T w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void set(U x, U y, U z, U w) {
		this->x = std::round(x);
		this->y = std::round(y);
		this->z = std::round(z);
		this->w = std::round(w);
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
	{ return Vector(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }

	constexpr Vector operator- (const Vector &rhs) const
	{ return Vector(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }

	constexpr T operator* (const Vector &rhs) const
	{ return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }

	// IMPLEMENT operator^ ?

	template <typename U>
	constexpr Vector operator* (U scale) const
	{ return Vector<U, 4>(x * scale, y * scale, z * scale, w * scale); }

	template <typename U>
	constexpr Vector operator/ (U scale) const
	{ return Vector<U, 4>(x / scale, y / scale, z / scale, w / scale); }

	Vector& operator+= (const Vector &rhs)
	{ x += rhs.x; y += rhs.y; z += rhs.z; w+= rhs.w; return *this; }

	Vector& operator-= (const Vector &rhs)
	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; w-= rhs.w; return *this; }

	Vector& operator *= (T scale)
	{ x *= scale; y *= scale; z *= scale; w *= scale; return *this; }

	Vector& operator /= (T scale)
	{ x /= scale; y /= scale; z /= scale; w /= scale; return *this; }

	// template <typename>
	// requires std::is_signed<T>::value
	constexpr Vector operator- () const
	{ return Vector(-x, -y, -z, -w); }

	// additional methods
	T getLength() const
	{ return std::sqrt(getLengthSquared()); }

	T getLength() const
	requires std::integral<T>
	{ return round(std::sqrt(getLengthSquared())); }

	WideWideType getLengthSquared() const
	{ return std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2) + std::pow(w, 2); }

	void scale(float newLength) { *this = scaled(newLength); }

	Vector scaled(float newLength) const {
		float scale = newLength / getLength();
		return *this * scale;
	}

	void normalize() { scale(1.0f); }
	Vector normalized() const { return scaled(1.0); }

	bool hasNan() const { return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w); }
	bool hasInf() const { return std::isinf(x) || std::isinf(y) || std::isinf(z) || std::isinf(w); }

#ifndef __DOXYGEN__
	IMPLEMENT_VECTOR_ACCESSOR2(x,x)	IMPLEMENT_VECTOR_ACCESSOR2(x,y)	IMPLEMENT_VECTOR_ACCESSOR2(x,z)	IMPLEMENT_VECTOR_ACCESSOR2(x,w)
	IMPLEMENT_VECTOR_ACCESSOR2(y,x)	IMPLEMENT_VECTOR_ACCESSOR2(y,y)	IMPLEMENT_VECTOR_ACCESSOR2(y,z)	IMPLEMENT_VECTOR_ACCESSOR2(y,w)
	IMPLEMENT_VECTOR_ACCESSOR2(z,x)	IMPLEMENT_VECTOR_ACCESSOR2(z,y)	IMPLEMENT_VECTOR_ACCESSOR2(z,z)	IMPLEMENT_VECTOR_ACCESSOR2(z,w)
	IMPLEMENT_VECTOR_ACCESSOR2(w,x)	IMPLEMENT_VECTOR_ACCESSOR2(w,y)	IMPLEMENT_VECTOR_ACCESSOR2(w,z)	IMPLEMENT_VECTOR_ACCESSOR2(w,w)

	IMPLEMENT_VECTOR_ACCESSOR3(x,x,x) IMPLEMENT_VECTOR_ACCESSOR3(x,x,y) IMPLEMENT_VECTOR_ACCESSOR3(x,x,z) IMPLEMENT_VECTOR_ACCESSOR3(x,x,w)
	IMPLEMENT_VECTOR_ACCESSOR3(x,y,x) IMPLEMENT_VECTOR_ACCESSOR3(x,y,y) IMPLEMENT_VECTOR_ACCESSOR3(x,y,z) IMPLEMENT_VECTOR_ACCESSOR3(x,y,w)
	IMPLEMENT_VECTOR_ACCESSOR3(x,z,x) IMPLEMENT_VECTOR_ACCESSOR3(x,z,y) IMPLEMENT_VECTOR_ACCESSOR3(x,z,z) IMPLEMENT_VECTOR_ACCESSOR3(x,z,w)
	IMPLEMENT_VECTOR_ACCESSOR3(y,x,x) IMPLEMENT_VECTOR_ACCESSOR3(y,x,y) IMPLEMENT_VECTOR_ACCESSOR3(y,x,z) IMPLEMENT_VECTOR_ACCESSOR3(y,x,w)
	IMPLEMENT_VECTOR_ACCESSOR3(y,y,x) IMPLEMENT_VECTOR_ACCESSOR3(y,y,y) IMPLEMENT_VECTOR_ACCESSOR3(y,y,z) IMPLEMENT_VECTOR_ACCESSOR3(y,y,w)
	IMPLEMENT_VECTOR_ACCESSOR3(y,z,x) IMPLEMENT_VECTOR_ACCESSOR3(y,z,y) IMPLEMENT_VECTOR_ACCESSOR3(y,z,z) IMPLEMENT_VECTOR_ACCESSOR3(y,z,w)
	IMPLEMENT_VECTOR_ACCESSOR3(z,x,x) IMPLEMENT_VECTOR_ACCESSOR3(z,x,y) IMPLEMENT_VECTOR_ACCESSOR3(z,x,z) IMPLEMENT_VECTOR_ACCESSOR3(z,x,w)
	IMPLEMENT_VECTOR_ACCESSOR3(z,y,x) IMPLEMENT_VECTOR_ACCESSOR3(z,y,y) IMPLEMENT_VECTOR_ACCESSOR3(z,y,z) IMPLEMENT_VECTOR_ACCESSOR3(z,y,w)
	IMPLEMENT_VECTOR_ACCESSOR3(z,z,x) IMPLEMENT_VECTOR_ACCESSOR3(z,z,y) IMPLEMENT_VECTOR_ACCESSOR3(z,z,z) IMPLEMENT_VECTOR_ACCESSOR3(z,z,w)
	IMPLEMENT_VECTOR_ACCESSOR3(w,x,x) IMPLEMENT_VECTOR_ACCESSOR3(w,x,y) IMPLEMENT_VECTOR_ACCESSOR3(w,x,z) IMPLEMENT_VECTOR_ACCESSOR3(w,x,w)
	IMPLEMENT_VECTOR_ACCESSOR3(w,y,x) IMPLEMENT_VECTOR_ACCESSOR3(w,y,y) IMPLEMENT_VECTOR_ACCESSOR3(w,y,z) IMPLEMENT_VECTOR_ACCESSOR3(w,y,w)
	IMPLEMENT_VECTOR_ACCESSOR3(w,z,x) IMPLEMENT_VECTOR_ACCESSOR3(w,z,y) IMPLEMENT_VECTOR_ACCESSOR3(w,z,z) IMPLEMENT_VECTOR_ACCESSOR3(w,z,w)

	IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,w)

	IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,w)

	IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,w)

	IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,w)
	IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,w)
#endif

	// depricated methods
	template<typename U>
	[[deprecated("Use common constructor instead!")]]
	Vector<U, 4>
	convert() const { return {*this}; }

protected:
	template<typename U>
	friend IOStream&
	operator<<(IOStream& os, const Vector<U, 4>& c);
};

template<typename U, typename T>
static inline Vector<T,4> operator* (const U &lhs, const Vector<T,4> &rhs)
{ return rhs * lhs; }

// Should be covered by the above
// template<typename T>
// static inline Vector<T,4> operator* (const T &lhs, const Vector<T,4> &rhs)
// {
// 	return rhs * lhs;
// }

template<typename T, typename U>
static inline Vector<U,4> operator* (const Matrix<T, 4, 4> &lhs, const Vector<U,4> &rhs)
{ return lhs * rhs.asTMatrix(); }

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<typename U>
IOStream&
operator<< (IOStream& os, const Vector<U, 4>& v) {
	os << "x=" << v.x << "\ty=" << v.y << "\tz=" << v.z << "\tw=" << v.w;
	return os;
}
#endif

using Vector4f = Vector<float, 4>;
using Vector4i = Vector<int16_t, 4>;
using Vector4u = Vector<uint16_t, 4>;

} // namespace modm