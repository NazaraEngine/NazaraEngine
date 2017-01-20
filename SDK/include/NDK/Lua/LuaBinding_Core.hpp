// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_CORE_HPP
#define NDK_LUABINDING_CORE_HPP

#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Core : public LuaBinding_Base
	{
		public:
			LuaBinding_Core(LuaBinding& binding);
			~LuaBinding_Core() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::Clock> clock;
			Nz::LuaClass<Nz::Directory> directory;
			Nz::LuaClass<Nz::File> file;
			Nz::LuaClass<Nz::Stream> stream;
	};
}

#endif // NDK_LUABINDING_CORE_HPP
