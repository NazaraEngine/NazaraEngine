// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ObjectRef
	* \brief Core class that represents a reference to an object
	*/

	/*!
	* \brief Gets the ObjectRef object by name
	* \return Optional reference
	*
	* \param name Name of the object
	*
	* \remark Produces a NazaraError if object not found
	*/
	template<typename Type>
	ObjectRef<Type> ObjectLibrary<Type>::Get(const String& name)
	{
		ObjectRef<Type> ref = Query(name);
		if (!ref)
			NazaraError("Object \"" + name + "\" is not present");

		return ref;
	}

	/*!
	* \brief Checks whether the library has the object with that name
	* \return true if it the case
	*/
	template<typename Type>
	bool ObjectLibrary<Type>::Has(const String& name)
	{
		return Type::s_library.find(name) != Type::s_library.end();
	}

	/*!
	* \brief Registers the ObjectRef object with that name
	*
	* \param name Name of the object
	* \param object Object to stock
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Register(const String& name, ObjectRef<Type> object)
	{
		Type::s_library.emplace(name, object);
	}

	/*!
	* \brief Gets the ObjectRef object by name
	* \return Optional reference
	*
	* \param name Name of the object
	*/
	template<typename Type>
	ObjectRef<Type> ObjectLibrary<Type>::Query(const String& name)
	{
		auto it = Type::s_library.find(name);
		if (it != Type::s_library.end())
			return it->second;
		else
			return nullptr;
	}

	/*!
	* \brief Unregisters the ObjectRef object with that name
	*
	* \param name Name of the object
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Unregister(const String& name)
	{
		Type::s_library.erase(name);
	}

	template<typename Type>
	bool ObjectLibrary<Type>::Initialize()
	{
		return true; // Nothing to do
	}

	template<typename Type>
	void ObjectLibrary<Type>::Uninitialize()
	{
		Type::s_library.clear();
	}
}

#include <Nazara/Core/DebugOff.hpp>
