// Copyright (C) 2017 Jérôme Leclercq
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
			using ClassFunc = std::function<int(LuaState& state, T& instance, std::size_t argumentCount)>;
			using ClassIndexFunc = std::function<bool(LuaState& state, T& instance)>;
			using ConstructorFunc = std::function<bool(LuaState& state, T* instance, std::size_t argumentCount)>;
			template<typename P> using ConvertToParent = std::function<P*(T*)>;
			using FinalizerFunc = std::function<bool(LuaState& state, T& instance)>;
			using StaticIndexFunc = std::function<bool(LuaState& state)>;
			using StaticFunc = std::function<int(LuaState& state)>;

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

			void Register(LuaState& state);

			void PushGlobalTable(LuaState& state);

			void SetConstructor(ConstructorFunc constructor);
			void SetFinalizer(FinalizerFunc finalizer);
			void SetGetter(ClassIndexFunc getter);
			void SetSetter(ClassIndexFunc setter);
			void SetStaticGetter(StaticIndexFunc getter);
			void SetStaticSetter(StaticIndexFunc getter);

		private:
			template<typename U, bool HasDestructor>
			friend struct LuaClassImplFinalizerSetupProxy;

			void PushClassInfo(LuaState& state);
			void SetupConstructor(LuaState& state);
			void SetupDefaultToString(LuaState& state);
			void SetupFinalizer(LuaState& state);
			void SetupGetter(LuaState& state, LuaCFunction proxy);
			void SetupGlobalTable(LuaState& state);
			void SetupMetatable(LuaState& state);
			void SetupMethod(LuaState& state, LuaCFunction proxy, const String& name, std::size_t methodIndex);
			void SetupSetter(LuaState& state, LuaCFunction proxy);

			using ParentFunc = std::function<void(LuaState& state, T* instance)>;
			using InstanceGetter = std::function<T*(LuaState& state)>;

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

			static int ConstructorProxy(lua_State* internalState);
			static int FinalizerProxy(lua_State* internalState);
			static int InfoDestructor(lua_State* internalState);
			static void Get(const std::shared_ptr<ClassInfo>& info, LuaState& state, T* instance);
			static int GetterProxy(lua_State* internalState);
			static int MethodProxy(lua_State* internalState);
			static int SetterProxy(lua_State* internalState);
			static int StaticGetterProxy(lua_State* internalState);
			static int StaticMethodProxy(lua_State* internalState);
			static int StaticSetterProxy(lua_State* internalState);
			static int ToStringProxy(lua_State* internalState);

			std::map<String, ClassFunc> m_methods;
			std::map<String, StaticFunc> m_staticMethods;
			std::shared_ptr<ClassInfo> m_info;
	};
}

#include <Nazara/Lua/LuaClass.inl>

#endif // NAZARA_LUACLASS_HPP
