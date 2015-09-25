// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename Type>
	ObjectRef<Type> ObjectLibrary<Type>::Get(const String& name)
	{
		ObjectRef<Type> ref = Query(name);
		if (!ref)
			NazaraError("Object \"" + name + "\" is not present");

		return ref;
	}

	template<typename Type>
	bool ObjectLibrary<Type>::Has(const String& name)
	{
		return Type::s_library.find(name) != Type::s_library.end();
	}

	template<typename Type>
	void ObjectLibrary<Type>::Register(const String& name, ObjectRef<Type> object)
	{
		Type::s_library.emplace(name, object);
	}

	template<typename Type>
	ObjectRef<Type> ObjectLibrary<Type>::Query(const String& name)
	{
		auto it = Type::s_library.find(name);
		if (it != Type::s_library.end())
			return it->second;
		else
			return nullptr;
	}

	template<typename Type>
	void ObjectLibrary<Type>::Unregister(const String& name)
	{
		Type::s_library.erase(name);
	}

	template<typename Type>
	bool ObjectLibrary<Type>::Initialize()
	{
		return true; // Que faire
	}

	template<typename Type>
	void ObjectLibrary<Type>::Uninitialize()
	{
		Type::s_library.clear();
	}
}

#include <Nazara/Core/DebugOff.hpp>
