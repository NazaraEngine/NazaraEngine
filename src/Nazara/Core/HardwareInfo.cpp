// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Error.hpp>
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
	nzProcessorVendor s_vendorEnum = nzProcessorVendor_Unknown;
	bool s_capabilities[nzProcessorCap_Max+1] = {false};
	bool s_x64 = false;

	char s_vendor[12] = {'C', 'P', 'U', 'i', 's', 'U', 'n', 'k', 'n', 'o', 'w', 'n'};

	struct VendorString
	{
		nzProcessorVendor vendorEnum;
		char vendor[13]; // +1 pour le \0 automatiquement ajouté par le compilateur
	};
}

unsigned int NzHardwareInfo::GetProcessorCount()
{
	static unsigned int processorCount = std::max(NzHardwareInfoImpl::GetProcessorCount(), 1U);
	return processorCount;
}

nzProcessorVendor NzHardwareInfo::GetProcessorVendor()
{
	return s_vendorEnum;
}

void NzHardwareInfo::GetProcessorVendor(char vendor[12])
{
	std::memcpy(vendor, s_vendor, 12);
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
	if (!NzHardwareInfoImpl::IsCpuidSupported())
		return false;

	nzUInt32 result[4];

	NzHardwareInfoImpl::Cpuid(0, result);
	std::memcpy(&s_vendor[0], &result[1], 4);
	std::memcpy(&s_vendor[4], &result[3], 4);
	std::memcpy(&s_vendor[8], &result[2], 4);

	VendorString s_vendorStrings[] =
	{
		{nzProcessorVendor_AMD, "AMDisbetter!"},
		{nzProcessorVendor_AMD, "AuthenticAMD"},
		{nzProcessorVendor_Centaur, "CentaurHauls"},
		{nzProcessorVendor_Cyrix, "CyrixInstead"},
		{nzProcessorVendor_Intel, "GenuineIntel"},
		{nzProcessorVendor_NexGen, "NexGenDriven"},
		{nzProcessorVendor_NSC, "Geode by NSC"},
		{nzProcessorVendor_SIS, "SiS SiS SiS "},
		{nzProcessorVendor_Transmeta, "GenuineTMx86"},
		{nzProcessorVendor_Transmeta, "TransmetaCPU"},
		{nzProcessorVendor_UMC, "UMC UMC UMC "},
		{nzProcessorVendor_VIA, "VIA VIA VIA "},
		{nzProcessorVendor_VIA, "Vortex86 SoC"}
	};

	// Identification du vendeur
	s_vendorEnum = nzProcessorVendor_Unknown;
	for (const VendorString& vendorString : s_vendorStrings)
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
	}

	NzHardwareInfoImpl::Cpuid(0x80000000, result);
	unsigned int exIds = result[0];

	if (exIds >= 0x80000001)
	{
		NzHardwareInfoImpl::Cpuid(0x80000001, result);
		s_capabilities[nzProcessorCap_FMA4]  = (result[2] & (1U << 16)) != 0;
		s_capabilities[nzProcessorCap_SSE4a] = (result[2] & (1U <<  6)) != 0;
		s_capabilities[nzProcessorCap_XOP]   = (result[2] & (1U << 11)) != 0;
		s_x64 = (result[3] & (1U << 29)) != 0;
	}

	return true;
}

bool NzHardwareInfo::Is64Bits()
{
	return s_x64;
}

void NzHardwareInfo::Uninitialize()
{
	// Rien à faire
}
