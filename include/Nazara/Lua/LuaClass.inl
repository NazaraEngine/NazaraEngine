// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/Debug.hpp>
#include <Nazara/Core/Error.hpp>

template<class T>
NzLuaClass<T>::NzLuaClass(const NzString& name) :
m_getter(nullptr),
m_setter(nullptr),
m_constructor(nullptr),
m_finalizer(nullptr),
m_name(name)
{
}

template<class T>
void NzLuaClass<T>::Register(NzLuaInstance& lua)
{
	if (!lua.NewMetatable(m_name))
		NazaraWarning("Class \"" + m_name + "\" already registred in this instance");
	{
		FinalizerFunc* finalizer = static_cast<FinalizerFunc*>(lua.PushUserdata(sizeof(FinalizerFunc)));
		*finalizer = m_finalizer;
		lua.PushString(m_name);

		lua.PushCFunction(FinalizerProxy, 2);
		lua.SetField("__gc");

		if (m_getter)
		{
			ClassIndexFunc* getter = static_cast<ClassIndexFunc*>(lua.PushUserdata(sizeof(ClassIndexFunc)));
			*getter = m_getter;
			lua.PushString(m_name);

			lua.PushCFunction(GetterProxy, 2);
		}
		else
			lua.PushValue(-1);

		lua.SetField("__index");

		if (m_setter)
		{
			ClassIndexFunc* setter = static_cast<ClassIndexFunc*>(lua.PushUserdata(sizeof(ClassIndexFunc)));
			*setter = m_setter;
			lua.PushString(m_name);

			lua.PushCFunction(SetterProxy, 2);
			lua.SetField("__newindex");
		}

		for (auto it = m_methods.begin(); it != m_methods.end(); ++it)
		{
			ClassFunc* method = static_cast<ClassFunc*>(lua.PushUserdata(sizeof(ClassFunc)));
			*method = it->second;
			lua.PushString(m_name);

			lua.PushCFunction(MethodProxy, 2);
			lua.SetField(it->first);
		}

	}
	lua.Pop();

	if (m_constructor)
	{
		ConstructorFunc* ptr = static_cast<ConstructorFunc*>(lua.PushUserdata(sizeof(ConstructorFunc)));
		*ptr = m_constructor;
		lua.PushString(m_name);

		lua.PushCFunction(ConstructorProxy, 2);
		lua.SetGlobal(m_name);
	}
}

template<class T>
void NzLuaClass<T>::SetConstructor(ConstructorFunc constructor)
{
	m_constructor = constructor;
}

template<class T>
void NzLuaClass<T>::SetFinalizer(FinalizerFunc finalizer)
{
	m_finalizer = finalizer;
}

template<class T>
void NzLuaClass<T>::SetGetter(ClassIndexFunc getter)
{
	m_getter = getter;
}

template<class T>
void NzLuaClass<T>::SetMethod(const NzString& name, ClassFunc method)
{
	m_methods[name] = method;
}

template<class T>
void NzLuaClass<T>::SetSetter(ClassIndexFunc setter)
{
	m_setter = setter;
}

template<class T>
int NzLuaClass<T>::ConstructorProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ConstructorFunc func = *static_cast<ConstructorFunc*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	const char* className = lua.ToString(lua.GetIndexOfUpValue(2));

	T* instance = func(lua);
	if (!instance)
	{
		lua.Error("Constructor failed");
		return 0; // Normalement pas nécessaire
	}

	T** ud = static_cast<T**>(lua.PushUserdata(sizeof(T*)));
	lua.SetMetatable(className);
	*ud = instance;

	return 1;
}

template<class T>
int NzLuaClass<T>::FinalizerProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	FinalizerFunc func = *static_cast<FinalizerFunc*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	const char* className = lua.ToString(lua.GetIndexOfUpValue(2));

	T* instance = *static_cast<T**>(lua.CheckUserdata(1, className));
	lua.Remove(1);

	if (!func || func(lua, *instance))
		delete instance;

	return 0;
}

template<class T>
int NzLuaClass<T>::GetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassIndexFunc func = *static_cast<ClassIndexFunc*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	const char* className = lua.ToString(lua.GetIndexOfUpValue(2));

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, className)));
	lua.Remove(1);

	if (!func(lua, instance))
	{
		// On accède alors à la table
		lua.PushMetatable(className);
		lua.PushValue(1);
		lua.GetTable();
	}

	return 1;
}

template<class T>
int NzLuaClass<T>::MethodProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassFunc func = *static_cast<ClassFunc*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	const char* className = lua.ToString(lua.GetIndexOfUpValue(2));

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, className)));
	lua.Remove(1);

	return (*func)(lua, instance);
}

template<class T>
int NzLuaClass<T>::SetterProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassIndexFunc func = *static_cast<ClassIndexFunc*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	const char* className = lua.ToString(lua.GetIndexOfUpValue(2));

	T& instance = *(*static_cast<T**>(lua.CheckUserdata(1, className)));
	lua.Remove(1);

	if (!func(lua, instance))
		lua.Error("Field not found");

	return 1;
}

#include <Nazara/Lua/DebugOff.hpp>
