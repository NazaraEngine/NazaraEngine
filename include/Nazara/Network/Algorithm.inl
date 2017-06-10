// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Algorithm.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> HostToNet(T value)
	{
#ifdef NAZARA_LITTLE_ENDIAN
		return SwapBytes(value);
#else
		return value;
#endif
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, T> NetToHost(T value)
	{
#ifdef NAZARA_LITTLE_ENDIAN
		return SwapBytes(value);
#else
		return value;
#endif
	}
}

#include <Nazara/Network/DebugOff.hpp>
