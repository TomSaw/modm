/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
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

#include <modm/io/iostream.hpp>

#include "angle.hpp"
#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Location in a 2D coordinate system
	 *
	 * This class is primarily used to describe the location of a robot
	 * on the field. The robot has a position (x- and y-coordinate) and a
	 * orientation (absolute angle in the range (-pi, pi)).
	 *
	 * \ingroup	modm_math_geometry
	 */
	template <typename T = int16_t>
	class Location2D
	{
	public:
		Vector<T, 2> position;
		float orientation = 0;

		constexpr Location2D() = default;
		constexpr Location2D(const Vector<T, 2>& position, const float& orientation)
			: position(position), orientation(orientation) {}

		[[deprecated("Use 'setPosition({x, y}, orientation)' instead!")]]
		constexpr Location2D(const T& x, const T& y, const float& orientation)
			: position(x, y), orientation(orientation) {}

		template<typename U>
		constexpr Location2D(const Location2D<U> &l) : position(l.position), orientation(l.orientation) {}

		// getters and setters
		void setPosition(const Vector<T, 2>& position) { this->position = position; }

		[[deprecated("Use 'setPosition({x, y}' instead!")]]
		void setPosition(T x, T y) { this->position.x = x; this->position.y = y; }
		void setOrientation(const float orientation) { this->orientation = orientation; }

		Vector<T, 2> getPosition() const { return position; }
		inline float getOrientation() const { return orientation; }
		T getX() const { return position.x; }
		T getY() const { return position.y; }

		bool operator== (const Location2D &other) const {
			return (
				position == other.position and
				std::abs(orientation - other.orientation) < __FLT_EPSILON__
			);
		}
		bool operator!= (const Location2D &other) const {
			return (
				position != other.position or
				std::abs(orientation - other.orientation) > __FLT_EPSILON__
			);
		}

		/// Add a position increment
		void move(const Location2D& diff) {
			Vector<T, 2> movement = diff.position;
			movement.rotate(orientation);

			position.translate(movement);
			orientation = Angle::normalize(orientation + diff.orientation);
		}

		void move(const Vector<T, 2>& diff) {
			Vector<T, 2> movement(diff);
			movement.rotate(orientation);

			position.translate(movement);
		}

		/**
		 * \brief	Add a increment only in x-direction
		 *
		 * Our robots mostly use a differential drive with two driven wheels
		 * side by side, allowing the robot to move only in the drive direction
		 * (x-direction in the local coordinate system of the robot) and
		 * rotate. A movement perpendicular to the drive direction is
		 * impossible without an external force.
		 *
		 * To estimate the position of the robot over time, we use odometry.
		 * Therefore it is necessary to add a lot small increments of
		 * movement over time.
		 * Because the y-component will always be zero, we created this
		 * method, which avoids unnecessary computations for the y-component
		 * and is therefore faster than the universal move-method.
		 *
		 * \param	x		movement in x-direction
		 * \param	phi		rotation
		 */
		void
		move(T x, float phi) {
			Vector<T, 2> vector(Vector<float, 2>(x * std::cos(orientation), x * std::sin(orientation)));
			position.translate(vector);

			orientation = Angle::normalize(orientation + phi);
		}

		// TODO
		Vector<T, 2> translated(const Vector<T, 2>& vector) const {
			Vector<T, 2> result(vector);
			result.rotate(orientation);
			result.translate(position);

			return result;
		}

	private:
		template <typename U>
		friend IOStream&
		operator <<( IOStream&, const Location2D<U>&);
	};

	template<typename T>
	IOStream&
	operator<< (IOStream& os, const Location2D<T>& location) {
		os << location.position << ", phi=" << location.orientation;
		return os;
	}
}