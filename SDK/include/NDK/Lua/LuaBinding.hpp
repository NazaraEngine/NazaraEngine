// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_HPP
#define NDK_LUABINDING_HPP

#include <NDK/BaseComponent.hpp>
#include <NDK/Entity.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>
#include <memory>

namespace Ndk
{
	class NDK_API LuaBinding
	{
		friend class LuaBinding_SDK;

		public:
			LuaBinding();
			~LuaBinding() = default;

			template<typename T> void BindComponent(const Nz::String& name);

			void RegisterClasses(Nz::LuaInstance& instance);

			std::unique_ptr<LuaBinding_Base> core;
			std::unique_ptr<LuaBinding_Base> math;
			std::unique_ptr<LuaBinding_Base> network;
			std::unique_ptr<LuaBinding_Base> sdk;
			std::unique_ptr<LuaBinding_Base> utility;

			#ifndef NDK_SERVER
			std::unique_ptr<LuaBinding_Base> audio;
			std::unique_ptr<LuaBinding_Base> graphics;
			std::unique_ptr<LuaBinding_Base> renderer;
			#endif

		private:
			template<typename T>
			static int AddComponentOfType(Nz::LuaInstance& lua, EntityHandle& handle);

			template<typename T>
			static int PushComponentOfType(Nz::LuaInstance& lua, BaseComponent& component);

			using AddComponentFunc = int(*)(Nz::LuaInstance&, EntityHandle&);
			using GetComponentFunc = int(*)(Nz::LuaInstance&, BaseComponent&);

			struct ComponentBinding
			{
				AddComponentFunc adder;
				ComponentIndex index;
				GetComponentFunc getter;
				Nz::String name;
			};

			ComponentBinding* QueryComponentIndex(Nz::LuaInstance& lua, int argIndex = 2);

			std::vector<ComponentBinding> m_componentBinding;
			std::unordered_map<Nz::String, ComponentIndex> m_componentBindingByName;
	};
}

#include <NDK/Lua/LuaBinding.inl>

#endif // NDK_LUABINDING_HPP
