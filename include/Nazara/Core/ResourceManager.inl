// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>

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
	ResourceManager<Type, Parameters>::ResourceManager(Loader& loader) :
	m_loader(loader)
	{
	}

	/*!
	* \brief Clears the content of the manager
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Clear()
	{
		m_resources.clear();
	}

	/*!
	* \brief Gets a reference to the object loaded from file
	* \return Reference to the object
	*
	* \param filePath Path to the asset that will be loaded
	*/
	template<typename Type, typename Parameters>
	std::shared_ptr<Type> ResourceManager<Type, Parameters>::Get(const std::filesystem::path& filePath)
	{
		std::filesystem::path absolutePath = std::filesystem::canonical(filePath);
		auto it = m_resources.find(absolutePath);
		if (it == m_resources.end())
		{
			std::shared_ptr<Type> resource = m_loader.LoadFromFile(absolutePath, GetDefaultParameters());
			if (!resource)
			{
				NazaraError("failed to load resource from file: {0}", absolutePath);
				return std::shared_ptr<Type>();
			}

			NazaraDebug("loaded resource from file {0}", absolutePath);

			it = m_resources.insert(std::make_pair(absolutePath, resource)).first;
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
		return m_defaultParameters;
	}

	/*!
	* \brief Registers the resource under the filePath
	*
	* \param filePath Path for the resource
	* \param resource Object to associate with
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Register(const std::filesystem::path& filePath, std::shared_ptr<Type> resource)
	{
		std::filesystem::path absolutePath = std::filesystem::canonical(filePath);

		m_resources[absolutePath] = resource;
	}

	/*!
	* \brief Sets the defaults parameters for the load
	*
	* \param params Default parameters for loading from file
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::SetDefaultParameters(Parameters params)
	{
		m_defaultParameters = std::move(params);
	}

	/*!
	* \brief Unregisters the resource under the filePath
	*
	* \param filePath Path for the resource
	*/
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Unregister(const std::filesystem::path& filePath)
	{
		std::filesystem::path absolutePath = std::filesystem::canonical(filePath);

		m_resources.erase(absolutePath);
	}
}
