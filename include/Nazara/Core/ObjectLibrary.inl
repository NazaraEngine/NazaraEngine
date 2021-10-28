// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ObjectLibrary
	* \brief Core class containing a collection of objects
	*/

	/*!
	* \brief Clears the library, freeing every object it contains
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Clear()
	{
		m_library.clear();
	}

	/*!
	* \brief Gets the std::shared_ptr object by name
	* \return Optional reference
	*
	* \param name Name of the object
	*
	* \remark Produces a NazaraError if object not found
	*/
	template<typename Type>
	std::shared_ptr<Type> ObjectLibrary<Type>::Get(const std::string& name)
	{
		std::shared_ptr<Type> ref = Query(name);
		if (!ref)
			NazaraError("Object \"" + name + "\" is not present");

		return ref;
	}

	/*!
	* \brief Checks whether the library has the object with that name
	* \return true if it the case
	*/
	template<typename Type>
	bool ObjectLibrary<Type>::Has(const std::string& name)
	{
		return m_library.find(name) != m_library.end();
	}

	/*!
	* \brief Registers the std::shared_ptr object with that name
	*
	* \param name Name of the object
	* \param object Object to stock
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Register(const std::string& name, std::shared_ptr<Type> object)
	{
		m_library.emplace(name, object);
	}

	/*!
	* \brief Gets the std::shared_ptr object by name
	* \return Optional reference
	*
	* \param name Name of the object
	*/
	template<typename Type>
	std::shared_ptr<Type> ObjectLibrary<Type>::Query(const std::string& name)
	{
		auto it = m_library.find(name);
		if (it != m_library.end())
			return it->second;
		else
			return nullptr;
	}

	/*!
	* \brief Unregisters the std::shared_ptr object with that name
	*
	* \param name Name of the object
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Unregister(const std::string& name)
	{
		m_library.erase(name);
	}
}

#include <Nazara/Core/DebugOff.hpp>
