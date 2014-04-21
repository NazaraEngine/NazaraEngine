// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstdlib>
#include <cstring>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/HardwareInfoImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/HardwareInfoImpl.hpp>
#else
	#error Lack of implementation: HardwareInfo
#endif

#include <Nazara/Core/Debug.hpp>

namespace
{
	struct VendorString
	{
		char vendor[13]; // +1 pour le \0 automatiquement ajouté par le compilateur
		nzProcessorVendor vendorEnum;
	};

	// Exceptionellement, la valeur "unknown" est intégrée
	const char* vendorNames[] =
	{
		"Unknown",                             // nzProcessorVendor_Unknown
		"Advanced Micro Devices",              // nzProcessorVendor_AMD
		"Centaur Technology",                  // nzProcessorVendor_Centaur
		"Cyrix Corporation",                   // nzProcessorVendor_Cyrix
		"Intel Corporation",                   // nzProcessorVendor_Intel
		"Kernel-based Virtual Machine",        // nzProcessorVendor_KVM
		"Microsoft Hyper-V",                   // nzProcessorVendor_HyperV
		"National Semiconductor",              // nzProcessorVendor_NSC
		"NexGen",                              // nzProcessorVendor_NexGen
		"Rise Technology",                     // nzProcessorVendor_Rise
		"Silicon Integrated Systems",          // nzProcessorVendor_SIS
		"Transmeta Corporation",               // nzProcessorVendor_Transmeta
		"United Microelectronics Corporation", // nzProcessorVendor_UMC
		"VIA Technologies",                    // nzProcessorVendor_VIA
		"VMware",                              // nzProcessorVendor_VMware
		"Vortex86",                            // nzProcessorVendor_Vortex
		"Xen"                                  // nzProcessorVendor_XenHVM
	};

	static_assert(sizeof(vendorNames)/sizeof(const char*) == nzProcessorVendor_Max+2, "Processor vendor name array is incomplete");

	VendorString vendorStrings[] =
	{
		// Triés par ordre alphabétique (Majuscules primant sur minuscules)
		{"AMDisbetter!", nzProcessorVendor_AMD},
		{"AuthenticAMD", nzProcessorVendor_AMD},
		{"CentaurHauls", nzProcessorVendor_Centaur},
		{"CyrixInstead", nzProcessorVendor_Cyrix},
		{"GenuineIntel", nzProcessorVendor_Intel},
		{"GenuineTMx86", nzProcessorVendor_Transmeta},
		{"Geode by NSC", nzProcessorVendor_NSC},
		{"KVMKVMKVMKVM", nzProcessorVendor_KVM},
		{"Microsoft Hv", nzProcessorVendor_HyperV},
		{"NexGenDriven", nzProcessorVendor_NexGen},
		{"RiseRiseRise", nzProcessorVendor_Rise},
		{"SiS SiS SiS ", nzProcessorVendor_SIS},
		{"TransmetaCPU", nzProcessorVendor_Transmeta},
		{"UMC UMC UMC ", nzProcessorVendor_UMC},
		{"VIA VIA VIA ", nzProcessorVendor_VIA},
		{"VMwareVMware", nzProcessorVendor_VMware},
		{"Vortex86 SoC", nzProcessorVendor_Vortex},
		{"XenVMMXenVMM", nzProcessorVendor_XenHVM}
	};

	nzProcessorVendor s_vendorEnum = nzProcessorVendor_Unknown;
	bool s_capabilities[nzProcessorCap_Max+1] = {false};
	bool s_initialized = false;

	char s_brandString[48] = "Not initialized";
	char s_vendor[12] = {'C', 'P', 'U', 'i', 's', 'U', 'n', 'k', 'n', 'o', 'w', 'n'};
}

NzString NzHardwareInfo::GetProcessorBrandString()
{
	if (!Initialize())
		NazaraError("Failed to initialize HardwareInfo");

	return s_brandString;
}

unsigned int NzHardwareInfo::GetProcessorCount()
{
	///DOC: Ne nécessite pas l'initialisation de HardwareInfo pour fonctionner
	static unsigned int processorCount = std::max(NzHardwareInfoImpl::GetProcessorCount(), 1U);
	return processorCount;
}

nzProcessorVendor NzHardwareInfo::GetProcessorVendor()
{
	if (!Initialize())
		NazaraError("Failed to initialize HardwareInfo");

	return s_vendorEnum;
}

NzString NzHardwareInfo::GetProcessorVendorName()
{
	if (!Initialize())
		NazaraError("Failed to initialize HardwareInfo");

	return vendorNames[s_vendorEnum+1];
}

bool NzHardwareInfo::HasCapability(nzProcessorCap capability)
{
	#ifdef NAZARA_DEBUG
	if (capability > nzProcessorCap_Max)
	{
		NazaraError("Capability type out of enum");
		return false;
	}
	#endif

	return s_capabilities[capability];
}

bool NzHardwareInfo::Initialize()
{
	if (s_initialized)
		return true;

	if (!NzHardwareInfoImpl::IsCpuidSupported())
	{
		NazaraError("Cpuid is not supported");
		return false;
	}

	s_initialized = true;

	nzUInt32 result[4];

	NzHardwareInfoImpl::Cpuid(0, result);
	std::memcpy(&s_vendor[0], &result[1], 4);
	std::memcpy(&s_vendor[4], &result[3], 4);
	std::memcpy(&s_vendor[8], &result[2], 4);

	// Identification du concepteur
	s_vendorEnum = nzProcessorVendor_Unknown;
	for (const VendorString& vendorString : vendorStrings)
	{
		if (std::memcmp(s_vendor, vendorString.vendor, 12) == 0)
		{
			s_vendorEnum = vendorString.vendorEnum;
			break;
		}
	}

	unsigned int ids = result[0];

	if (ids >= 1)
	{
		NzHardwareInfoImpl::Cpuid(1, result);
		s_capabilities[nzProcessorCap_AVX]   = (result[2] & (1U << 28)) != 0;
		s_capabilities[nzProcessorCap_FMA3]  = (result[2] & (1U << 12)) != 0;
		s_capabilities[nzProcessorCap_MMX]   = (result[3] & (1U << 23)) != 0;
		s_capabilities[nzProcessorCap_SSE]   = (result[3] & (1U << 25)) != 0;
		s_capabilities[nzProcessorCap_SSE2]  = (result[3] & (1U << 26)) != 0;
		s_capabilities[nzProcessorCap_SSE3]  = (result[2] & (1U <<  0)) != 0;
		s_capabilities[nzProcessorCap_SSSE3] = (result[2] & (1U <<  9)) != 0;
		s_capabilities[nzProcessorCap_SSE41] = (result[2] & (1U << 19)) != 0;
		s_capabilities[nzProcessorCap_SSE42] = (result[2] & (1U << 20)) != 0;

		NzHardwareInfoImpl::Cpuid(0x80000000, result);
		unsigned int exIds = result[0];

		if (exIds >= 0x80000001)
		{
			NzHardwareInfoImpl::Cpuid(0x80000001, result);
			s_capabilities[nzProcessorCap_x64]   = (result[3] & (1U << 29)) != 0;
			s_capabilities[nzProcessorCap_FMA4]  = (result[2] & (1U << 16)) != 0;
			s_capabilities[nzProcessorCap_SSE4a] = (result[2] & (1U <<  6)) != 0;
			s_capabilities[nzProcessorCap_XOP]   = (result[2] & (1U << 11)) != 0;

			if (exIds >= 0x80000004)
			{
				char* ptr = &s_brandString[0];
				for (nzUInt32 code = 0x80000002; code <= 0x80000004; ++code)
				{
					NzHardwareInfoImpl::Cpuid(code, result);
					std::memcpy(ptr, &result[0], 16);

					ptr += 16;
				}
			}
		}
	}

	return true;
}

bool NzHardwareInfo::IsInitialized()
{
	return s_initialized;
}

void NzHardwareInfo::Uninitialize()
{
	// Rien à faire
	s_initialized = false;
}
