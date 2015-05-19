// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREINFO_HPP
#define NAZARA_HARDWAREINFO_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

class NAZARA_API NzHardwareInfo
{
	public:
		static void Cpuid(nzUInt32 functionId, nzUInt32 subFunctionId, nzUInt32 result[4]);

		static NzString GetProcessorBrandString();
		static unsigned int GetProcessorCount();
		static nzProcessorVendor GetProcessorVendor();
		static NzString GetProcessorVendorName();
		static nzUInt64 GetTotalMemory();

		static bool HasCapability(nzProcessorCap capability);

		static bool Initialize();

		static bool IsCpuidSupported();
		static bool IsInitialized();

		static void Uninitialize();
};

#endif // NAZARA_HARDWAREINFO_HPP
