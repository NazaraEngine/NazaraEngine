// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUAINSTANCE_HPP
#define NAZARA_LUAINSTANCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Lua/LuaState.hpp>
#include <cstddef>

namespace Nz
{
	class NAZARA_LUA_API LuaInstance : public LuaState
	{
		friend class LuaCoroutine;
		friend class LuaState;

		public:
			LuaInstance();
			LuaInstance(const LuaInstance&) = delete;
			LuaInstance(LuaInstance&& instance);
			~LuaInstance();

			inline std::size_t GetMemoryLimit() const;
			inline std::size_t GetMemoryUsage() const;
			inline UInt32 GetTimeLimit() const;

			inline void SetMemoryLimit(std::size_t memoryLimit);
			inline void SetTimeLimit(UInt32 limit);

			LuaInstance& operator=(const LuaInstance&) = delete;
			LuaInstance& operator=(LuaInstance&& instance);

		private:
			inline void SetMemoryUsage(std::size_t memoryUsage);

			static void* MemoryAllocator(void *ud, void *ptr, std::size_t osize, std::size_t nsize);
			static void TimeLimiter(lua_State* internalState, lua_Debug* debug);

			std::size_t m_memoryLimit;
			std::size_t m_memoryUsage;
			UInt32 m_timeLimit;
			Clock m_clock;
			unsigned int m_level;
	};
}

#include <Nazara/Lua/LuaInstance.inl>

#endif // NAZARA_LUAINSTANCE_HPP
