// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Clear()
	{
		Type::s_managerMap.clear();
	}

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

	template<typename Type, typename Parameters>
	const Parameters& ResourceManager<Type, Parameters>::GetDefaultParameters()
	{
		return Type::s_managerParameters;
	}

	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Purge()
	{
		auto it = Type::s_managerMap.begin();
		while (it != Type::s_managerMap.end())
		{
			const ObjectRef<Type>& ref = it->second;
			if (ref.GetReferenceCount() == 1) // Sommes-nous les seuls à détenir la ressource ?
			{
				NazaraDebug("Purging resource from file " + ref->GetFilePath());
				Type::s_managerMap.erase(it++); // Alors on la supprime
			}
			else
				++it;
		}
	}

	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Register(const String& filePath, ObjectRef<Type> resource)
	{
		String absolutePath = File::AbsolutePath(filePath);

		Type::s_managerMap[absolutePath] = resource;
	}

	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::SetDefaultParameters(const Parameters& params)
	{
		Type::s_managerParameters = params;
	}

	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Unregister(const String& filePath)
	{
		String absolutePath = File::AbsolutePath(filePath);

		Type::s_managerMap.erase(absolutePath);
	}

	template<typename Type, typename Parameters>
	bool ResourceManager<Type, Parameters>::Initialize()
	{
		return true;
	}

	template<typename Type, typename Parameters>
	void ResourceManager<Type, Parameters>::Uninitialize()
	{
		Clear();
	}
}

#include <Nazara/Core/DebugOff.hpp>
