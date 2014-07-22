// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <iostream>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Lua/Debug.hpp>

template<class T>
NzLuaClass<T>::NzLuaClass(const NzString& name) :
m_info(new ClassInfo)
{
	m_info->name = name;
}
/*
template<class T>
void NzLuaClass<T>::Inherit(NzLuaClass<P>& parent)
{
	static_assert(std::is_base_of<P, T>::value, "P must be a base of T");

	m_info->parentInfo = parent.m_info;
}
*/
template<class T>
void NzLuaClass<T>::Register(NzLuaInstance& lua)
{
	// Le ClassInfo doit rester en vie jusqu'à la fin du script
	// Obliger l'instance de LuaClass à rester en vie dans cette fin serait contraignant pour l'utilisateur
	// J'utilise donc une astuce, la stocker dans une UserData associée avec chaque fonction de la metatable du type,
	// cette UserData disposera d'un finalizer qui libérera le ClassInfo
	// Ainsi c'est Lua qui va s'occuper de la destruction pour nous :-)
	// De même, l'utilisation d'un shared_ptr permet de garder la structure en vie même si l'instance est libérée avant le LuaClass
	void* info = lua.PushUserdata(sizeof(std::shared_ptr<ClassInfo>));
	NzPlacementNew<std::shared_ptr<ClassInfo>>(info, m_info);

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
		lua.PushValue(1); // On associe l'UserData avec la fonction
		lua.PushCFunction(FinalizerProxy, 1);
		lua.SetField("__gc"); // Finalizer

		if (m_info->getter)
		{
			lua.PushValue(1);
			lua.PushValue(-2);
			lua.PushCFunction(GetterProxy, 2);
		}
		else
			// Optimisation, plutôt que de rediriger vers une fonction C qui ne fera rien d'autre que rechercher
			// dans la table, nous envoyons directement la table, de sorte que Lua fasse directement la recherche
			// Ceci n'est possible que si nous n'avons ni getter, ni parent
			lua.PushValue(-1);

		lua.SetField("__index"); // Getter

		if (m_info->setter)
		{
			lua.PushValue(1);
			lua.PushCFunction(SetterProxy, 1);
			lua.SetField("__newindex"); // Setter
		}

		m_info->methods.reserve(m_methods.size());
		for (auto& pair : m_methods)
		{
			m_info->methods.push_back(pair.second);

			lua.PushValue(1);
			lua.PushInteger(m_info->methods.size() - 1);

			lua.PushCFunction(MethodProxy, 2);
			lua.SetField(pair.first); // Méthode
		}
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
			lua.PushValue(1);
			lua.PushValue(-2);
			lua.PushCFunction(StaticGetterProxy, 2);
		}
		else
			// Optimisation, plutôt que de rediriger vers une fonction C qui ne fera rien d'autre que rechercher
			// dans la table, nous envoyons directement la table, de sorte que Lua fasse directement la recherche
			// Ceci n'est possible que si nous n'avons ni getter, ni parent
			lua.PushValue(-1);

		lua.SetField("__index"); // ClassMeta.__index = StaticGetterProxy/ClassMeta

		if (m_info->staticSetter)
		{
			lua.PushValue(1);
			lua.PushCFunction(StaticSetterProxy, 1);
			lua.SetField("__newindex"); // ClassMeta.__newindex = StaticSetterProxy
		}

		m_info->staticMethods.reserve(m_staticMethods.size());
		for (auto& pair : m_staticMethods)
		{
			m_info->staticMethods.push_back(pair.second);

			lua.PushValue(1);
			lua.PushInteger(m_info->staticMethods.size() - 1);

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
void NzLuaClass<T>::PushGlobalTable(NzLuaInstance& lua)
{
	lua.PushReference(m_info->globalTableRef);
}

template<class T>
void NzLuaClass<T>::SetConstructor(ConstructorFunc constructor)
{
	m_info->constructor = constructor;
}

template<class T>
void NzLuaClass<T>::SetFinalizer(FinalizerFunc finalizer)
{
	m_info->finalizer = finalizer;
}

template<class T>
void NzLuaClass<T>::SetGetter(ClassIndexFunc getter)
{
	m_info->getter = getter;
}

template<class T>
void NzLuaClass<T>::SetMethod(const NzString& name, ClassFunc method)
{
	m_methods[name] = method;
}

template<class T>
void NzLuaClass<T>::SetSetter(ClassIndexFunc setter)
{
	m_info->setter = setter;
}

template<class T>
void NzLuaClass<T>::SetStaticGetter(StaticIndexFunc getter)
{
	m_info->staticGetter = getter;
}

template<class T>
void NzLuaClass<T>::SetStaticMethod(const NzString& name, StaticFunc method)
{
	m_staticMethods[name] = method;
}

template<class T>
void NzLuaClass<T>::SetStaticSetter(StaticIndexFunc setter)
{
	m_info->staticSetter = setter;
}

template<class T>
int NzLuaClass<T>::ConstructorProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	ConstructorFunc constructor = info->constructor;

	lua.Remove(1); // On enlève l'argument "table" du stack

	T* instance = constructor(lua);
	if (!instance)
	{
		lua.Error("Constructor failed");
		return 0; // Normalement jamais exécuté (l'erreur provoquant une exception)
	}

	T** ud = static_cast<T**>(lua.PushUserdata(sizeof(T*)));
	*ud = instance;
	lua.SetMetatable(info->name);

	return 1;
}

template<class T>
int NzLuaClass<T>::FinalizerProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	FinalizerFunc finalizer = info->finalizer;

	T* instance = *static_cast<T**>(lua.CheckUserdata(1, info->name));

	if (!finalizer || finalizer(lua, *instance))
		delete instance;

	return 0;
}

template<class T>
int NzLuaClass<T>::InfoDestructor(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	std::shared_ptr<ClassInfo>& infoPtr = *static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	lua.DestroyReference(infoPtr->globalTableRef);

	using namespace std; // Obligatoire pour le destructeur
	infoPtr.~shared_ptr(); // Si vous voyez une autre façon de faire, je suis preneur

	return 0;
}

template<class T>
int NzLuaClass<T>::GetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	ClassIndexFunc getter = info->getter;

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, info->name)));

	if (!getter(lua, instance))
	{
		// On accède alors à la table
		lua.PushValue(lua.GetIndexOfUpValue(2));
		lua.PushValue(-2);
		lua.GetTable();
	}

	return 1;
}

template<class T>
int NzLuaClass<T>::MethodProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	int index = lua.ToInteger(lua.GetIndexOfUpValue(2));
	ClassFunc method = info->methods[index];

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, info->name)));

	lua.Remove(1); // On enlève l'argument "userdata" du stack

	return method(lua, instance);
}

template<class T>
int NzLuaClass<T>::SetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	ClassIndexFunc setter = info->setter;

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, info->name)));

	if (!setter(lua, instance))
	{
		std::size_t length;
		const char* str = lua.ToString(2, &length);

		lua.Error("Class \"" + info->name + "\" has no field \"" + NzString(str, length) + ')');
	}

	return 1;
}

template<class T>
int NzLuaClass<T>::StaticGetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	StaticIndexFunc getter = info->staticGetter;

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
int NzLuaClass<T>::StaticMethodProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	int index = lua.ToInteger(lua.GetIndexOfUpValue(2));
	StaticFunc method = info->staticMethods[index];

	return method(lua);
}

template<class T>
int NzLuaClass<T>::StaticSetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	StaticIndexFunc setter = info->staticSetter;

	if (!setter(lua))
	{
		std::size_t length;
		const char* str = lua.ToString(2, &length);

		lua.Error("Class \"" + info->name + "\" has no static field \"" + NzString(str, length) + ')');
	}

	return 1;
}

#include <Nazara/Lua/DebugOff.hpp>
