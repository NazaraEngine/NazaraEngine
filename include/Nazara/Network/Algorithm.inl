// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Endianness.hpp>

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
