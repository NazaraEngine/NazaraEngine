// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
		return m_cpuCapabilities[capability];
	}
}

