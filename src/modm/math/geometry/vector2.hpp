/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
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

#include <cmath>
#include <concepts>

#include "angle.hpp"
#include "vector.hpp"

namespace modm
{
// forward declaration
template <typename T>
class Location2D;

/**
 * \brief	Class for handling common vector operations (2D)
 *
 * Operations:
 * \code
 *	+ : addition of points
	*	- : different of points
	*	* : dot product or scalar multiplication
	*	/ : scalar division
	*	^ : cross product (determinant)
	*	~ : perpendicular
	* \endcode
	*
	* Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com)
	* and heavily modified.
	*
	* \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	*
	* \author	Fabian Greif
	* \author	Niklas Hauser
	* \author	Thomas Sommer
	* \ingroup	modm_math_geometry
	*/
template<typename T>
class Vector<T, 2>
{
	using VecT1 = Vector<T, 1>;
	using VecT2 = Vector;

public:
	using WideType = GeometricTraits<T>::WideType;
	using FloatType = GeometricTraits<T>::FloatType;

	T x{0}, y{0};

	// fundamental constructors
	constexpr Vector() = default;

	constexpr explicit Vector(T xy)
		: x(xy), y(xy) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr explicit Vector(T xy)
		: x(std::round(xy)), y(std::round(xy)) {}

	constexpr Vector(T x, T y)
		: x(x), y(y) {}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr Vector(U x, U y)
		: x(std::round(x)), y(std::round(y)) {}

	template<typename U>
	constexpr Vector(Vector<U, 2> v) : Vector(v.x, v.y) {}

	// vector constructors
	constexpr Vector(VecT1 vx, VecT1 vy) : Vector(vx.x, vy.x) {}
	constexpr Vector(T x, VecT1 vy) : Vector(x, vy.x) {}
	constexpr Vector(VecT1 vx, T y) : Vector(vx.x, y) {}

	// matrix constructors
	constexpr Vector(const Matrix<T, 2, 1> &rhs)
		: Vector(reinterpret_cast<const T*>(&rhs)[0], reinterpret_cast<const T*>(&rhs)[1]) {}

	// array constructors
	template<typename U>
	constexpr explicit Vector(U *array) : Vector(array[0], array[1]) {}

	// as matrix convertors
	Matrix<T, 2, 1>&
	asMatrix() { return *((Matrix<T, 2, 1> *) this); }

	const Matrix<T, 2, 1>&
	asMatrix() const { return *((Matrix<T, 2, 1> *) this); }

	Matrix<T, 1, 2>&
	asTransposedMatrix() { return *((Matrix<T, 1, 2> *) this); }

	const Matrix<T, 1, 2>&
	asTransposedMatrix() const { return *((Matrix<T, 1, 2> *) this); }

	// matrix assignment
	Vector& operator= (const Matrix<T, 2, 1> &rhs) {
		x = reinterpret_cast<const T*>(&rhs)[0];
		y = reinterpret_cast<const T*>(&rhs)[1];

		return *this;
	}

	// getters
	T getX() const { return x; }
	T getY() const { return y; }

	// setters
	void setX(T x) { this->x = x; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setX(U x) { this->x = std::round(x); }


	void setY(T y) { this->y = y; }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void setY(U y) { this->x = std::round(y); }


	void set(T x, T y) {
		this->x = x;
		this->y = y;
	}

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	void set(U x, U y) {
		this->x = std::round(x);
		this->y = std::round(y);
	}

	// accessors
	T& operator[] (std::size_t index)
	{ return reinterpret_cast<T*>(this)[index]; }

	const T& operator[] (std::size_t index) const
	{ return reinterpret_cast<const T*>(this)[index]; }

	T* ptr() { return reinterpret_cast<T*>(this); }

	const T* ptr() const { return reinterpret_cast<const T*>(this); }

	// operators
	auto operator<=>(const Vector&) const = default;

	constexpr Vector operator+ (const Vector &rhs) const
	{ return Vector(x + rhs.x, y + rhs.y); }

	constexpr Vector operator- (const Vector &rhs) const
	{ return Vector(x - rhs.x, y - rhs.y); }

	constexpr T operator* (const Vector &rhs) const
	{ return x * rhs.x + y * rhs.y; }

	constexpr T operator^ (const Vector &rhs) const
	{ return x * rhs.y - y * rhs.x; }

	template <typename U>
	constexpr Vector operator* (U scale) const
	{ return Vector<U, 2>(x * scale, y * scale); }

	template <typename U>
	constexpr Vector operator/ (U scale) const
	{ return Vector<U, 2>(x / scale, y / scale); }

	Vector& operator+= (const Vector &rhs)
	{ x += rhs.x; y += rhs.y; return *this; }

	Vector& operator-= (const Vector &rhs)
	{ x -= rhs.x; y -= rhs.y; return *this; }

	template<typename U>
	Vector& operator*= (U scale)
	{ x *= scale; y *= scale; return *this; }

	template<typename U>
	Vector& operator/= (U scale)
	{ x /= scale; y /= scale; return *this; }

	Vector& operator~ ()
	{ *this = perpendicular(); return *this; }

	// template <typename>
	// requires std::is_signed<T>::value
	constexpr Vector operator- () const
	{ return Vector(-x, -y); }

	// additional methods
	T getLength() const
	{ return std::sqrt(getLengthSquared()); }

	T getLength() const
	requires std::integral<T>
	{ return round(std::sqrt(getLengthSquared())); }

	WideType getLengthSquared() const
	{ return std::pow(x, 2) + std::pow(y, 2); }

	WideType getDistanceTo(const Vector& other) const
	{ return (other - *this).getLength(); }

	float getAngle() const
	{ return std::atan2(y, x); }

	float getAngleTo(const Vector& other) const
	{ return (other - *this).getAngle(); }

	/**
	 * \brief	Normalize length to 1
	 *
	 * \warning	This method is only useful if T is a floating point type.
	 * 			For integer types the result might be wrong!
	 */
	Vector& normalize() {
		operator/=( getLength() );
		return *this;
	}

	Vector normalized() const {
		return *this / getLength();
	}

	/**
	 * \brief	Scale the vector to \p length
	 */
	Vector& scale(float length) {
		operator*=( length / getLength() );
		return *this;
	}

	Vector scaled(float length) const {
		return *this * (length / getLength());
	}

	// TODO implement as operator+=(Angle phi), operator-=(Angle phi)
	Vector&
	rotate(float phi) {
		const float c = std::cos(phi);
		const float s = std::sin(phi);
		const Vector<float, 2> tmp(c * x - s * y, s * x + c * y);
		set(tmp.x, tmp.y);

		return *this;
	}

	/**
	 * \brief	Same like operator+= but also returns reference
	 */
	Vector&
	translate(Vector v)
	{ operator+=(v); return *this; }

	/**
	 * \brief	Calculate the dot-product
	 *
	 * Also known as the scalar product.
	 *
	 * \code
	 * this.x * other.x + this.y * other.y
	 * \endcode
	 */
	WideType dot(const Vector& other) const
	{ return WideType(x) * WideType(other.x) + WideType(y) * WideType(other.y); }

	/**
	 * \brief	Calculate the cross-product
	 *
	 * In 2D there is no clear definition of this operation.
	 *
	 * This implementation is the most common one and will return the
	 * magnitude of the vector that would result from a regular
	 * 3D cross product of the input vectors, taking their Z values
	 * implicitly as 0 (i.e. treating the 2D space as a plane in the 3D space).
	 * The 3D cross product will be perpendicular to that plane, and thus
	 * have 0 X & Y components (thus the scalar returned is the Z value of
	 * the 3D cross product vector).
	 *
	 * \code
	 * this.x * other.y - this.y * other.x
	 * \endcode
	 *
	 * Other implementations take no arguments and returns a vector
	 * perpendicular to the input vector. This can be reached with the
	 * toOrthogonalVector() method, which returns a perpendicular copy
	 * of the vector.
	 */
	WideType cross(const Vector& other) const
	{ return WideType(x) * WideType(other.y) - WideType(y) * WideType(other.x); }

	Vector toOrthogonalVector() const
	{ return Vector(y, -x); }

	Vector perpendicular() const
	{ return Vector(y, -x); }

	/**
	 * \brief	Check if three points are in a counter-clock wise direction
	 *
	 * Check if we move counter-clock wise if we move from the first point
	 * to the second and the third.
	 *
	 * If all three points are in a line there are three possibilities:
	 *  1) strait line: third point behind the second (returns 1)
	 *  2) last point between the other two (returns 0)
	 *  3) third point before the first one (returns -1)
	 *
	 * This definition is useful for inclusion or intersection testing.
	 */
	static int8_t
	ccw(const Vector& a, const Vector& b, const Vector& c) {
		const Vector<WideType, 2> v1 = b - a;
		const Vector<WideType, 2> v2 = c - a;
		const WideType d1 = v1.x * v2.y;
		const WideType d2 = v1.y * v2.x;

		if (d1 > d2)
			return 1;
		else if (d1 < d2)
			return -1;
		else
		{
			if ((v1.x * v2.x < 0) || (v1.y * v2.y < 0))
				return -1;
			else
				return (v1.x * v1.x + v1.y * v1.y) < (v2.x * v2.x + v2.y * v2.y) ? 1 : 0;
		}
	}

	bool hasNan() const { return std::isnan(x) || std::isnan(y); }
	bool hasInf() const { return std::isinf(x) || std::isinf(y); }

#ifndef __DOXYGEN__
	IMPLEMENT_VECTOR_ACCESSOR2(x,x); IMPLEMENT_VECTOR_ACCESSOR2(x,y);
	IMPLEMENT_VECTOR_ACCESSOR2(y,x); IMPLEMENT_VECTOR_ACCESSOR2(y,y);

	IMPLEMENT_VECTOR_ACCESSOR3(x,x,x); IMPLEMENT_VECTOR_ACCESSOR3(x,x,y);
	IMPLEMENT_VECTOR_ACCESSOR3(x,y,x); IMPLEMENT_VECTOR_ACCESSOR3(x,y,y);
	IMPLEMENT_VECTOR_ACCESSOR3(y,x,x); IMPLEMENT_VECTOR_ACCESSOR3(y,x,y);
	IMPLEMENT_VECTOR_ACCESSOR3(y,y,x); IMPLEMENT_VECTOR_ACCESSOR3(y,y,y);

	IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y);
	IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y);
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y);
	IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y);

	IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y);
	IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y);
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y);
	IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y);
#endif

	// depricated methods
	template<typename U>
	[[deprecated("Use common constructor instead!")]]
	Vector<U, 2>
	convert() const { return {*this}; }

protected:
	template<typename U>
	friend Vector<U, 2>
	operator* (float scale, const Vector<U, 2> &vector);

	template<typename U>
	friend IOStream&
	operator<<(IOStream& os, const Vector<U, 2>& c);
};

/**
 * \brief	Scalar multiplication
 * \ingroup	modm_math_geometry
 */
template<typename U>
Vector<U, 2>
operator* (float scale, const Vector<U, 2> &vector)
{ return vector * scale; }

/**
 * \brief	Scalar division
 * \ingroup	modm_math_geometry
 */
template<typename U>
Vector<U, 2>
operator/ (float scale, const Vector<U, 2> &vector)
{ return vector / scale; }

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<typename U>
IOStream&
operator<< (IOStream& os, const Vector<U, 2>& v) {
	os << "x=" << v.x << "\ty=" << v.y;
	return os;
}
#endif

using Vector2f = Vector<float, 2>;
using Vector2i = Vector<int16_t, 2>;
using Vector2u = Vector<uint16_t, 2>;

} // namespace modm