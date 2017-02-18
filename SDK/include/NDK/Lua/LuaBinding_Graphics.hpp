// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_GRAPHICS_HPP
#define NDK_LUABINDING_GRAPHICS_HPP

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Graphics : public LuaBinding_Base
	{
		public:
			LuaBinding_Graphics(LuaBinding& binding);
			~LuaBinding_Graphics() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::AbstractViewer> abstractViewer;
			Nz::LuaClass<Nz::InstancedRenderableRef> instancedRenderable;
			Nz::LuaClass<Nz::MaterialRef> material;
			Nz::LuaClass<Nz::ModelRef> model;
			Nz::LuaClass<Nz::SpriteRef> sprite;
			Nz::LuaClass<Nz::SpriteLibrary> spriteLibrary;
	};
}

#endif // NDK_LUABINDING_GRAPHICS_HPP
