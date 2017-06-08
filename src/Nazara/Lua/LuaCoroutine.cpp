// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaCoroutine.hpp>
#include <Lua/lauxlib.h>
#include <Lua/lua.h>
#include <Lua/lualib.h>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	LuaCoroutine::LuaCoroutine(lua_State* internalState, int refIndex) :
	LuaState(internalState),
	m_ref(refIndex)
	{
	}

	LuaCoroutine::~LuaCoroutine()
	{
		if (m_ref >= 0)
			DestroyReference(m_ref);
	}

	bool LuaCoroutine::CanResume() const
	{
		return lua_status(m_state) == LUA_YIELD;
	}

	Ternary LuaCoroutine::Resume(unsigned int argCount)
	{
		LuaInstance& instance = GetInstance(m_state);
		if (instance.m_level++ == 0)
			instance.m_clock.Restart();

		int status = lua_resume(m_state, nullptr, argCount);
		instance.m_level--;

		if (status == LUA_OK)
			return Ternary_True;
		else if (status == LUA_YIELD)
			return Ternary_Unknown;
		else
		{
			m_lastError = ToString(-1);
			Pop();
			return Ternary_False;
		}
	}

	bool LuaCoroutine::Run(int argCount, int /*resultCount*/)
	{
		return Resume(argCount) != Ternary_False;
	}
}
