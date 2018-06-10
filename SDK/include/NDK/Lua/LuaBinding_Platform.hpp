// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_LUABINDING_SYSTEM_HPP
#define NDK_LUABINDING_SYSTEM_HPP

#include <Nazara/Platform/Keyboard.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Platform : public LuaBinding_Base
	{
		public:
			LuaBinding_Platform(LuaBinding& binding);
			~LuaBinding_Platform() = default;

			void Register(Nz::LuaState& state) override;

			// Platform
			Nz::LuaClass<Nz::Keyboard> keyboard;
	};
}

#endif // NDK_LUABINDING_SYSTEM_HPP
