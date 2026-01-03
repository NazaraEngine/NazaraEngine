// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <algorithm>
#include <cstring>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/HardwareInfoImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/HardwareInfoImpl.hpp>
#else
	#error Lack of implementation: HardwareInfo
#endif


namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr EnumArray<ProcessorVendor, std::string_view> s_vendorNames {
			"ACRN",                                // ProcessorVendor::ACRN
			"Advanced Micro Devices",              // ProcessorVendor::AMD
			"ao486",                               // ProcessorVendor::Ao486
			"Apple Rosetta 2",                     // ProcessorVendor::AppleRosetta2
			"bhyve",                               // ProcessorVendor::Bhyve
			"Centaur Technology",                  // ProcessorVendor::Centaur
			"Cyrix Corporation",                   // ProcessorVendor::Cyrix
			"MCST Elbrus",                         // ProcessorVendor::Elbrus
			"Hygon",                               // ProcessorVendor::Hygon
			"Microsoft Hyper-V",                   // ProcessorVendor::HyperV
			"Intel Corporation",                   // ProcessorVendor::Intel
			"Kernel-based Virtual Machine",        // ProcessorVendor::KVM
			"Microsoft x86-to-ARM",                // ProcessorVendor::MicrosoftXTA
			"National Semiconductor",              // ProcessorVendor::NSC
			"NexGen",                              // ProcessorVendor::NexGen
			"Parallels",                           // ProcessorVendor::Parallels
			"QEMU",                                // ProcessorVendor::QEMU
			"QNX Hypervisor",                      // ProcessorVendor::QNX
			"Rise Technology",                     // ProcessorVendor::Rise
			"Silicon Integrated Systems",          // ProcessorVendor::SiS
			"Transmeta Corporation",               // ProcessorVendor::Transmeta
			"United Microelectronics Corporation", // ProcessorVendor::UMC
			"VIA Technologies",                    // ProcessorVendor::VIA
			"VMware",                              // ProcessorVendor::VMware
			"Vortex86",                            // ProcessorVendor::Vortex
			"Xen",                                 // ProcessorVendor::XenHVM
			"Zhaoxin"                              // ProcessorVendor::Zhaoxin
		};

		static_assert(s_vendorNames.size() == ProcessorVendorCount, "Processor vendor name array is incomplete");

		struct VendorString
		{
			char vendor[13]; // +1 for the end of string character
			ProcessorVendor vendorEnum;
		};

		constexpr frozen::unordered_map s_vendorStrings = frozen::make_unordered_map<frozen::string, ProcessorVendor>({
			{ "  Shanghai  ", ProcessorVendor::Zhaoxin       },
			{ " KVMKVMKVM  ", ProcessorVendor::KVM           },
			{ " QNXQVMBSQG ", ProcessorVendor::QNX           },
			{ " lrpepyh  vr", ProcessorVendor::Parallels     },
			{ "ACRNACRNACRN", ProcessorVendor::ACRN          },
			{ "AMDisbetter!", ProcessorVendor::AMD           },
			{ "AuthenticAMD", ProcessorVendor::AMD           },
			{ "CentaurHauls", ProcessorVendor::Centaur       },
			{ "CyrixInstead", ProcessorVendor::Cyrix         },
			{ "E2K MACHINE",  ProcessorVendor::Elbrus        },
			{ "GenuineIntel", ProcessorVendor::Intel         },
			{ "GenuineTMx86", ProcessorVendor::Transmeta     },
			{ "Geode by NSC", ProcessorVendor::NSC           },
			{ "HygonGenuine", ProcessorVendor::Hygon         },
			{ "KVMKVMKVMKVM", ProcessorVendor::KVM           },
			{ "Microsoft Hv", ProcessorVendor::HyperV        },
			{ "MicrosoftXTA", ProcessorVendor::MicrosoftXTA  },
			{ "NexGenDriven", ProcessorVendor::NexGen        },
			{ "RiseRiseRise", ProcessorVendor::Rise          },
			{ "SiS SiS SiS ", ProcessorVendor::SIS           },
			{ "TCGTCGTCGTCG", ProcessorVendor::QEMU          },
			{ "TransmetaCPU", ProcessorVendor::Transmeta     },
			{ "UMC UMC UMC ", ProcessorVendor::UMC           },
			{ "VIA VIA VIA ", ProcessorVendor::VIA           },
			{ "VMwareVMware", ProcessorVendor::VMware        },
			{ "VirtualApple", ProcessorVendor::AppleRosetta2 },
			{ "Vortex86 SoC", ProcessorVendor::Vortex        },
			{ "XenVMMXenVMM", ProcessorVendor::XenHVM        },
			{ "bhyve bhyve ", ProcessorVendor::Bhyve         },
			{ "prl hyperv ",  ProcessorVendor::Parallels     }, //< endianness-fixed version of " lrpepyh vr"
		});
	}

	/*!
	* \ingroup core
	* \class Nz::HardwareInfo
	* \brief Core class that represents the info we can get from hardware
	*/

	HardwareInfo::HardwareInfo()
	{
		FetchCPUInfo();
		FetchMemoryInfo();
	}

	std::string_view HardwareInfo::GetCpuVendorName() const
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return (m_cpuVendor != ProcessorVendor::Unknown) ? s_vendorNames[m_cpuVendor] : "Unknown";
	}

	void HardwareInfo::Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 result[4])
	{
		return PlatformImpl::HardwareInfoImpl::Cpuid(functionId, subFunctionId, result);
	}

	bool HardwareInfo::IsCpuidSupported()
	{
		return PlatformImpl::HardwareInfoImpl::IsCpuidSupported();
	}

	void HardwareInfo::FetchCPUInfo()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		m_cpuThreadCount = std::max(PlatformImpl::HardwareInfoImpl::GetProcessorCount(), 1U);

		m_cpuCapabilities.fill(false);
		m_cpuVendor = ProcessorVendor::Unknown;

		std::strcpy(m_cpuBrandString.data(), "CPU from unknown vendor - cpuid not supported");

		if (!PlatformImpl::HardwareInfoImpl::IsCpuidSupported())
			return;

		// To begin, we get the id of the constructor and the id of maximal functions supported by the CPUID
		std::array<UInt32, 4> registers;
		PlatformImpl::HardwareInfoImpl::Cpuid(0, 0, registers.data());

		UInt32& eax = registers[0];
		UInt32& ebx = registers[1];
		UInt32& ecx = registers[2];
		UInt32& edx = registers[3];

		UInt32 manufacturerId[3] = { ebx, edx, ecx };
		frozen::string manufactorID(reinterpret_cast<const char*>(&manufacturerId[0]), 12);

		if (auto vendorIt = s_vendorStrings.find(manufactorID); vendorIt != s_vendorStrings.end())
			m_cpuVendor = vendorIt->second;
		else
			m_cpuVendor = ProcessorVendor::Unknown;

		if (eax >= 1)
		{
			// Retrieval of certain capacities of the processor (ECX and EDX, function 1)
			PlatformImpl::HardwareInfoImpl::Cpuid(1, 0, registers.data());

			m_cpuCapabilities[ProcessorCap::AES]    = (ecx & (1U << 25)) != 0;
			m_cpuCapabilities[ProcessorCap::AVX]    = (ecx & (1U << 28)) != 0;
			m_cpuCapabilities[ProcessorCap::FMA3]   = (ecx & (1U << 12)) != 0;
			m_cpuCapabilities[ProcessorCap::MMX]    = (edx & (1U << 23)) != 0;
			m_cpuCapabilities[ProcessorCap::Popcnt] = (ecx & (1U << 23)) != 0;
			m_cpuCapabilities[ProcessorCap::RDRAND] = (ecx & (1U << 30)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE]    = (edx & (1U << 25)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE2]   = (edx & (1U << 26)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE3]   = (ecx & (1U << 0)) != 0;
			m_cpuCapabilities[ProcessorCap::SSSE3]  = (ecx & (1U << 9)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE41]  = (ecx & (1U << 19)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE42]  = (ecx & (1U << 20)) != 0;
		}

		// Retrieval of biggest extended function handled (EAX, function 0x80000000)
		PlatformImpl::HardwareInfoImpl::Cpuid(0x80000000, 0, registers.data());

		UInt32 maxSupportedExtendedFunction = eax;
		if (maxSupportedExtendedFunction >= 0x80000001)
		{
			// Retrieval of extended capabilities of the processor (ECX and EDX, function 0x80000001)
			PlatformImpl::HardwareInfoImpl::Cpuid(0x80000001, 0, registers.data());

			m_cpuCapabilities[ProcessorCap::x64]   = (edx & (1U << 29)) != 0; // Support of 64bits, doesn't mean executable is 64bits
			m_cpuCapabilities[ProcessorCap::FMA4]  = (ecx & (1U << 16)) != 0;
			m_cpuCapabilities[ProcessorCap::SSE4a] = (ecx & (1U << 6)) != 0;
			m_cpuCapabilities[ProcessorCap::XOP]   = (ecx & (1U << 11)) != 0;

			if (maxSupportedExtendedFunction >= 0x80000004)
			{
				// Retrieval of the string describing the processor (EAX, EBX, ECX and EDX, functions from 0x80000002 to 0x80000004 inclusive)
				char* ptr = &m_cpuBrandString[0];
				for (UInt32 code = 0x80000002; code <= 0x80000004; ++code)
				{
					PlatformImpl::HardwareInfoImpl::Cpuid(code, 0, registers.data());
					std::memcpy(ptr, &registers[0], 4*sizeof(UInt32)); // We add the 16 bytes to the string

					ptr += 4 * sizeof(UInt32);
				}

				// The character '\0' is already part of the string
			}
		}
	}

	void HardwareInfo::FetchMemoryInfo()
	{
		m_systemTotalMemory = PlatformImpl::HardwareInfoImpl::GetTotalMemory();
	}
}
