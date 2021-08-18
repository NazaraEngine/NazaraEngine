// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_NETWORK_HPP
#define NAZARA_ALGORITHM_NETWORK_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Enums.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

namespace Nz
{
	NAZARA_NETWORK_API const char* ErrorToString(ResolveError resolveError);
	NAZARA_NETWORK_API const char* ErrorToString(SocketError socketError);

	NAZARA_NETWORK_API bool ParseIPAddress(const char* addressPtr, UInt8 result[16], UInt16* port = nullptr, bool* isIPv6 = nullptr, const char** endOfRead = nullptr);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> HostToNet(T value);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> NetToHost(T value);
}

#include <Nazara/Network/Algorithm.inl>

#endif // NAZARA_ALGORITHM_NETWORK_HPP
