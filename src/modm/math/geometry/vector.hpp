/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Martin Rosekeit
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
#include <numeric>
#include <algorithm>
#include <modm/math/matrix.hpp>

#include "geometric_traits.hpp"

namespace modm
{
// forward declaration
template<typename T, std::size_t, std::size_t>
class Matrix;

/**
 * \brief	Class for handling common point operations
 *
 * Basic data type of all geometric operations. Used to represent vectors
 * as well as particular points in the coordinate system.
 *
 * \section	point_vector	Point vs. vector
 *
 * In geometry, it is often convenient to use vector arithmetic to
 * represent points.
 *
 * A vector, by its definition, has no fixed starting point, but if we
 * imagine the starting point of a vector to be the origin, then the
 * endpoint of the vector represents a particular point.
 *
 * In this manner, every vector can be said to identify a unique point,
 * which is the endpoint of the vector when its starting point is the
 * origin.
 *
 * Therefore there isn't a Point-class, but only a Vector class.
 *
 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
 *
 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
 *
 * \ingroup	modm_math_geometry
 * \author	Niklas Hauser
 * \author	Thomas Sommer
 */
template<typename T, std::size_t N>
class Vector
{
public:
	static constexpr std::size_t size = N;

	T coords[N] = {0};

	// fundamental constructors
	constexpr Vector() = default;

	constexpr explicit Vector(T v)
	{ std::fill(coords, coords + N, v); }

	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr explicit Vector(U v)
	{ std::fill(coords, coords + N, std::round(v)); }

	constexpr Vector(const T (&arr)[N])
	{ std::copy(arr, arr + N, coords); }

	// TODO need std::round
	template<typename U>
	requires std::integral<T> && std::floating_point<U>
	constexpr Vector(const U (&arr)[N])
	{ std::copy(arr, arr + N, coords); }

	// matrix constructor
	constexpr Vector(const Matrix<T, N, 1> &rhs)
	{ std::copy(rhs, rhs + N, coords); }

	// as matrix convertors
	Matrix<T, N, 1>&
	asMatrix()
	{ return *reinterpret_cast<modm::Matrix<T, N, 1>*>(this); }

	const Matrix<T, N, 1>&
	asMatrix() const
	{ return *reinterpret_cast<const modm::Matrix<T, N, 1>*>(this); }

	Matrix<T, 1, N>&
	asTransposedMatrix()
	{ return *reinterpret_cast<const modm::Matrix<T, 1, N>*>(this); }

	const Matrix<T, 1, N>&
	asTransposedMatrix() const
	{ return *reinterpret_cast<modm::Matrix<T, 1, N>*>(this); }

	// matrix assignment
	Vector& operator= (const Matrix<T, N, 1> &rhs) {
		std::copy(coords, coords + N, &rhs);
		return *this;
	}

	// accessors
	T& operator [] (std::size_t index)
	{ return coords[index]; }

	const T& operator [] (std::size_t index) const
	{ return coords[index]; }

	T* ptr() { return coords; }

	const T* ptr() const { return reinterpret_cast<const T*>(coords); }

	// operators
	auto operator<=>(const Vector &) const = default;

	Vector operator+ (const Vector &rhs) const {
		Vector ret;
		std::transform(coords, coords + N, rhs.coords, ret.coords, std::plus<T>());
		return ret;
	}

	Vector operator- (const Vector &rhs) const {
		Vector ret;
		std::transform(coords, coords + N, rhs.coords, ret.coords, std::minus<T>());
		return ret;
	}

	T operator* (const Vector &rhs) const {
		T tmp[N];
		std::transform(coords, coords + N, rhs.coords, tmp, std::multiplies<T>());
		return std::accumulate(tmp, tmp + N, 0);
	}

	Vector operator* (const T &rhs) const {
		Vector ret;
		std::transform(coords, coords + N, ret.coords, [=] (T c) {return c * rhs;});
		return ret;
	}

	Vector operator/ (const T &rhs) const {
		Vector ret;
		std::transform(coords, coords + N, ret.coords, [=] (T c) {return c / rhs;});
		return ret;
	}

	Vector& operator+= (const Vector &rhs) {
		std::transform(coords, coords + N, rhs.coords, coords, std::plus<T>());
		return *this;
	}

	Vector& operator-= (const Vector &rhs) {
		std::transform(coords, coords + N, rhs.coords, coords, std::minus<T>());
		return *this;
	}

	Vector& operator*= (const T &rhs) {
		std::transform(coords, coords + N, coords, [=] (T c) {return c * rhs;});
		return *this;
	}

	Vector& operator/= (const T &rhs) {
		std::transform(coords, coords + N, coords, [=] (T c) {return c / rhs;});
		return *this;
	}

	// template <typename>
	// requires std::is_signed<T>::value
	constexpr Vector operator- () {
		Vector ret;
		std::transform(coords, coords + N, ret.coords, std::negate<T>());
		return ret;
	}

	// additional methods
	T getLength() const
	{ return std::sqrt(getLengthSquared()); }

	T getLengthSquared() const {
		T tmp[N];
		// Better do std::pow(c, 2);
		std::transform(coords, coords + N, coords, tmp, std::multiplies<T>());
		return std::accumulate(tmp, tmp + N, 0);
	}

	// IMPLEMENT operator<<
};

namespace detail {
	template< typename T, std::size_t N >
	struct MakeSigned< Vector<T, N> >
	{ using type = Vector< SignedType<T>, N >; };

	template< typename T, std::size_t N >
	struct MakeUnsigned< Vector<T, N> >
	{ using type = Vector< UnsignedType<T>, N >; };

	template< typename T, std::size_t N >
	struct WideType< Vector<T, N> >
	{ using type = Vector< WideType<T>, N >; };

} // namespace detail

} // namespace modm	

#define IMPLEMENT_VECTOR_ACCESSOR2(a,b)		\
	Vector<T, 2> a##b() const				\
	{										\
		return Vector<T, 2>(a, b);			\
	}

#define IMPLEMENT_VECTOR_ACCESSOR3(a, b, c)	\
	Vector<T, 3> a##b##c() const			\
	{										\
		return Vector<T, 3>(a, b, c);		\
	}

#define IMPLEMENT_VECTOR_ACCESSOR4(a, b, c, d)\
	Vector<T, 4> a##b##c##d() const			\
	{										\
		return Vector<T, 4>(a, b, c, d);	\
	}

#include "vector1.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"