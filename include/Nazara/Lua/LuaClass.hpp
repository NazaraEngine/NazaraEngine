// Copyright (C) 2014 Jérôme Leclercq
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

template<class T/*, class P = void*/>
class NzLuaClass
{
	//static_assert(std::is_same<P, void>::value || std::is_base_of<P, T>::value, "P must be a base of T");

	public:
		using ClassFunc = int (*)(NzLuaInstance& lua, T& instance);
		using ClassIndexFunc = bool (*)(NzLuaInstance& lua, T& instance);
		using ConstructorFunc = T* (*)(NzLuaInstance& lua);
		using FinalizerFunc = bool (*)(NzLuaInstance& lua, T& instance);
		using StaticIndexFunc = bool (*)(NzLuaInstance& lua);
		using StaticFunc = int (*)(NzLuaInstance& lua);

		NzLuaClass(const NzString& name);

		//void Inherit(NzLuaClass<P>& parent);

		void Register(NzLuaInstance& lua);

		void PushGlobalTable(NzLuaInstance& lua);

		void SetConstructor(ConstructorFunc constructor);
		void SetFinalizer(FinalizerFunc finalizer);
		void SetGetter(ClassIndexFunc getter);
		void SetMethod(const NzString& name, ClassFunc method);
		void SetSetter(ClassIndexFunc setter);
		void SetStaticGetter(StaticIndexFunc getter);
		void SetStaticMethod(const NzString& name, StaticFunc func);
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
			NzString name;
			int globalTableRef = -1;
		};

		std::map<NzString, ClassFunc> m_methods;
		std::map<NzString, StaticFunc> m_staticMethods;
		std::shared_ptr<ClassInfo> m_info;
};

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
