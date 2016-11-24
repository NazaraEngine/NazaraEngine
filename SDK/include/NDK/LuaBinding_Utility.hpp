// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_UTILITY_HPP
#define NDK_LUABINDING_UTILITY_HPP

#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Utility : public LuaBinding_Base
	{
		public:
			LuaBinding_Utility(LuaBinding& binding);
			~LuaBinding_Utility() = default;

			void Register(Nz::LuaInstance& instance) override;

			// Utility
			Nz::LuaClass<Nz::AbstractImageRef> abstractImage;
			Nz::LuaClass<Nz::FontRef> font;
			Nz::LuaClass<Nz::Keyboard> keyboard;
			Nz::LuaClass<Nz::Node> node;
	};
}

#endif // NDK_LUABINDING_UTILITY_HPP
