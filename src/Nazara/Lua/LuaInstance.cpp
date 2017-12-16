// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaInstance.hpp>
#include <Lua/lauxlib.h>
#include <Lua/lua.h>
#include <Lua/lualib.h>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	namespace
	{
		int AtPanic(lua_State* internalState)
		{
			String lastError(lua_tostring(internalState, -1));

			throw std::runtime_error("Lua panic: " + lastError.ToStdString());
		}
	}

	LuaInstance::LuaInstance() :
	LuaState(nullptr),
	m_memoryLimit(0),
	m_memoryUsage(0),
	m_timeLimit(1000),
	m_level(0)
	{
		m_state = lua_newstate(MemoryAllocator, this);
		lua_atpanic(m_state, AtPanic);
		lua_sethook(m_state, TimeLimiter, LUA_MASKCOUNT, 1000);
		luaL_openlibs(m_state);
	}

	LuaInstance::LuaInstance(LuaInstance&& instance) :
	LuaState(std::move(instance)),
	m_memoryLimit(std::move(m_memoryLimit)),
	m_memoryUsage(std::move(m_memoryUsage)),
	m_timeLimit(std::move(m_timeLimit)),
	m_clock(std::move(m_clock)),
	m_level(std::move(m_level))
	{
		lua_setallocf(m_state, MemoryAllocator, this);
	}

	LuaInstance::~LuaInstance()
	{
		if (m_state)
			lua_close(m_state);
	}

	LuaInstance& LuaInstance::operator=(LuaInstance&& instance)
	{
		LuaState::operator=(std::move(instance));

		m_memoryLimit = std::move(instance.m_memoryLimit);
		m_memoryUsage = std::move(instance.m_memoryUsage);
		m_timeLimit = std::move(instance.m_timeLimit);
		m_clock = std::move(instance.m_clock);
		m_level = std::move(instance.m_level);

		lua_setallocf(m_state, MemoryAllocator, this);

		return *this;
	}

	void* LuaInstance::MemoryAllocator(void* ud, void* ptr, std::size_t osize, std::size_t nsize)
	{
		LuaInstance* instance = static_cast<LuaInstance*>(ud);
		std::size_t memoryLimit = instance->GetMemoryLimit();
		std::size_t memoryUsage = instance->GetMemoryUsage();

		if (nsize == 0)
		{
			assert(memoryUsage >= osize);

			instance->SetMemoryUsage(memoryUsage - osize);
			std::free(ptr);

			return nullptr;
		}
		else
		{
			std::size_t usage = memoryUsage + nsize;
			if (ptr)
				usage -= osize;

			if (memoryLimit != 0 && usage > memoryLimit)
			{
				NazaraError("Lua memory usage is over memory limit (" + String::Number(usage) + " > " + String::Number(memoryLimit) + ')');
				return nullptr;
			}

			instance->SetMemoryUsage(usage);

			return std::realloc(ptr, nsize);
		}
	}

	void LuaInstance::TimeLimiter(lua_State* internalState, lua_Debug* debug)
	{
		NazaraUnused(debug);

		LuaInstance* instance;
		lua_getallocf(internalState, reinterpret_cast<void**>(&instance));

		if (instance->m_clock.GetMilliseconds() > instance->GetTimeLimit())
			luaL_error(internalState, "maximum execution time exceeded");
	}
}
