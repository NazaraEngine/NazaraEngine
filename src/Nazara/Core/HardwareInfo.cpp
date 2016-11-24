// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
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

namespace Nz
{
	namespace
	{
		struct VendorString
		{
			char vendor[13]; // +1 pour le \0 automatiquement ajouté par le compilateur
			ProcessorVendor vendorEnum;
		};

		// Exceptionellement, la valeur "unknown" est intégrée
		const char* vendorNames[] =
		{
			"Unknown",                             // ProcessorVendor_Unknown
			"Advanced Micro Devices",              // ProcessorVendor_AMD
			"Centaur Technology",                  // ProcessorVendor_Centaur
			"Cyrix Corporation",                   // ProcessorVendor_Cyrix
			"Intel Corporation",                   // ProcessorVendor_Intel
			"Kernel-based Virtual Machine",        // ProcessorVendor_KVM
			"Microsoft Hyper-V",                   // ProcessorVendor_HyperV
			"National Semiconductor",              // ProcessorVendor_NSC
			"NexGen",                              // ProcessorVendor_NexGen
			"Rise Technology",                     // ProcessorVendor_Rise
			"Silicon Integrated Systems",          // ProcessorVendor_SIS
			"Transmeta Corporation",               // ProcessorVendor_Transmeta
			"United Microelectronics Corporation", // ProcessorVendor_UMC
			"VIA Technologies",                    // ProcessorVendor_VIA
			"VMware",                              // ProcessorVendor_VMware
			"Vortex86",                            // ProcessorVendor_Vortex
			"Xen"                                  // ProcessorVendor_XenHVM
		};

		static_assert(sizeof(vendorNames)/sizeof(const char*) == ProcessorVendor_Max+2, "Processor vendor name array is incomplete");

		VendorString vendorStrings[] =
		{
			// Triés par ordre alphabétique (Majuscules primant sur minuscules)
			{"AMDisbetter!", ProcessorVendor_AMD},
			{"AuthenticAMD", ProcessorVendor_AMD},
			{"CentaurHauls", ProcessorVendor_Centaur},
			{"CyrixInstead", ProcessorVendor_Cyrix},
			{"GenuineIntel", ProcessorVendor_Intel},
			{"GenuineTMx86", ProcessorVendor_Transmeta},
			{"Geode by NSC", ProcessorVendor_NSC},
			{"KVMKVMKVMKVM", ProcessorVendor_KVM},
			{"Microsoft Hv", ProcessorVendor_HyperV},
			{"NexGenDriven", ProcessorVendor_NexGen},
			{"RiseRiseRise", ProcessorVendor_Rise},
			{"SiS SiS SiS ", ProcessorVendor_SIS},
			{"TransmetaCPU", ProcessorVendor_Transmeta},
			{"UMC UMC UMC ", ProcessorVendor_UMC},
			{"VIA VIA VIA ", ProcessorVendor_VIA},
			{"VMwareVMware", ProcessorVendor_VMware},
			{"Vortex86 SoC", ProcessorVendor_Vortex},
			{"XenVMMXenVMM", ProcessorVendor_XenHVM}
		};

		ProcessorVendor s_vendorEnum = ProcessorVendor_Unknown;
		bool s_capabilities[ProcessorCap_Max+1] = {false};
		bool s_initialized = false;

		char s_brandString[48] = "Not initialized";
	}

	/*!
	* \ingroup core
	* \class Nz::HardwareInfo
	* \brief Core class that represents the info we can get from hardware
	*/

	/*!
	* \brief Generates the cpuid instruction (available on x86 & x64)
	*
	* \param functionId Information to retrieve
	* \param subFunctionId Additional code for information retrieval
	* \param result Supported features of the CPU
	*/

	void HardwareInfo::Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 result[4])
	{
		return HardwareInfoImpl::Cpuid(functionId, subFunctionId, result);
	}

	/*!
	* \brief Gets the brand of the processor
	* \return String of the brand
	*
	* \remark Produces a NazaraError if not Initialize
	*/

	String HardwareInfo::GetProcessorBrandString()
	{
		if (!Initialize())
			NazaraError("Failed to initialize HardwareInfo");

		return s_brandString;
	}

	/*!
	* \brief Gets the number of threads
	* \return Number of threads available on the CPU
	*
	* \remark Doesn't need the initialization of HardwareInfo
	*/

	unsigned int HardwareInfo::GetProcessorCount()
	{
		static unsigned int processorCount = std::max(HardwareInfoImpl::GetProcessorCount(), 1U);
		return processorCount;
	}

	/*!
	* \brief Gets the processor vendor
	* \return ProcessorVendor containing information the vendor
	*
	* \remark Produces a NazaraError if not Initialize
	*/

	ProcessorVendor HardwareInfo::GetProcessorVendor()
	{
		if (!Initialize())
			NazaraError("Failed to initialize HardwareInfo");

		return s_vendorEnum;
	}

	/*!
	* \brief Gets the vendor of the processor
	* \return String of the vendor
	*
	* \remark Produces a NazaraError if not Initialize
	*/

	String HardwareInfo::GetProcessorVendorName()
	{
		if (!Initialize())
			NazaraError("Failed to initialize HardwareInfo");

		return vendorNames[s_vendorEnum+1];
	}

	/*!
	* \brief Gets the amount of total memory
	* \return Number of total memory available
	*
	* \remark Doesn't need the initialization of HardwareInfo
	*/

	UInt64 HardwareInfo::GetTotalMemory()
	{
		static UInt64 totalMemory = HardwareInfoImpl::GetTotalMemory();
		return totalMemory;
	}

	/*!
	* \brief Checks whether the processor owns the capacity to handle certain instructions
	* \return true If instructions supported
	*
	* \remark Produces a NazaraError if capability is a wrong enum with NAZARA_DEBUG defined
	*/

	bool HardwareInfo::HasCapability(ProcessorCap capability)
	{
		#ifdef NAZARA_DEBUG
		if (capability > ProcessorCap_Max)
		{
			NazaraError("Capability type out of enum");
			return false;
		}
		#endif

		return s_capabilities[capability];
	}

	/*!
	* \brief Initializes the HardwareInfo class
	* \return true if successful
	*
	* \remark Produces a NazaraError if cpuid is not supported
	*/

	bool HardwareInfo::Initialize()
	{
		if (IsInitialized())
			return true;

		if (!HardwareInfoImpl::IsCpuidSupported())
		{
			NazaraError("Cpuid is not supported");
			return false;
		}

		s_initialized = true;

		UInt32 registers[4]; // To store our registers values (EAX, EBX, ECX and EDX)

		// Let's make it clearer
		UInt32& eax = registers[0];
		UInt32& ebx = registers[1];
		UInt32& ecx = registers[2];
		UInt32& edx = registers[3];

		// To begin, we get the id of the constructor and the id of maximal functions supported by the CPUID
		HardwareInfoImpl::Cpuid(0, 0, registers);

		// Note the order: EBX, EDX, ECX
		UInt32 manufacturerId[3] = {ebx, edx, ecx};

		// Identification of conceptor
		s_vendorEnum = ProcessorVendor_Unknown;
		for (const VendorString& vendorString : vendorStrings)
		{
			if (std::memcmp(manufacturerId, vendorString.vendor, 12) == 0)
			{
				s_vendorEnum = vendorString.vendorEnum;
				break;
			}
		}

		if (eax >= 1)
		{
			// Retrieval of certain capacities of the processor (ECX et EDX, function 1)
			HardwareInfoImpl::Cpuid(1, 0, registers);

			s_capabilities[ProcessorCap_AVX]   = (ecx & (1U << 28)) != 0;
			s_capabilities[ProcessorCap_FMA3]  = (ecx & (1U << 12)) != 0;
			s_capabilities[ProcessorCap_MMX]   = (edx & (1U << 23)) != 0;
			s_capabilities[ProcessorCap_SSE]   = (edx & (1U << 25)) != 0;
			s_capabilities[ProcessorCap_SSE2]  = (edx & (1U << 26)) != 0;
			s_capabilities[ProcessorCap_SSE3]  = (ecx & (1U <<  0)) != 0;
			s_capabilities[ProcessorCap_SSSE3] = (ecx & (1U <<  9)) != 0;
			s_capabilities[ProcessorCap_SSE41] = (ecx & (1U << 19)) != 0;
			s_capabilities[ProcessorCap_SSE42] = (ecx & (1U << 20)) != 0;
		}

		// Retrieval of biggest extended function handled (EAX, function 0x80000000)
		HardwareInfoImpl::Cpuid(0x80000000, 0, registers);

		UInt32 maxSupportedExtendedFunction = eax;
		if (maxSupportedExtendedFunction >= 0x80000001)
		{
			// Retrieval of extended capabilities of the processor (ECX and EDX, function 0x80000001)
			HardwareInfoImpl::Cpuid(0x80000001, 0, registers);

			s_capabilities[ProcessorCap_x64]   = (edx & (1U << 29)) != 0; // Support of 64bits, independent of the OS
			s_capabilities[ProcessorCap_FMA4]  = (ecx & (1U << 16)) != 0;
			s_capabilities[ProcessorCap_SSE4a] = (ecx & (1U <<  6)) != 0;
			s_capabilities[ProcessorCap_XOP]   = (ecx & (1U << 11)) != 0;

			if (maxSupportedExtendedFunction >= 0x80000004)
			{
				// Retrieval of the string describing the processor (EAX, EBX, ECX and EDX,
				// functions from 0x80000002 to 0x80000004 inclusive)
				char* ptr = &s_brandString[0];
				for (UInt32 code = 0x80000002; code <= 0x80000004; ++code)
				{
					HardwareInfoImpl::Cpuid(code, 0, registers);
					std::memcpy(ptr, &registers[0], 4*sizeof(UInt32)); // We add the 16 bytes to the string

					ptr += 4*sizeof(UInt32);
				}

				// The character '\0' is already part of the string
			}
		}

		return true;
	}

	/*!
	* \brief Checks whether the instruction of cpuid is supported
	* \return true if it the case
	*/

	bool HardwareInfo::IsCpuidSupported()
	{
		return HardwareInfoImpl::IsCpuidSupported();
	}

	/*!
	* \brief Checks whether the class HardwareInfo is initialized
	* \return true if it is initialized
	*/

	bool HardwareInfo::IsInitialized()
	{
		return s_initialized;
	}

	/*!
	* \brief Unitializes the class HardwareInfo
	*/

	void HardwareInfo::Uninitialize()
	{
		// Nothing to do
		s_initialized = false;
	}
}
