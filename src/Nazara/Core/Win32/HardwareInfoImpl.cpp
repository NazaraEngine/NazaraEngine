// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/HardwareInfoImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <windows.h>

#ifdef NAZARA_COMPILER_MSVC
	#include <intrin.h>
#endif

#include <Nazara/Core/Debug.hpp>

void NzHardwareInfoImpl::Cpuid(nzUInt32 functionId, nzUInt32 subFunctionId, nzUInt32 registers[4])
{
#if defined(NAZARA_COMPILER_MSVC)
	static_assert(sizeof(nzUInt32) == sizeof(int), "Assertion failed");

	// Visual propose une fonction intrinsèque pour le cpuid
	__cpuidex(reinterpret_cast<int*>(registers), static_cast<int>(functionId), static_cast<int>(subFunctionId));
#elif defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
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
	SYSTEM_INFO infos;
	GetNativeSystemInfo(&infos);

	return infos.dwNumberOfProcessors;
}

nzUInt64 NzHardwareInfoImpl::GetTotalMemory()
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);

	return memStatus.ullTotalPhys;
}

bool NzHardwareInfoImpl::IsCpuidSupported()
{
#ifdef NAZARA_PLATFORM_x64
	return true; // Toujours supporté sur un processeur 64 bits
#else
	#if defined(NAZARA_COMPILER_MSVC)
	int supported;
	__asm
	{
		pushfd
		pop  eax
		mov  ecx, eax
		xor  eax, 0x200000
		push eax
		popfd
		pushfd
		pop  eax
		xor  eax, ecx
		mov  supported, eax
		push ecx
		popfd
	};

	return supported != 0;
	#elif defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
	int supported;
	asm volatile (" pushfl\n"
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
