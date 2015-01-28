// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename Type>
NzObjectRef<Type> NzObjectLibrary<Type>::Get(const NzString& name)
{
	NzObjectRef<Type> ref = Query(name);
	if (!ref)
		NazaraError("Object \"" + name + "\" is not present");

	return ref;
}

template<typename Type>
bool NzObjectLibrary<Type>::Has(const NzString& name)
{
	return Type::s_library.find(name) != Type::s_library.end();
}

template<typename Type>
void NzObjectLibrary<Type>::Register(const NzString& name, NzObjectRef<Type> object)
{
	Type::s_library.emplace(name, object);
}

template<typename Type>
NzObjectRef<Type> NzObjectLibrary<Type>::Query(const NzString& name)
{
	auto it = Type::s_library.find(name);
	if (it != Type::s_library.end())
		return it->second;
	else
		return nullptr;
}

template<typename Type>
void NzObjectLibrary<Type>::Unregister(const NzString& name)
{
	Type::s_library.erase(name);
}

template<typename Type>
bool NzObjectLibrary<Type>::Initialize()
{
	return true; // Que faire
}

template<typename Type>
void NzObjectLibrary<Type>::Uninitialize()
{
	Type::s_library.clear();
}

#include <Nazara/Core/DebugOff.hpp>
