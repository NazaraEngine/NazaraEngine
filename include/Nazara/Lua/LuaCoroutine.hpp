// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUACOROUTINE_HPP
#define NAZARA_LUACOROUTINE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Lua/LuaState.hpp>
#include <cstddef>
#include <functional>

namespace Nz
{
	class NAZARA_LUA_API LuaCoroutine : public LuaState
	{
		friend class LuaState;

		public:
			LuaCoroutine(const LuaCoroutine&) = delete;
			inline LuaCoroutine(LuaCoroutine&& instance);
			~LuaCoroutine();

			bool CanResume() const;

			Ternary Resume(unsigned int argCount = 0);

			LuaCoroutine& operator=(const LuaCoroutine&) = delete;
			inline LuaCoroutine& operator=(LuaCoroutine&& instance);

		private:
			LuaCoroutine(lua_State* internalState, int refIndex);

			bool Run(int argCount, int resultCount) override;

			int m_ref;
	};
}

#include <Nazara/Lua/LuaInstance.inl>

#endif // NAZARA_LUACOROUTINE_HPP
