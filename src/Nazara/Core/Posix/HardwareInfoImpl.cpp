// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/HardwareInfoImpl.hpp>
#include <unistd.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void HardwareInfoImpl::Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 registers[4])
	{
	#if defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
		// https://en.wikipedia.org/wiki/CPUID
		asm volatile(
			#ifdef NAZARA_PLATFORM_x64
			"pushq %%rbx     \n\t" // save %rbx
			#else
			"pushl %%ebx     \n\t" // save %ebx
			#endif
			"cpuid            \n\t"
			"movl %%ebx ,%[ebx]  \n\t" // write the result into output var
			#ifdef NAZARA_PLATFORM_x64
			"popq %%rbx \n\t"
			#else
			"popl %%ebx \n\t"
			#endif
			: "=a"(registers[0]), [ebx] "=r"(registers[1]), "=c"(registers[2]), "=d"(registers[3])
			: "a"(functionId), "c" (subFunctionId));
	#else
		NazaraInternalError("Cpuid has been called although it is not supported");
	#endif
	}

	unsigned int HardwareInfoImpl::GetProcessorCount()
	{
		// Simpler (and more portable) than using CPUID
		return sysconf(_SC_NPROCESSORS_CONF);
	}

	UInt64 HardwareInfoImpl::GetTotalMemory()
	{
		UInt64 pages = sysconf(_SC_PHYS_PAGES);
		UInt64 page_size = sysconf(_SC_PAGE_SIZE);

		return pages * page_size;
	}

	bool HardwareInfoImpl::IsCpuidSupported()
	{
	#ifdef NAZARA_PLATFORM_x64
		return true; // cpuid is always supported on x64 arch
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
}
