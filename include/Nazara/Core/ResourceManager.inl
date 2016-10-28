// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ResourceManager
	* \brief Core class that represents a resource manager
	*/

	/*!
	* \brief Clears the content of the manager
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Clear()
	{
		Type::s_managerMap.clear();
	}

	/*!
	* \brief Gets a reference to the object loaded from file
	* \return Reference to the object
	*
	* \param filePath Path to the asset that will be loaded
	*/
	template<typename Type, typename Parameters>
	ObjectRef<Type> ResourceManager<Type, Parameters>::Get(const String& filePath)
	{
		String absolutePath = File::AbsolutePath(filePath);
		auto it = Type::s_managerMap.find(absolutePath);
		if (it == Type::s_managerMap.end())
		{
			ObjectRef<Type> resource = Type::New();
			if (!resource)
			{
				NazaraError("Failed to create resource");
				return ObjectRef<Type>();
			}

			if (!resource->LoadFromFile(absolutePath, GetDefaultParameters()))
			{
				NazaraError("Failed to load resource from file: " + absolutePath);
				return ObjectRef<Type>();
			}

			NazaraDebug("Loaded resource from file " + absolutePath);

			it = Type::s_managerMap.insert(std::make_pair(absolutePath, resource)).first;
		}

		return it->second;
	}

	/*!
	* \brief Gets the defaults parameters for the load
	* \return Default parameters for loading from file
	*/
	template<typename Type, typename Parameters>
	const Parameters& ResourceManager<Type, Parameters>::GetDefaultParameters()
	{
		return Type::s_managerParameters;
	}

	/*!
	* \brief Purges the resource manager from every asset whose it is the only owner
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Purge()
	{
		auto it = Type::s_managerMap.begin();
		while (it != Type::s_managerMap.end())
		{
			const ObjectRef<Type>& ref = it->second;
			if (ref->GetReferenceCount() == 1) // Are we the only ones to own the resource ?
			{
				NazaraDebug("Purging resource from file " + ref->GetFilePath());
				Type::s_managerMap.erase(it++); // Then we erase it
			}
			else
				++it;
		}
	}

	/*!
	* \brief Registers the resource under the filePath
	*
	* \param filePath Path for the resource
	* \param resource Object to associate with
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Register(const String& filePath, ObjectRef<Type> resource)
	{
		String absolutePath = File::AbsolutePath(filePath);

		Type::s_managerMap[absolutePath] = resource;
	}

	/*!
	* \brief Sets the defaults parameters for the load
	*
	* \param params Default parameters for loading from file
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::SetDefaultParameters(const Parameters& params)
	{
		Type::s_managerParameters = params;
	}

	/*!
	* \brief Unregisters the resource under the filePath
	*
	* \param filePath Path for the resource
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Unregister(const String& filePath)
	{
		String absolutePath = File::AbsolutePath(filePath);

		Type::s_managerMap.erase(absolutePath);
	}

	/*!
	* \brief Initializes the resource manager
	* \return true
	*/
	template<typename Type, typename Parameters>
	bool ResourceManager<Type, Parameters>::Initialize()
	{
		return true;
	}

	/*!
	* \brief Uninitialize the resource manager
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Uninitialize()
	{
		Clear();
	}
}

#include <Nazara/Core/DebugOff.hpp>
