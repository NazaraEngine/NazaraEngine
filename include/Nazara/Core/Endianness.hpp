// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENDIANNESS_HPP
#define NAZARA_ENDIANNESS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>

#if !defined(NAZARA_BIG_ENDIAN) && !defined(NAZARA_LITTLE_ENDIAN)
	// Automatic detection following macros of compiler
	#if defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || \
		defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__) || defined(__hppa__)
		#define NAZARA_BIG_ENDIAN
	#elif defined(__i386__) || defined(__i386) || defined(__X86__) || defined (__x86_64) || defined(_M_I86) || \
		defined(_M_IX86) || defined(_M_X64)
		#define NAZARA_LITTLE_ENDIAN
	#else
		#error Failed to identify endianness, you must define either NAZARA_BIG_ENDIAN or NAZARA_LITTLE_ENDIAN
	#endif
#elif defined(NAZARA_BIG_ENDIAN) && defined(NAZARA_LITTLE_ENDIAN)
	#error You cannot define both NAZARA_BIG_ENDIAN and NAZARA_LITTLE_ENDIAN
#endif

namespace Nz
{
	inline constexpr Endianness GetPlatformEndianness();
	inline void SwapBytes(void* buffer, std::size_t size);
	template<typename T> T SwapBytes(T value);
}

#include <Nazara/Core/Endianness.inl>

#endif // NAZARA_ENDIANNESS_HPP
