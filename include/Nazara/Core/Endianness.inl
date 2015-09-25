// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <algorithm>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Endianness GetPlatformEndianness()
	{
		#if defined(NAZARA_BIG_ENDIAN)
		return Endianness_BigEndian;
		#elif defined(NAZARA_LITTLE_ENDIAN)
		return Endianness_LittleEndian;
		#endif
	}

	inline void SwapBytes(void* buffer, unsigned int size)
	{
		UInt8* bytes = reinterpret_cast<UInt8*>(buffer);
		unsigned int i = 0;
		unsigned int j = size-1;

		while (i < j)
			std::swap(bytes[i++], bytes[j--]);
	}
}

#include <Nazara/Core/DebugOff.hpp>
