// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

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
#if NAZARA_ENDIANNESS_DETECTED
	return NazaraEndianness;
#else
	static nzEndianness endianness = nzEndianness_Unknown;
	static bool tested = false;
	if (!tested)
	{
		nzUInt32 i = 1;
		nzUInt8* p = reinterpret_cast<nzUInt8*>(&i);

		// Méthode de récupération de l'endianness au runtime
		if (p[0] == 1)
			endianness = nzEndianness_LittleEndian;
		else if (p[3] == 1)
			endianness = nzEndianness_BigEndian;

		tested = true;
	}

	return endianness;
#endif
}
