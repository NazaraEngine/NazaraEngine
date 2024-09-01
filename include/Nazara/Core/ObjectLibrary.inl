// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>

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
	std::shared_ptr<Type> ObjectLibrary<Type>::Get(std::string_view name) const
	{
		std::shared_ptr<Type> ref = Query(name);
		if (!ref)
			NazaraErrorFmt("Object \"{}\" is not present", name);

		return ref;
	}

	/*!
	* \brief Checks whether the library has the object with that name
	* \return true if it the case
	*/
	template<typename Type>
	bool ObjectLibrary<Type>::Has(std::string_view name) const
	{
		return m_library.contains(name);
	}

	/*!
	* \brief Registers the std::shared_ptr object with that name
	*
	* \param name Name of the object
	* \param object Object to stock
	*/
	template<typename Type>
	void ObjectLibrary<Type>::Register(std::string name, std::shared_ptr<Type> object)
	{
		m_library.emplace(std::move(name), object);
	}

	/*!
	* \brief Gets the std::shared_ptr object by name
	* \return Optional reference
	*
	* \param name Name of the object
	*/
	template<typename Type>
	std::shared_ptr<Type> ObjectLibrary<Type>::Query(std::string_view name) const
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
	void ObjectLibrary<Type>::Unregister(std::string_view name)
	{
		m_library.erase(name);
	}
}

