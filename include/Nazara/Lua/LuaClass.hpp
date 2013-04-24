// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUACLASS_HPP
#define NAZARA_LUACLASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <map>

template<class T>
class NzLuaClass : public NzLuaClassImpl
{
	public:
		using ClassFunc = int (*)(NzLuaInstance& lua, T& instance);
		using ClassIndexFunc = bool (*)(NzLuaInstance& lua, T& instance);
		using ConstructorFunc = T* (*)(NzLuaInstance& lua);
		using FinalizerFunc = bool (*)(NzLuaInstance& lua, T& instance);

		NzLuaClass(const NzString& name);

		//template<class U> void Inherit(NzLuaClass<U>& parent);

		void Register(NzLuaInstance& lua);

		void SetConstructor(ConstructorFunc constructor);
		void SetFinalizer(FinalizerFunc finalizer);
		void SetGetter(ClassIndexFunc getter);
		void SetMethod(const NzString& name, ClassFunc method);
		void SetSetter(ClassIndexFunc setter);

	private:
		static int ConstructorProxy(lua_State* state);
		static int FinalizerProxy(lua_State* state);
		static int GetterProxy(lua_State* state);
		static int MethodProxy(lua_State* state);
		static int SetterProxy(lua_State* state);

		std::map<NzString, ClassFunc> m_methods;
		ClassIndexFunc m_getter;
		ClassIndexFunc m_setter;
		ConstructorFunc m_constructor;
		FinalizerFunc m_finalizer;
		NzString m_name;
};

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
