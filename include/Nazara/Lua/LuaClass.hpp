// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUACLASS_HPP
#define NAZARA_LUACLASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Nz
{
	template<class T>
	class LuaClass
	{
		template<class U>
		friend class LuaClass;

		public:
			using ClassFunc = std::function<int(LuaInstance& lua, T& instance)>;
			using ClassIndexFunc = std::function<bool(LuaInstance& lua, T& instance)>;
			using ConstructorFunc = std::function<bool(LuaInstance& lua, T* instance, std::size_t argumentCount)>;
			template<typename P> using ConvertToParent = std::function<P*(T*)>;
			using FinalizerFunc = std::function<bool(LuaInstance& lua, T& instance)>;
			using StaticIndexFunc = std::function<bool(LuaInstance& lua)>;
			using StaticFunc = std::function<int(LuaInstance& lua)>;

			LuaClass() = default;
			LuaClass(const String& name);

			void BindDefaultConstructor();

			void BindMethod(const String& name, ClassFunc method);
			template<typename R, typename P, typename... Args, typename... DefArgs> std::enable_if_t<std::is_base_of<P, T>::value> BindMethod(const String& name, R(P::*func)(Args...), DefArgs&&... defArgs);
			template<typename R, typename P, typename... Args, typename... DefArgs> std::enable_if_t<std::is_base_of<P, T>::value> BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs);
			template<typename R, typename P, typename... Args, typename... DefArgs> std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> BindMethod(const String& name, R(P::*func)(Args...), DefArgs&&... defArgs);
			template<typename R, typename P, typename... Args, typename... DefArgs> std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs);

			void BindStaticMethod(const String& name, StaticFunc func);
			template<typename R, typename... Args, typename... DefArgs> void BindStaticMethod(const String& name, R(*func)(Args...), DefArgs&&... defArgs);

			template<class P> void Inherit(LuaClass<P>& parent);
			template<class P> void Inherit(LuaClass<P>& parent, ConvertToParent<P> convertFunc);

			void Reset();
			void Reset(const String& name);

			void Register(LuaInstance& lua);

			void PushGlobalTable(LuaInstance& lua);

			void SetConstructor(ConstructorFunc constructor);
			void SetFinalizer(FinalizerFunc finalizer);
			void SetGetter(ClassIndexFunc getter);
			void SetSetter(ClassIndexFunc setter);
			void SetStaticGetter(StaticIndexFunc getter);
			void SetStaticSetter(StaticIndexFunc getter);

		private:
			void PushClassInfo(LuaInstance& lua);
			void SetupConstructor(LuaInstance& lua);
			void SetupDefaultToString(LuaInstance& lua);
			void SetupFinalizer(LuaInstance& lua);
			void SetupGetter(LuaInstance& lua, LuaCFunction proxy);
			void SetupGlobalTable(LuaInstance& lua);
			void SetupMetatable(LuaInstance& lua);
			void SetupMethod(LuaInstance& lua, LuaCFunction proxy, const String& name, std::size_t methodIndex);
			void SetupSetter(LuaInstance& lua, LuaCFunction proxy);
		
			using ParentFunc = std::function<void(LuaInstance& lua, T* instance)>;
			using InstanceGetter = std::function<T*(LuaInstance& lua)>;

			struct ClassInfo
			{
				std::vector<ClassFunc> methods;
				std::vector<ParentFunc> parentGetters;
				std::vector<StaticFunc> staticMethods;
				std::unordered_map<String, InstanceGetter> instanceGetters;
				ClassIndexFunc getter;
				ClassIndexFunc setter;
				ConstructorFunc constructor;
				FinalizerFunc finalizer;
				StaticIndexFunc staticGetter;
				StaticIndexFunc staticSetter;
				String name;
				int globalTableRef = -1;
			};

			static int ConstructorProxy(lua_State* state);
			static int FinalizerProxy(lua_State* state);
			static int InfoDestructor(lua_State* state);
			static void Get(const std::shared_ptr<ClassInfo>& info, LuaInstance& lua, T* instance);
			static int GetterProxy(lua_State* state);
			static int MethodProxy(lua_State* state);
			static int SetterProxy(lua_State* state);
			static int StaticGetterProxy(lua_State* state);
			static int StaticMethodProxy(lua_State* state);
			static int StaticSetterProxy(lua_State* state);
			static int ToStringProxy(lua_State* state);

			std::map<String, ClassFunc> m_methods;
			std::map<String, StaticFunc> m_staticMethods;
			std::shared_ptr<ClassInfo> m_info;
	};
}

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
