// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUAINTERFACE_HPP
#define NDK_LUAINTERFACE_HPP

#include <NDK/Prerequesites.hpp>

namespace Nz
{
	class LuaInstance;
}

namespace Ndk
{
	class LuaBinding;

	class NDK_API LuaAPI
	{
		public:
			LuaAPI() = delete;
			~LuaAPI() = delete;

			static LuaBinding* GetBinding();

			static bool Initialize();

			static void RegisterClasses(Nz::LuaInstance& instance);

			static void Uninitialize();

		private:
			static LuaBinding* s_binding;
	};
}

#include <NDK/LuaAPI.inl>

#endif // NDK_LUAINTERFACE_HPP
