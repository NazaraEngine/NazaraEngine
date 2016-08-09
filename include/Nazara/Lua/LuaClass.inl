// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <type_traits>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	template<class T>
	LuaClass<T>::LuaClass(const String& name) :
	m_info(new ClassInfo)
	{
		m_info->name = name;
	}

	template<class T>
	inline void LuaClass<T>::BindDefaultConstructor()
	{
		SetConstructor([] (Nz::LuaInstance& lua, T* instance)
		{
			NazaraUnused(lua);

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

		parentInfo->instanceGetters[m_info->name] = [info = m_info, convertFunc] (LuaInstance& lua) -> P*
		{
			return convertFunc(static_cast<T*>(lua.CheckUserdata(1, info->name)));
		};

		m_info->parentGetters.emplace_back([parentInfo, convertFunc] (LuaInstance& lua, T* instance)
		{
			LuaClass<P>::Get(parentInfo, lua, convertFunc(instance));
		});
	}

	template<class T>
	void LuaClass<T>::Register(LuaInstance& lua)
	{
		// Le ClassInfo doit rester en vie jusqu'à la fin du script
		// Obliger l'instance de LuaClass à rester en vie dans cette fin serait contraignant pour l'utilisateur
		// J'utilise donc une astuce, la stocker dans une UserData associée avec chaque fonction de la metatable du type,
		// cette UserData disposera d'un finalizer qui libérera le ClassInfo
		// Ainsi c'est Lua qui va s'occuper de la destruction pour nous :-)
		// De même, l'utilisation d'un shared_ptr permet de garder la structure en vie même si l'instance est libérée avant le LuaClass
		std::shared_ptr<ClassInfo>* info = static_cast<std::shared_ptr<ClassInfo>*>(lua.PushUserdata(sizeof(std::shared_ptr<ClassInfo>)));
		PlacementNew(info, m_info);

		// On créé la table qui contiendra une méthode (Le finalizer) pour libérer le ClassInfo
		lua.PushTable(0, 1);
			lua.PushLightUserdata(info);
			lua.PushCFunction(InfoDestructor, 1);
			lua.SetField("__gc");
		lua.SetMetatable(-2); // La table devient la metatable de l'UserData

		// Maintenant, nous allons associer l'UserData avec chaque fonction, de sorte qu'il reste en vie
		// aussi longtemps que nécessaire, et que le pointeur soit transmis à chaque méthode

		if (!lua.NewMetatable(m_info->name))
			NazaraWarning("Class \"" + m_info->name + "\" already registred in this instance");
		{
			// Set the type in a __type field
			lua.PushString(m_info->name);
			lua.SetField("__type");

			// In case a __tostring method is missing, add a default implementation returning the type
			if (m_methods.find("__tostring") == m_methods.end())
			{
				// Define the Finalizer
				lua.PushValue(1); // shared_ptr on UserData
				lua.PushCFunction(ToStringProxy, 1);
				lua.SetField("__tostring");
			}

			// Define the Finalizer
			lua.PushValue(1);
			lua.PushCFunction(FinalizerProxy, 1);
			lua.SetField("__gc");

			if (m_info->getter || !m_info->parentGetters.empty())
			{
				lua.PushValue(1);  // shared_ptr on UserData
				lua.PushValue(-2); // Metatable
				lua.PushCFunction(GetterProxy, 2);
			}
			else
				// Optimisation, plutôt que de rediriger vers une fonction C qui ne fera rien d'autre que rechercher
				// dans la table, nous envoyons directement la table, de sorte que Lua fasse directement la recherche
				// Ceci n'est possible que si nous n'avons ni getter, ni parent
				lua.PushValue(-1); // Metatable

			lua.SetField("__index"); // Getter

			if (m_info->setter)
			{
				lua.PushValue(1); // shared_ptr on UserData
				lua.PushCFunction(SetterProxy, 1);
				lua.SetField("__newindex"); // Setter
			}

			m_info->methods.reserve(m_methods.size());
			for (auto& pair : m_methods)
			{
				std::size_t methodIndex = m_info->methods.size();
				m_info->methods.push_back(pair.second);

				lua.PushValue(1); // shared_ptr on UserData
				lua.PushInteger(methodIndex);

				lua.PushCFunction(MethodProxy, 2);
				lua.SetField(pair.first); // Method name
			}

			m_info->instanceGetters[m_info->name] = [info = m_info] (LuaInstance& instance)
			{
				return static_cast<T*>(instance.CheckUserdata(1, info->name));
			};
		}
		lua.Pop(); // On pop la metatable

		if (m_info->constructor || m_info->staticGetter || m_info->staticSetter || !m_info->staticMethods.empty())
		{
			// Création de l'instance globale
			lua.PushTable(); // Class = {}

			// Création de la metatable associée à la table globale
			lua.PushTable(); // ClassMeta = {}

			if (m_info->constructor)
			{
				lua.PushValue(1); // ClassInfo
				lua.PushCFunction(ConstructorProxy, 1);
				lua.SetField("__call"); // ClassMeta.__call = ConstructorProxy
			}

			if (m_info->staticGetter)
			{
				lua.PushValue(1);  // shared_ptr on UserData
				lua.PushValue(-2); // ClassMeta
				lua.PushCFunction(StaticGetterProxy, 2);
			}
			else
				// Optimisation, plutôt que de rediriger vers une fonction C qui ne fera rien d'autre que rechercher
				// dans la table, nous envoyons directement la table, de sorte que Lua fasse directement la recherche
				// Ceci n'est possible que si nous n'avons ni getter, ni parent
				lua.PushValue(-1); // ClassMeta

			lua.SetField("__index"); // ClassMeta.__index = StaticGetterProxy/ClassMeta

			if (m_info->staticSetter)
			{
				lua.PushValue(1); // shared_ptr on UserData
				lua.PushCFunction(StaticSetterProxy, 1);
				lua.SetField("__newindex"); // ClassMeta.__newindex = StaticSetterProxy
			}

			m_info->staticMethods.reserve(m_staticMethods.size());
			for (auto& pair : m_staticMethods)
			{
				std::size_t methodIndex = m_info->staticMethods.size();
				m_info->staticMethods.push_back(pair.second);

				lua.PushValue(1); // shared_ptr on UserData
				lua.PushInteger(methodIndex);

				lua.PushCFunction(StaticMethodProxy, 2);
				lua.SetField(pair.first); // ClassMeta.method = StaticMethodProxy
			}

			lua.SetMetatable(-2); // setmetatable(Class, ClassMeta)

			lua.PushValue(-1); // Copie
			lua.SetGlobal(m_info->name); // Class

			m_info->globalTableRef = lua.CreateReference();
		}
		lua.Pop(); // On pop l'Userdata (contenant nos informations)
	}

	template<class T>
	void LuaClass<T>::PushGlobalTable(LuaInstance& lua)
	{
		lua.PushReference(m_info->globalTableRef);
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

		BindMethod(name, [func, handler] (LuaInstance& lua, T& object) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, T>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaInstance& lua, T& object) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...), DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaInstance& lua, T& object) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, object, func);
		});
	}

	template<class T>
	template<typename R, typename P, typename... Args, typename... DefArgs>
	std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value> LuaClass<T>::BindMethod(const String& name, R(P::*func)(Args...) const, DefArgs&&... defArgs)
	{
		typename LuaImplMethodProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		BindMethod(name, [func, handler] (LuaInstance& lua, T& object) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, object, func);
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

		BindStaticMethod(name, [func, handler] (LuaInstance& lua) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, func);
		});
	}

	template<class T>
	void LuaClass<T>::SetStaticSetter(StaticIndexFunc setter)
	{
		m_info->staticSetter = setter;
	}

	template<class T>
	int LuaClass<T>::ConstructorProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		const ConstructorFunc& constructor = info->constructor;

		lua.Remove(1); // On enlève l'argument "table" du stack

		T* instance = static_cast<T*>(lua.PushUserdata(sizeof(T)));

		if (!constructor(lua, instance))
		{
			lua.Error("Constructor failed");
			return 0; // Normalement jamais exécuté (l'erreur provoquant une exception)
		}

		lua.SetMetatable(info->name);
		return 1;
	}

	template<class T>
	int LuaClass<T>::FinalizerProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		const FinalizerFunc& finalizer = info->finalizer;

		T* instance = static_cast<T*>(lua.CheckUserdata(1, info->name));
		lua.Remove(1); //< Remove the instance from the Lua stack

		if (!finalizer || finalizer(lua, *instance))
			instance->~T();

		return 0;
	}

	template<class T>
	int LuaClass<T>::InfoDestructor(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		lua.DestroyReference(info->globalTableRef);

		using namespace std; // Obligatoire pour le destructeur
		info.~shared_ptr(); // Si vous voyez une autre façon de faire, je suis preneur

		return 0;
	}

	template<class T>
	void LuaClass<T>::Get(const std::shared_ptr<ClassInfo>& info, LuaInstance& lua, T* instance)
	{
		const ClassIndexFunc& getter = info->getter;

		if (!getter || !getter(lua, *instance))
		{
			// Query from the metatable
			lua.GetMetatable(info->name); //< Metatable
			lua.PushValue(1); //< Field
			lua.GetTable(); // Metatable[Field]

			lua.Remove(-2); // Remove Metatable

			if (!lua.IsValid(-1))
			{
				for (const ParentFunc& parentGetter : info->parentGetters)
				{
					lua.Pop(); //< Pop the last nil value

					parentGetter(lua, instance);
					if (lua.IsValid(-1))
						return;
				}
			}
		}
	}

	template<class T>
	int LuaClass<T>::GetterProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));

		T* instance = static_cast<T*>(lua.CheckUserdata(1, info->name));
		lua.Remove(1); //< Remove the instance from the Lua stack

		Get(info, lua, instance);
		return 1;
	}

	template<class T>
	int LuaClass<T>::MethodProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));

		T* instance = nullptr;
		if (lua.GetMetatable(1))
		{
			LuaType type = lua.GetField("__type");
			if (type == LuaType_String)
			{
				String name = lua.ToString(-1);
				auto it = info->instanceGetters.find(name);
				if (it != info->instanceGetters.end())
					instance = it->second(lua);
			}
			lua.Pop(2);

			lua.Remove(1); //< Remove the instance from the Lua stack
		}

		if (!instance)
		{
			lua.Error("Method cannot be called without an object");
			return 0;
		}

		unsigned int index = static_cast<unsigned int>(lua.ToInteger(lua.GetIndexOfUpValue(2)));
		const ClassFunc& method = info->methods[index];
		return method(lua, *instance);
	}

	template<class T>
	int LuaClass<T>::SetterProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		const ClassIndexFunc& setter = info->setter;

		T& instance = *static_cast<T*>(lua.CheckUserdata(1, info->name));
		lua.Remove(1); //< Remove the instance from the Lua stack

		if (!setter(lua, instance))
		{
			std::size_t length;
			const char* str = lua.ToString(2, &length);

			lua.Error("Class \"" + info->name + "\" has no field \"" + String(str, length) + "\")");
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::StaticGetterProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		const StaticIndexFunc& getter = info->staticGetter;

		if (!getter(lua))
		{
			// On accède alors à la table
			lua.PushValue(lua.GetIndexOfUpValue(2));
			lua.PushValue(-2);
			lua.GetTable();
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::StaticMethodProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		unsigned int index = static_cast<unsigned int>(lua.ToInteger(lua.GetIndexOfUpValue(2)));
		const StaticFunc& method = info->staticMethods[index];

		return method(lua);
	}

	template<class T>
	int LuaClass<T>::StaticSetterProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
		const StaticIndexFunc& setter = info->staticSetter;

		if (!setter(lua))
		{
			std::size_t length;
			const char* str = lua.ToString(2, &length);

			lua.Error("Class \"" + info->name + "\" has no static field \"" + String(str, length) + ')');
		}

		return 1;
	}

	template<class T>
	int LuaClass<T>::ToStringProxy(lua_State* state)
	{
		LuaInstance& lua = *LuaInstance::GetInstance(state);

		std::shared_ptr<ClassInfo>& info = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));

		lua.PushString(info->name);
		return 1;
	}
}

#include <Nazara/Lua/DebugOff.hpp>
