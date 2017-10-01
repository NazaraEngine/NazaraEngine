// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_SDK_HPP
#define NDK_LUABINDING_SDK_HPP

#include <NDK/Lua/LuaBinding_Base.hpp>
#include <NDK/Components.hpp>
#include <NDK/Console.hpp>
#include <NDK/Entity.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	class Application;

	class NDK_API LuaBinding_SDK : public LuaBinding_Base
	{
		public:
			LuaBinding_SDK(LuaBinding& binding);
			~LuaBinding_SDK() = default;

			void Register(Nz::LuaState& state) override;

			Nz::LuaClass<Application*> application;
			Nz::LuaClass<EntityHandle> entity;
			Nz::LuaClass<NodeComponentHandle> nodeComponent;
			Nz::LuaClass<VelocityComponentHandle> velocityComponent;
			Nz::LuaClass<WorldHandle> world;

			#ifndef NDK_SERVER
			Nz::LuaClass<CameraComponentHandle> cameraComponent;
			Nz::LuaClass<ConsoleHandle> console;
			Nz::LuaClass<GraphicsComponentHandle> graphicsComponent;
			#endif

	};
}

#endif // NDK_LUABINDING_SDK_HPP
