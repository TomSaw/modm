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

#include <cxxabi.h>

#include <modm/io/iostream.hpp>
#include <typeinfo>

namespace modm::log
{

template<typename T>
modm::IOStream&
demangle(modm::IOStream& os)
{
	static int status;
	static char* realname;

	realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);

	os << realname;
	return os;
}

/**
 * @brief 	Makro to iostream human readable typename T
 * 			Usage:
 * 			using T = AnyTypeYouWant;
 * 			MODM_LOG_DEBUG << MODM_PRETTY_TYPEINFO(T);
 */
#define MODM_PRETTY_TYPEINFO(T) "[" #T " = " << modm::log::demangle<T> << "]"

}  // namespace modm::log