// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline const char* HardwareInfo::GetCpuBrandString() const
	{
		return m_cpuBrandString.data();
	}

	inline unsigned int HardwareInfo::GetCpuThreadCount() const
	{
		return m_cpuThreadCount;
	}

	inline ProcessorVendor HardwareInfo::GetCpuVendor() const
	{
		return m_cpuVendor;
	}

	inline UInt64 HardwareInfo::GetSystemTotalMemory() const
	{
		return m_systemTotalMemory;
	}

	inline bool HardwareInfo::HasCapability(ProcessorCap capability) const
	{
		return m_cpuCapabilities[UnderlyingCast(capability)];
	}
}

#include <Nazara/Core/DebugOff.hpp>
