// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Error.hpp>
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

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr std::array s_vendorNames = {
			std::string_view("ACRN"),                                // ProcessorVendor::ACRN
			std::string_view("Advanced Micro Devices"),              // ProcessorVendor::AMD
			std::string_view("ao486"),                               // ProcessorVendor::Ao486
			std::string_view("Apple Rosetta 2"),                     // ProcessorVendor::AppleRosetta2
			std::string_view("bhyve"),                               // ProcessorVendor::Bhyve
			std::string_view("Centaur Technology"),                  // ProcessorVendor::Centaur
			std::string_view("Cyrix Corporation"),                   // ProcessorVendor::Cyrix
			std::string_view("MCST Elbrus"),                         // ProcessorVendor::Elbrus
			std::string_view("Hygon"),                               // ProcessorVendor::Hygon
			std::string_view("Microsoft Hyper-V"),                   // ProcessorVendor::HyperV
			std::string_view("Intel Corporation"),                   // ProcessorVendor::Intel
			std::string_view("Kernel-based Virtual Machine"),        // ProcessorVendor::KVM
			std::string_view("Microsoft x86-to-ARM"),                // ProcessorVendor::MicrosoftXTA
			std::string_view("National Semiconductor"),              // ProcessorVendor::NSC
			std::string_view("NexGen"),                              // ProcessorVendor::NexGen
			std::string_view("Parallels"),                           // ProcessorVendor::Parallels
			std::string_view("QEMU"),                                // ProcessorVendor::QEMU
			std::string_view("QNX Hypervisor"),                      // ProcessorVendor::QNX
			std::string_view("Rise Technology"),                     // ProcessorVendor::Rise
			std::string_view("Silicon Integrated Systems"),          // ProcessorVendor::SiS
			std::string_view("Transmeta Corporation"),               // ProcessorVendor::Transmeta
			std::string_view("United Microelectronics Corporation"), // ProcessorVendor::UMC
			std::string_view("VIA Technologies"),                    // ProcessorVendor::VIA
			std::string_view("VMware"),                              // ProcessorVendor::VMware
			std::string_view("Vortex86"),                            // ProcessorVendor::Vortex
			std::string_view("Xen"),                                 // ProcessorVendor::XenHVM
			std::string_view("Zhaoxin)")                             // ProcessorVendor::Zhaoxin
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

		return s_vendorNames[UnderlyingCast(m_cpuVendor)];
	}

	void HardwareInfo::Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 result[4])
	{
		return HardwareInfoImpl::Cpuid(functionId, subFunctionId, result);
	}

	bool HardwareInfo::IsCpuidSupported()
	{
		return HardwareInfoImpl::IsCpuidSupported();
	}

	void HardwareInfo::FetchCPUInfo()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		m_cpuThreadCount = std::max(HardwareInfoImpl::GetProcessorCount(), 1U);

		m_cpuCapabilities.fill(false);
		m_cpuVendor = ProcessorVendor::Unknown;

		std::strcpy(m_cpuBrandString.data(), "CPU from unknown vendor - cpuid not supported");

		if (!HardwareInfoImpl::IsCpuidSupported())
		{
			NazaraWarning("Cpuid is not supported");
			return;
		}

		// To begin, we get the id of the constructor and the id of maximal functions supported by the CPUID
		std::array<UInt32, 4> registers;
		HardwareInfoImpl::Cpuid(0, 0, registers.data());

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
			HardwareInfoImpl::Cpuid(1, 0, registers.data());

			m_cpuCapabilities[UnderlyingCast(ProcessorCap::AES)]    = (ecx & (1U << 25)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::AVX)]    = (ecx & (1U << 28)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::FMA3)]   = (ecx & (1U << 12)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::MMX)]    = (edx & (1U << 23)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::Popcnt)] = (ecx & (1U << 23)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::RDRAND)] = (ecx & (1U << 30)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE)]    = (edx & (1U << 25)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE2)]   = (edx & (1U << 26)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE3)]   = (ecx & (1U << 0)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSSE3)]  = (ecx & (1U << 9)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE41)]  = (ecx & (1U << 19)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE42)]  = (ecx & (1U << 20)) != 0;
		}

		// Retrieval of biggest extended function handled (EAX, function 0x80000000)
		HardwareInfoImpl::Cpuid(0x80000000, 0, registers.data());

		UInt32 maxSupportedExtendedFunction = eax;
		if (maxSupportedExtendedFunction >= 0x80000001)
		{
			// Retrieval of extended capabilities of the processor (ECX and EDX, function 0x80000001)
			HardwareInfoImpl::Cpuid(0x80000001, 0, registers.data());

			m_cpuCapabilities[UnderlyingCast(ProcessorCap::x64)]   = (edx & (1U << 29)) != 0; // Support of 64bits, doesn't mean executable is 64bits
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::FMA4)]  = (ecx & (1U << 16)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::SSE4a)] = (ecx & (1U << 6)) != 0;
			m_cpuCapabilities[UnderlyingCast(ProcessorCap::XOP)]   = (ecx & (1U << 11)) != 0;

			if (maxSupportedExtendedFunction >= 0x80000004)
			{
				// Retrieval of the string describing the processor (EAX, EBX, ECX and EDX, functions from 0x80000002 to 0x80000004 inclusive)
				char* ptr = &m_cpuBrandString[0];
				for (UInt32 code = 0x80000002; code <= 0x80000004; ++code)
				{
					HardwareInfoImpl::Cpuid(code, 0, registers.data());
					std::memcpy(ptr, &registers[0], 4*sizeof(UInt32)); // We add the 16 bytes to the string

					ptr += 4 * sizeof(UInt32);
				}

				// The character '\0' is already part of the string
			}
		}
	}

	void HardwareInfo::FetchMemoryInfo()
	{
		m_systemTotalMemory = HardwareInfoImpl::GetTotalMemory();
	}
}
