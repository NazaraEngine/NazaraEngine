// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HARDWAREINFO_HPP
#define NAZARA_CORE_HARDWAREINFO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <string>

namespace Nz
{
	class NAZARA_CORE_API HardwareInfo
	{
		public:
			HardwareInfo() = delete;
			~HardwareInfo() = delete;

			static void Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 result[4]);

			static std::string_view GetProcessorBrandString();
			static unsigned int GetProcessorCount();
			static ProcessorVendor GetProcessorVendor();
			static std::string_view GetProcessorVendorName();
			static UInt64 GetTotalMemory();

			static bool HasCapability(ProcessorCap capability);

			static bool Initialize();

			static bool IsCpuidSupported();
			static bool IsInitialized();

			static void Uninitialize();
	};
}

#endif // NAZARA_CORE_HARDWAREINFO_HPP
