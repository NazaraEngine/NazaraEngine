// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUA_HPP
#define NAZARA_LUA_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Lua/Config.hpp>

namespace Nz
{
	class NAZARA_LUA_API Lua
	{
		public:
			Lua() = delete;
			~Lua() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_LUA_HPP
