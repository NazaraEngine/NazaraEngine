// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/HardwareInfoImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

void NzHardwareInfoImpl::Cpuid(nzUInt32 functionId, nzUInt32 subFunctionId, nzUInt32 registers[4])
{
#if defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
	// Source: http://stackoverflow.com/questions/1666093/cpuid-implementations-in-c
	asm volatile ("cpuid" // Besoin d'être volatile ?
	              : "=a" (registers[0]), "=b" (registers[1]), "=c" (registers[2]), "=d" (registers[3]) // output
	              : "a" (functionId), "c" (subFunctionId));                                // input
#else
	NazaraInternalError("Cpuid has been called although it is not supported");
#endif
}

unsigned int NzHardwareInfoImpl::GetProcessorCount()
{
	// Plus simple (et plus portable) que de passer par le CPUID
	return sysconf(_SC_NPROCESSORS_CONF);
}

nzUInt64 NzHardwareInfoImpl::GetTotalMemory()
{
	nzUInt64 pages = sysconf(_SC_PHYS_PAGES);
	nzUInt64 page_size = sysconf(_SC_PAGE_SIZE);

    return pages * page_size;
}

bool NzHardwareInfoImpl::IsCpuidSupported()
{
#ifdef NAZARA_PLATFORM_x64
	return true; // Toujours supporté sur un processeur 64 bits
#else
	#if defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
	int supported;
	asm volatile ("	pushfl\n"
	              " pop %%eax\n"
	              " mov %%eax, %%ecx\n"
	              " xor $0x200000, %%eax\n"
	              " push %%eax\n"
	              " popfl\n"
	              " pushfl\n"
	              " pop %%eax\n"
	              " xor %%ecx, %%eax\n"
	              " mov %%eax, %0\n"
	              " push %%ecx\n"
	              " popfl"
	              : "=m" (supported)         // output
	              :                          // input
	              : "eax", "ecx", "memory"); // clobbered register

	return supported != 0;
	#else
	return false;
	#endif
#endif
}
