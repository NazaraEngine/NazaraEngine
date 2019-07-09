// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaInstance.hpp>
#include <Lua/lauxlib.h>
#include <Lua/lua.h>
#include <Lua/lualib.h>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <array>
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
	}

	LuaInstance::LuaInstance(LuaInstance&& instance) :
	LuaState(std::move(instance))
	{
		std::swap(m_memoryLimit, instance.m_memoryLimit);
		std::swap(m_memoryUsage, instance.m_memoryUsage);
		std::swap(m_timeLimit, instance.m_timeLimit);
		std::swap(m_clock, instance.m_clock);
		std::swap(m_level, instance.m_level);

		if (m_state)
			lua_setallocf(m_state, MemoryAllocator, this);

		if (instance.m_state)
			lua_setallocf(instance.m_state, MemoryAllocator, &instance);
	}

	LuaInstance::~LuaInstance()
	{
		if (m_state)
			lua_close(m_state);
	}

	void LuaInstance::LoadLibraries(LuaLibFlags libFlags)
	{
		// From luaL_openlibs
		std::array<luaL_Reg, LuaLib_Max + 1> libs;
		std::size_t libCount = 0;

		libs[libCount++] = { "_G", luaopen_base };

		if (libFlags & LuaLib_Coroutine)
			libs[libCount++] = { LUA_COLIBNAME, luaopen_coroutine };

		if (libFlags & LuaLib_Debug)
			libs[libCount++] = { LUA_DBLIBNAME, luaopen_debug };

		if (libFlags & LuaLib_Io)
			libs[libCount++] = { LUA_IOLIBNAME, luaopen_io };

		if (libFlags & LuaLib_Math)
			libs[libCount++] = { LUA_MATHLIBNAME, luaopen_math };

		if (libFlags & LuaLib_Os)
			libs[libCount++] = { LUA_OSLIBNAME, luaopen_os };

		if (libFlags & LuaLib_Package)
			libs[libCount++] = { LUA_LOADLIBNAME, luaopen_package };

		if (libFlags & LuaLib_String)
			libs[libCount++] = { LUA_STRLIBNAME, luaopen_string };

		if (libFlags & LuaLib_Table)
			libs[libCount++] = { LUA_TABLIBNAME, luaopen_table };

		if (libFlags & LuaLib_Utf8)
			libs[libCount++] = { LUA_UTF8LIBNAME, luaopen_utf8 };

		for (std::size_t i = 0; i < libCount; ++i)
		{
			luaL_requiref(m_state, libs[i].name, libs[i].func, 1);
			lua_pop(m_state, 1);  /* remove lib */
		}
	}

	LuaInstance& LuaInstance::operator=(LuaInstance&& instance)
	{
		LuaState::operator=(std::move(instance));

		std::swap(m_memoryLimit, instance.m_memoryLimit);
		std::swap(m_memoryUsage, instance.m_memoryUsage);
		std::swap(m_timeLimit, instance.m_timeLimit);
		std::swap(m_clock, instance.m_clock);
		std::swap(m_level, instance.m_level);

		if (m_state)
			lua_setallocf(m_state, MemoryAllocator, this);

		if (instance.m_state)
			lua_setallocf(instance.m_state, MemoryAllocator, &instance);

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
