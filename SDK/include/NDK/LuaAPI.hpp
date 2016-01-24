// Copyright (C) 2015 Jérôme Leclercq
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
	class NDK_API LuaAPI
	{
		public:
			LuaAPI() = delete;
			~LuaAPI() = delete;

			static void Register_Audio(Nz::LuaInstance& instance);
			static void Register_Core(Nz::LuaInstance& instance);
			static void Register_Graphics(Nz::LuaInstance& instance);
			static void Register_Math(Nz::LuaInstance& instance);
			static void Register_Renderer(Nz::LuaInstance& instance);
			static void Register_Utility(Nz::LuaInstance& instance);
	};
}

#include <NDK/LuaAPI.inl>

#endif // NDK_LUAINTERFACE_HPP
