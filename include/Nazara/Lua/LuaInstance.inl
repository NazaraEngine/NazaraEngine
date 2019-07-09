// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	inline std::size_t LuaInstance::GetMemoryLimit() const
	{
		return m_memoryLimit;
	}

	inline std::size_t LuaInstance::GetMemoryUsage() const
	{
		return m_memoryUsage;
	}

	inline UInt32 LuaInstance::GetTimeLimit() const
	{
		return m_timeLimit;
	}

	inline void LuaInstance::SetMemoryLimit(std::size_t memoryLimit)
	{
		m_memoryLimit = memoryLimit;
	}

	inline void LuaInstance::SetTimeLimit(UInt32 limit)
	{
		m_timeLimit = limit;
	}

	inline void LuaInstance::SetMemoryUsage(std::size_t memoryUsage)
	{
		m_memoryUsage = memoryUsage;
	}
}

#include <Nazara/Lua/DebugOff.hpp>
