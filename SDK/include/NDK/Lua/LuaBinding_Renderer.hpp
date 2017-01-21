// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_RENDERER_HPP
#define NDK_LUABINDING_RENDERER_HPP

#include <Nazara/Renderer/Texture.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Renderer : public LuaBinding_Base
	{
		public:
			LuaBinding_Renderer(LuaBinding& binding);
			~LuaBinding_Renderer() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::TextureRef> texture;
			Nz::LuaClass<Nz::TextureLibrary> textureLibrary;
			Nz::LuaClass<Nz::TextureManager> textureManager;
	};
}

#endif // NDK_LUABINDING_RENDERER_HPP
