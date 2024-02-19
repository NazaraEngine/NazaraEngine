// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HARDWAREINFO_HPP
#define NAZARA_CORE_HARDWAREINFO_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <array>
#include <string_view>

namespace Nz
{
	class NAZARA_CORE_API HardwareInfo
	{
		public:
			HardwareInfo();
			HardwareInfo(const HardwareInfo&) = delete;
			HardwareInfo(HardwareInfo&&) = delete;
			~HardwareInfo() = default;

			inline const char* GetCpuBrandString() const;
			inline unsigned int GetCpuThreadCount() const;
			inline ProcessorVendor GetCpuVendor() const;
			std::string_view GetCpuVendorName() const;
			inline UInt64 GetSystemTotalMemory() const;

			inline bool HasCapability(ProcessorCap capability) const;

			static void Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 result[4]);
			static bool IsCpuidSupported();

			HardwareInfo& operator=(const HardwareInfo&) = delete;
			HardwareInfo& operator=(HardwareInfo&&) = delete;

		private:
			void FetchCPUInfo();
			void FetchMemoryInfo();

			std::array<char, 3 * 4 * 4> m_cpuBrandString;
			EnumArray<ProcessorCap, bool> m_cpuCapabilities;
			ProcessorVendor m_cpuVendor;
			unsigned int m_cpuThreadCount;
			UInt64 m_systemTotalMemory;
	};
}

#include <Nazara/Core/HardwareInfo.inl>

#endif // NAZARA_CORE_HARDWAREINFO_HPP
