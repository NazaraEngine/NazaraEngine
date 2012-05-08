// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENDIANNESS_HPP
#define NAZARA_ENDIANNESS_HPP

#include <Nazara/Prerequesites.hpp>

#if defined(NAZARA_ENDIANNESS_BIGENDIAN)
	#define NAZARA_ENDIANNESS_DETECTED 1
	#define NazaraEndianness nzEndianness_BigEndian
#elif defined(NAZARA_ENDIANNESS_LITTLEENDIAN)
	#define NAZARA_ENDIANNESS_DETECTED 1
	#define NazaraEndianness nzEndianness_LittleEndian
#else
	#if defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || \
		defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__) || defined(__hppa__)
		#define NAZARA_ENDIANNESS_DETECTED 1
		#define NAZARA_ENDIANNESS_BIGENDIAN
		#define NazaraEndianness nzEndianness_BigEndian
	#elif defined(__i386__) || defined(__i386) || defined(__X86__) || defined (__x86_64)
		#define NAZARA_ENDIANNESS_DETECTED 1
		#define NAZARA_ENDIANNESS_LITTLEENDIAN
		#define NazaraEndianness nzEndianness_LittleEndian
	#else
		#define NAZARA_ENDIANNESS_DETECTED 0
		#define NazaraEndianness NzGetPlatformEndianness()
	#endif
#endif

enum nzEndianness
{
	nzEndianness_BigEndian,
	nzEndianness_LittleEndian,
	nzEndianness_Unknown
};

inline void NzByteSwap(void* buffer, unsigned int size);
inline nzEndianness NzGetPlatformEndianness();

#include <Nazara/Core/Endianness.inl>

#endif // NAZARA_ENDIANNESS_HPP
