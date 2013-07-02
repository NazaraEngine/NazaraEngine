// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
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
	// PS: L'utilisation d'un shared_ptr permet de garder la structure en vie même si l'instance est libérée avant le LuaClass
	void* info = lua.PushUserdata(sizeof(std::shared_ptr<ClassInfo>));
	new (info) std::shared_ptr<ClassInfo>(m_info);

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
			lua.PushCFunction(GetterProxy, 1);
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

		m_info->methods.resize(m_methods.size());

		unsigned int index = 0;
		for (auto& pair : m_methods)
		{
			m_info->methods[index] = pair.second;

			lua.PushValue(1);
			lua.PushInteger(index);

			lua.PushCFunction(MethodProxy, 2);
			lua.SetField(pair.first); // Méthode

			index++;
		}
	}
	lua.Pop();

	if (m_info->constructor)
	{
		lua.PushValue(1);
		lua.PushCFunction(ConstructorProxy, 1);
		lua.SetGlobal(m_info->name);
	}
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
int NzLuaClass<T>::ConstructorProxy(lua_State* state)
{
	NzLuaInstance& lua = *NzLuaInstance::GetInstance(state);

	ClassInfo* info = *static_cast<ClassInfo**>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));
	ConstructorFunc constructor = info->constructor;

	T* instance = constructor(lua);
	if (!instance)
	{
		lua.Error("Constructor failed");
		return 0; // Normalement pas nécessaire
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

	std::shared_ptr<ClassInfo>* infoPtr = static_cast<std::shared_ptr<ClassInfo>*>(lua.ToUserdata(lua.GetIndexOfUpValue(1)));

	using namespace std; // Obligatoire pour le destructeur
	infoPtr->~shared_ptr(); // Si vous voyez une autre façon de faire, je suis preneur

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
		lua.PushMetatable(info->name);
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
		lua.Error("Field not found");

	return 1;
}

#include <Nazara/Lua/DebugOff.hpp>
