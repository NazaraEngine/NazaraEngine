// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <algorithm>
#include <Nazara/Core/Debug.hpp>

inline void NzByteSwap(void* buffer, unsigned int size)
{
	nzUInt8* bytes = reinterpret_cast<nzUInt8*>(buffer);
	unsigned int i = 0;
	unsigned int j = size-1;

	while (i < j)
		std::swap(bytes[i++], bytes[j--]);
}

inline nzEndianness NzGetPlatformEndianness()
{
	#if defined(NAZARA_BIG_ENDIAN)
	return nzEndianness_BigEndian;
	#elif defined(NAZARA_LITTLE_ENDIAN)
	return nzEndianness_LittleEndian;
	#endif
}

#include <Nazara/Core/DebugOff.hpp>
