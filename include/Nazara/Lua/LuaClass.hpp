// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUACLASS_HPP
#define NAZARA_LUACLASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <map>
#include <memory>
//#include <type_traits>

namespace Nz
{
	template<class T/*, class P = void*/>
	class LuaClass
	{
		//static_assert(std::is_same<P, void>::value || std::is_base_of<P, T>::value, "P must be a base of T");

		public:
			using ClassFunc = int (*)(LuaInstance& lua, T& instance);
			using ClassIndexFunc = bool (*)(LuaInstance& lua, T& instance);
			using ConstructorFunc = T* (*)(LuaInstance& lua);
			using FinalizerFunc = bool (*)(LuaInstance& lua, T& instance);
			using StaticIndexFunc = bool (*)(LuaInstance& lua);
			using StaticFunc = int (*)(LuaInstance& lua);

			LuaClass(const String& name);

			//void Inherit(LuaClass<P>& parent);

			void Register(LuaInstance& lua);

			void PushGlobalTable(LuaInstance& lua);

			void SetConstructor(ConstructorFunc constructor);
			void SetFinalizer(FinalizerFunc finalizer);
			void SetGetter(ClassIndexFunc getter);
			void SetMethod(const String& name, ClassFunc method);
			void SetSetter(ClassIndexFunc setter);
			void SetStaticGetter(StaticIndexFunc getter);
			void SetStaticMethod(const String& name, StaticFunc func);
			void SetStaticSetter(StaticIndexFunc getter);

		private:
			static int ConstructorProxy(lua_State* state);
			static int FinalizerProxy(lua_State* state);
			static int InfoDestructor(lua_State* state);
			static int GetterProxy(lua_State* state);
			static int MethodProxy(lua_State* state);
			static int SetterProxy(lua_State* state);
			static int StaticGetterProxy(lua_State* state);
			static int StaticMethodProxy(lua_State* state);
			static int StaticSetterProxy(lua_State* state);

			struct ClassInfo
			{
				std::vector<ClassFunc> methods;
				std::vector<StaticFunc> staticMethods;
				ClassIndexFunc getter = nullptr;
				ClassIndexFunc setter = nullptr;
				ConstructorFunc constructor = nullptr;
				FinalizerFunc finalizer = nullptr;
				StaticIndexFunc staticGetter = nullptr;
				StaticIndexFunc staticSetter = nullptr;
				String name;
				int globalTableRef = -1;
			};

			std::map<String, ClassFunc> m_methods;
			std::map<String, StaticFunc> m_staticMethods;
			std::shared_ptr<ClassInfo> m_info;
	};
}

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
