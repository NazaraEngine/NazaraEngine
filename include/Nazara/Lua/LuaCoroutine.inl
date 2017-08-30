// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaCoroutine.hpp>

namespace Nz
{
	inline LuaCoroutine::LuaCoroutine(LuaCoroutine&& instance) :
	LuaState(std::move(instance)),
	m_ref(instance.m_ref)
	{
		instance.m_ref = -1;
	}

	inline LuaCoroutine& LuaCoroutine::operator=(LuaCoroutine&& instance)
	{
		LuaState::operator=(std::move(instance));

		m_ref = instance.m_ref;
		instance.m_ref = -1;

		return *this;
	}
}
