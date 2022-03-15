// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
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
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		struct VendorString
		{
			char vendor[13]; // +1 pour le \0 automatiquement ajouté par le compilateur
			ProcessorVendor vendorEnum;
		};

		const char* s_vendorNames[] =
		{
			"Advanced Micro Devices",              // ProcessorVendor::AMD
			"Centaur Technology",                  // ProcessorVendor::Centaur
			"Cyrix Corporation",                   // ProcessorVendor::Cyrix
			"Intel Corporation",                   // ProcessorVendor::Intel
			"Kernel-based Virtual Machine",        // ProcessorVendor::KVM
			"Microsoft Hyper-V",                   // ProcessorVendor::HyperV
			"National Semiconductor",              // ProcessorVendor::NSC
			"NexGen",                              // ProcessorVendor::NexGen
			"Rise Technology",                     // ProcessorVendor::Rise
			"Silicon Integrated Systems",          // ProcessorVendor::SIS
			"Transmeta Corporation",               // ProcessorVendor::Transmeta
			"United Microelectronics Corporation", // ProcessorVendor::UMC
			"VIA Technologies",                    // ProcessorVendor::VIA
			"VMware",                              // ProcessorVendor::VMware
			"Vortex86",                            // ProcessorVendor::Vortex
			"Xen"                                  // ProcessorVendor::XenHVM
		};

		static_assert(sizeof(s_vendorNames)/sizeof(const char*) == ProcessorVendorCount, "Processor vendor name array is incomplete");

		VendorString vendorStrings[] =
		{
			// Triés par ordre alphabétique (Majuscules primant sur minuscules)
			{"AMDisbetter!", ProcessorVendor::AMD},
			{"AuthenticAMD", ProcessorVendor::AMD},
			{"CentaurHauls", ProcessorVendor::Centaur},
			{"CyrixInstead", ProcessorVendor::Cyrix},
			{"GenuineIntel", ProcessorVendor::Intel},
			{"GenuineTMx86", ProcessorVendor::Transmeta},
			{"Geode by NSC", ProcessorVendor::NSC},
			{"KVMKVMKVMKVM", ProcessorVendor::KVM},
			{"Microsoft Hv", ProcessorVendor::HyperV},
			{"NexGenDriven", ProcessorVendor::NexGen},
			{"RiseRiseRise", ProcessorVendor::Rise},
			{"SiS SiS SiS ", ProcessorVendor::SIS},
			{"TransmetaCPU", ProcessorVendor::Transmeta},
			{"UMC UMC UMC ", ProcessorVendor::UMC},
			{"VIA VIA VIA ", ProcessorVendor::VIA},
			{"VMwareVMware", ProcessorVendor::VMware},
			{"Vortex86 SoC", ProcessorVendor::Vortex},
			{"XenVMMXenVMM", ProcessorVendor::XenHVM}
		};

		ProcessorVendor s_vendorEnum = ProcessorVendor::Unknown;
		bool s_capabilities[ProcessorCapCount] = {false};
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

	std::string_view HardwareInfo::GetProcessorBrandString()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

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
		NAZARA_USE_ANONYMOUS_NAMESPACE

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

	std::string_view HardwareInfo::GetProcessorVendorName()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!Initialize())
			NazaraError("Failed to initialize HardwareInfo");

		return s_vendorNames[UnderlyingCast(s_vendorEnum)];
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
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return s_capabilities[UnderlyingCast(capability)];
	}

	/*!
	* \brief Initializes the HardwareInfo class
	* \return true if successful
	*
	* \remark Produces a NazaraError if cpuid is not supported
	*/

	bool HardwareInfo::Initialize()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

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
		s_vendorEnum = ProcessorVendor::Unknown;
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
			// Retrieval of certain capacities of the processor (ECX and EDX, function 1)
			HardwareInfoImpl::Cpuid(1, 0, registers);

			s_capabilities[UnderlyingCast(ProcessorCap::AVX)]   = (ecx & (1U << 28)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::FMA3)]  = (ecx & (1U << 12)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::MMX)]   = (edx & (1U << 23)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE)]   = (edx & (1U << 25)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE2)]  = (edx & (1U << 26)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE3)]  = (ecx & (1U << 0)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSSE3)] = (ecx & (1U << 9)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE41)] = (ecx & (1U << 19)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE42)] = (ecx & (1U << 20)) != 0;
		}

		// Retrieval of biggest extended function handled (EAX, function 0x80000000)
		HardwareInfoImpl::Cpuid(0x80000000, 0, registers);

		UInt32 maxSupportedExtendedFunction = eax;
		if (maxSupportedExtendedFunction >= 0x80000001)
		{
			// Retrieval of extended capabilities of the processor (ECX and EDX, function 0x80000001)
			HardwareInfoImpl::Cpuid(0x80000001, 0, registers);

			s_capabilities[UnderlyingCast(ProcessorCap::x64)]   = (edx & (1U << 29)) != 0; // Support of 64bits, independent of the OS
			s_capabilities[UnderlyingCast(ProcessorCap::FMA4)]  = (ecx & (1U << 16)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::SSE4a)] = (ecx & (1U << 6)) != 0;
			s_capabilities[UnderlyingCast(ProcessorCap::XOP)]   = (ecx & (1U << 11)) != 0;

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
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return s_initialized;
	}

	/*!
	* \brief Unitializes the class HardwareInfo
	*/

	void HardwareInfo::Uninitialize()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		// Nothing to do
		s_initialized = false;
	}
}
