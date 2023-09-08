// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Endianness.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> HostToNet(T value)
	{
		return HostToBigEndian(value);
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> NetToHost(T value)
	{
		return BigEndianToHost(value);
	}
}

#include <Nazara/Network/DebugOff.hpp>
