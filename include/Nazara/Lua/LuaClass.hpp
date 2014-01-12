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

		NzLuaClass(const NzString& name);

		//void Inherit(NzLuaClass<P>& parent);

		void Register(NzLuaInstance& lua);

		void SetConstructor(ConstructorFunc constructor);
		void SetFinalizer(FinalizerFunc finalizer);
		void SetGetter(ClassIndexFunc getter);
		void SetMethod(const NzString& name, ClassFunc method);
		void SetSetter(ClassIndexFunc setter);

	private:
		static int ConstructorProxy(lua_State* state);
		static int FinalizerProxy(lua_State* state);
		static int InfoDestructor(lua_State* state);
		static int GetterProxy(lua_State* state);
		static int MethodProxy(lua_State* state);
		static int SetterProxy(lua_State* state);

		struct ClassInfo
		{
			std::vector<ClassFunc> methods;
			ClassIndexFunc getter = nullptr;
			ClassIndexFunc setter = nullptr;
			ConstructorFunc constructor = nullptr;
			FinalizerFunc finalizer = nullptr;
			NzString name;
		};

		std::map<NzString, ClassFunc> m_methods;
		std::shared_ptr<ClassInfo> m_info;
};

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
