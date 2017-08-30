// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaClass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <type_traits>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	template<class T>
	LuaClass<T>::LuaClass(const String& name)
	{
		Reset(name);
	}

	template<class T>
	inline void LuaClass<T>::BindDefaultConstructor()
	{
		SetConstructor([] (Nz::LuaState& state, T* instance, std::size_t argumentCount)
		{
			NazaraUnused(state);
			NazaraUnused(argumentCount);

			PlacementNew(instance);
			return true;
		});
	}

	template<class T>
	template<class P>
	inline void LuaClass<T>::Inherit(LuaClass<P>& parent)
	{
		Inherit<P>(parent, [] (T* instance) -> P*
		{
			return static_cast<P*>(instance);
		});
	}

	template<class T>
	template<class P>
	inline void LuaClass<T>::Inherit(LuaClass<P>& parent, ConvertToParent<P> convertFunc)
	{
		static_assert(!std::is_same<P, T>::value || std::is_base_of<P, T>::value, "P must be a base of T");

		std::shared_ptr<typename LuaClass<P>::ClassInfo>& parentInfo = parent.m_info;

		parentInfo->instanceGetters[m_info->name] = [info = m_info, convertFunc] (LuaState& state) -> P*
		{
			return convertFunc(static_cast<T*>(state.CheckUserdata(1, info->name)));
		};

		m_info->parentGetters.emplace_back([parentInfo, convertFunc] (LuaState& state, T* instance)
		{
			LuaClass<P>::Get(parentInfo, state, convertFunc(instance));
		});
	}


	template<class T>
	void LuaClass<T>::Reset()
	{
		m_info.reset();
	}

	template<class T>
	void LuaClass<T>::Reset(const String& name)
	{
		m_info = std::make_shared<ClassInfo>();
		m_info->name = name;

		m_info->instanceGetters[m_info->name] = [info = m_info] (LuaState& state)
		{
			return static_cast<T*>(state.CheckUserdata(1, info->name));
		};
	}

	template<class T>
	void LuaClass<T>::Register(LuaState& state)
	{
		PushClassInfo(state);

		// Let's create the metatable which will be associated with every state.
		SetupMetatable(state);

		if (m_info->constructor || m_info->staticGetter || m_info->staticSetter || !m_staticMethods.empty())
			SetupGlobalTable(state);

		state.Pop(); // Pop our ClassInfo, which is now referenced by all our functions
	}

	template<class T>
	void LuaClass<T>::PushGlobalTable(LuaState& state)
	{
		state.PushReference(m_info->globalTableRef);
	}

	template<class T>
	void LuaClass<T>::SetConstructor(ConstructorFunc constructor)
	{
		m_info->constructor = constructor;
	}

	template<class T>
	void LuaClass<T>::SetFinalizer(FinalizerFunc finalizer)
	{
		m_info->finalizer = finalizer;
	}

	template<class T>
	void LuaClass<T>::SetGetter(ClassIndexFunc getter)
	{
		m_info->getter = getter;
	}

	template<class T>
	void LuaClass<T>::BindMethod(const String& name, ClassFunc method)
	{
		m_methods[name] = method;
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, T>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...), DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaState& state, T& object, std::size_t /*argumentCount*/) -> int
		{
			handler.ProcessArguments(state);

			return handler.Invoke(state, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, T>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaState& state, T& object, std::size_t /*argumentCount*/) -> int
		{
			handler.ProcessArguments(state);

			return handler.Invoke(state, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...), DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaState& state, T& object, std::size_t /*argumentCount*/) -> int
		{
			handler.ProcessArguments(state);

			return handler.Invoke(state, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaState& state, T& object, std::size_t /*argumentCount*/) -> int
		{
			handler.ProcessArguments(state);

			return handler.Invoke(state, object, func);
		});
	}

	template<class T>
	void LuaClass<T>::SetSetter(ClassIndexFunc setter)
	{
		m_info->setter = setter;
	}

	template<class T>
	void LuaClass<T>::SetStaticGetter(StaticIndexFunc getter)
	{
		m_info->staticGetter = getter;
	}

	template<class T>
	void LuaClass<T>::BindStaticMethod(const String& name, StaticFunc method)
	{
		m_staticMethods[name] = method;
	}

	template<class T>
	template<typename R, typename... Args, typename... DefArgs>
	void LuaClass<T>::BindStaticMethod(const String& name, R(*func)(Args...), DefArgs&&... defArgs)
	{
		typename LuaImplFunctionProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindStaticMethod(name, [func, handler] (LuaState& state) -> int
		{
			handler.ProcessArguments(state);

			return handler.Invoke(state, func);
		});
	}

	template<class T>
	void LuaClass<T>::SetStaticSetter(StaticIndexFunc setter)
	{
		m_info->staticSetter = setter;
	}

	template<class T>
	void LuaClass<T>::PushClassInfo(LuaState& state)
	{
		// Our ClassInfo has to outlive the LuaClass, because we don't want to force the user to keep the LuaClass alive
		// To do that, each Registration creates a tiny shared_ptr wrapper whose life is directly managed by Lua.
		// This shared_ptr object gets pushed as a up-value for every proxy function set in the metatable.
		// This way, there is no way our ClassInfo gets freed before any instance and the global class gets destroyed.
		std::shared_ptr<ClassInfo>* info = static_cast<std::shared_ptr<ClassInfo>*>(state.PushUserdata(sizeof(std::shared_ptr<ClassInfo>)));
		PlacementNew(info, m_info);

		// Setup a tiny metatable to let Lua know how to destroy our ClassInfo
		state.PushTable(0, 1);
			state.PushLightUserdata(info);
			state.PushCFunction(InfoDestructor, 1);
			state.SetField("__gc");
		state.SetMetatable(-2);
	}

	template<class T>
	void LuaClass<T>::SetupConstructor(LuaState& state)
	{
			state.PushValue(1); // ClassInfo
		state.PushCFunction(ConstructorProxy, 1);
		state.SetField("__call"); // ClassMeta.__call = ConstructorProxy
	}

	template<class T>
	void LuaClass<T>::SetupDefaultToString(LuaState& state)
	{
			state.PushValue(1); // shared_ptr on UserData
		state.PushCFunction(ToStringProxy, 1);
		state.SetField("__tostring");
	}

	template<typename T, bool HasDestructor>
	struct LuaClassImplFinalizerSetupProxy;

	template<typename T>
	struct LuaClassImplFinalizerSetupProxy<T, true>
	{
		static void Setup(LuaState& state)
		{
			state.PushValue(1); // ClassInfo
			state.PushCFunction(LuaClass<T>::FinalizerProxy, 1);
			state.SetField("__gc");
		}
	};

	template<typename T>
	struct LuaClassImplFinalizerSetupProxy<T, false>
	{
		static void Setup(LuaState&)
		{
		}
	};

	template<class T>
	void LuaClass<T>::SetupFinalizer(LuaState& state)
	{
		LuaClassImplFinalizerSetupProxy<T, std::is_destructible<T>::value>::Setup(state);
	}

	template<class T>
	void LuaClass<T>::SetupGetter(LuaState& state, LuaCFunction proxy)
	{
			state.PushValue(1);  // ClassInfo
			state.PushValue(-2); // Metatable
		state.PushCFunction(proxy, 2);

		state.SetField("__index"); // Getter
	}

	template<class T>
	void LuaClass<T>::SetupGlobalTable(LuaState& state)
	{
		// Create the global table
		state.PushTable(); // Class = {}

		// Create a metatable which will be used for our global table
		state.PushTable(); // ClassMeta = {}

		if (m_info->constructor)
			SetupConstructor(state);

		if (m_info->staticGetter)
			SetupGetter(state, StaticGetterProxy);
		else
		{
			// Optimize by assigning the metatable instead of a search function
			state.PushValue(-1); // Metatable
			state.SetField("__index");
		}

		if (m_info->staticSetter)
			SetupSetter(state, StaticSetterProxy);

		m_info->staticMethods.reserve(m_staticMethods.size());
		for (auto& pair : m_staticMethods)
		{
			std::size_t methodIndex = m_info->staticMethods.size();
			m_info->staticMethods.push_back(pair.second);

			SetupMethod(state, StaticMethodProxy, pair.first, methodIndex);
		}

		state.SetMetatable(-2); // setmetatable(Class, ClassMeta), pops ClassMeta

		state.PushValue(-1); // As CreateReference() pops the table, push a copy
		m_info->globalTableRef = state.CreateReference();

		state.SetGlobal(m_info->name); // _G["Class"] = Class
	}

	template<class T>
	void LuaClass<T>::SetupMetatable(LuaState& state)
	{
		if (!state.NewMetatable(m_info->name))
			NazaraWarning("Class \"" + m_info->name + "\" already registred in this instance");
		{
			SetupFinalizer(state);

			if (m_info->getter || !m_info->parentGetters.empty())
				SetupGetter(state, GetterProxy);
			else
			{
				// Optimize by assigning the metatable instead of a search function
				// This is only possible if we have no custom getter nor parent
				state.PushValue(-1); // Metatable
				state.SetField("__index");
			}

			if (m_info->setter)
				SetupSetter(state, SetterProxy);

			// In case a __tostring method is missing, add a default implementation returning the class name
			if (m_methods.find("__tostring") == m_methods.end())
				SetupDefaultToString(state);

			m_info->methods.reserve(m_methods.size());
			for (auto& pair : m_methods)
			{
				std::size_t methodIndex = m_info->methods.size();
				m_info->methods.push_back(pair.second);

				SetupMethod(state, MethodProxy, pair.first, methodIndex);
			}
		}
		state.Pop(); //< Pops the metatable, it won't be collected before it's referenced by the Lua registry.
	}

	template<class T>
	void LuaClass<T>::SetupMethod(LuaState& state, LuaCFunction proxy, const String& name, std::size_t methodIndex)
	{
			state.PushValue(1); // ClassInfo
			state.PushInteger(methodIndex);
		state.PushCFunction(proxy, 2);

		state.SetField(name); // Method name
	}

	template<class T>
	void LuaClass<T>::SetupSetter(LuaState& state, LuaCFunction proxy)
	{
			state.PushValue(1); // ClassInfo
		state.PushCFunction(proxy, 1);

		state.SetField("__newindex"); // Setter
	}


	template<class T>
	int LuaClass<T>::ConstructorProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		const ConstructorFunc& constructor = info->constructor;

		state.Remove(1); // On enlève l'argument "table" du stack

		std::size_t argCount = state.GetStackTop();

		T* instance = static_cast<T*>(state.PushUserdata(sizeof(T)));

		if (!constructor(state, instance, argCount))
		{
			state.Error("Constructor failed");
			return 0; // Normalement jamais exécuté (l'erreur provoquant une exception)
		}

		state.SetMetatable(info->name);
		return 1;
	}

	template<class T>
	int LuaClass<T>::FinalizerProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		const FinalizerFunc& finalizer = info->finalizer;

		T* instance = static_cast<T*>(state.CheckUserdata(1, info->name));
		state.Remove(1); //< Remove the instance from the Lua stack

		if (!finalizer || finalizer(state, *instance))
			instance->~T();

		return 0;
	}

	template<class T>
	int LuaClass<T>::InfoDestructor(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		state.DestroyReference(info->globalTableRef);

		using namespace std; // Obligatoire pour le destructeur
		info.~shared_ptr(); // Si vous voyez une autre façon de faire, je suis preneur

		return 0;
	}

	template<class T>
	void LuaClass<T>::Get(const std::shared_ptr<ClassInfo>& info, LuaState& state, T* instance)
	{
		const ClassIndexFunc& getter = info->getter;

		if (!getter || !getter(state, *instance))
		{
			// Query from the metatable
			state.GetMetatable(info->name); //< Metatable
			state.PushValue(2); //< Field
			state.GetTable(); // Metatable[Field]

			state.Remove(-2); // Remove Metatable

			if (!state.IsValid(-1))
			{
				for (const ParentFunc& parentGetter : info->parentGetters)
				{
					state.Pop(); //< Pop the last nil value

					parentGetter(state, instance);
					if (state.IsValid(-1))
						return;
				}
			}
		}
	}

	template<class T>
	int LuaClass<T>::GetterProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));

		T* instance = static_cast<T*>(state.CheckUserdata(1, info->name));

		Get(info, state, instance);
		return 1;
	}

	template<class T>
	int LuaClass<T>::MethodProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));

		T* instance = nullptr;
		if (state.GetMetatable(1))
		{
			LuaType type = state.GetField("__name");
			if (type == LuaType_String)
			{
				String name = state.ToString(-1);
				auto it = info->instanceGetters.find(name);
				if (it != info->instanceGetters.end())
					instance = it->second(state);
			}
			state.Pop(2);
		}

		if (!instance)
		{
			state.Error("Method cannot be called without an object");
			return 0;
		}

		std::size_t argCount = state.GetStackTop() - 1U;

		unsigned int index = static_cast<unsigned int>(state.ToInteger(state.GetIndexOfUpValue(2)));
		const ClassFunc& method = info->methods[index];
		return method(state, *instance, argCount);
	}

	template<class T>
	int LuaClass<T>::SetterProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		const ClassIndexFunc& setter = info->setter;

		T& instance = *static_cast<T*>(state.CheckUserdata(1, info->name));

		if (!setter(state, instance))
		{
			std::size_t length;
			const char* str = state.ToString(2, &length);

			state.Error("Class \"" + info->name + "\" has no field \"" + String(str, length) + "\")");
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::StaticGetterProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		const StaticIndexFunc& getter = info->staticGetter;

		if (!getter(state))
		{
			// On accède alors à la table
			state.PushValue(state.GetIndexOfUpValue(2));
			state.PushValue(-2);
			state.GetTable();
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::StaticMethodProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		unsigned int index = static_cast<unsigned int>(state.ToInteger(state.GetIndexOfUpValue(2)));
		const StaticFunc& method = info->staticMethods[index];

		return method(state);
	}

	template<class T>
	int LuaClass<T>::StaticSetterProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));
		const StaticIndexFunc& setter = info->staticSetter;

		if (!setter(state))
		{
			std::size_t length;
			const char* str = state.ToString(2, &length);

			state.Error("Class \"" + info->name + "\" has no static field \"" + String(str, length) + ')');
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::ToStringProxy(lua_State* internalState)
	{
		LuaState state = LuaInstance::GetState(internalState);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(state.ToUserdata(state.GetIndexOfUpValue(1)));

		state.PushString(info->name);
		return 1;
	}
}

#include <Nazara/Lua/DebugOff.hpp>
