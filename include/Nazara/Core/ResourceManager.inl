// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename Type, typename Parameters>
void NzResourceManager<Type, Parameters>::Clear()
{
	Type::s_managerMap.clear();
}

template<typename Type, typename Parameters>
NzObjectRef<Type> NzResourceManager<Type, Parameters>::Get(const NzString& filePath)
{
	NzString absolutePath = NzFile::AbsolutePath(filePath);
	auto it = Type::s_managerMap.find(absolutePath);
	if (it == Type::s_managerMap.end())
	{
		NzObjectRef<Type> resource = Type::New();
		if (!resource)
		{
			NazaraError("Failed to create resource");
			return NzObjectRef<Type>();
		}

		if (!resource->LoadFromFile(absolutePath, GetDefaultParameters()))
		{
			NazaraError("Failed to load resource from file: " + absolutePath);
			return NzObjectRef<Type>();
		}

		NazaraDebug("Loaded resource from file " + absolutePath);

		it = Type::s_managerMap.insert(std::make_pair(absolutePath, resource)).first;
	}

	return it->second;
}

template<typename Type, typename Parameters>
const Parameters& NzResourceManager<Type, Parameters>::GetDefaultParameters()
{
	return Type::s_managerParameters;
}

template<typename Type, typename Parameters>
void NzResourceManager<Type, Parameters>::Purge()
{
	auto it = Type::s_managerMap.begin();
	while (it != Type::s_managerMap.end())
	{
		const NzObjectRef<Type>& ref = it->second;
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
void NzResourceManager<Type, Parameters>::Register(const NzString& filePath, NzObjectRef<Type> resource)
{
	NzString absolutePath = NzFile::AbsolutePath(filePath);

	Type::s_managerMap[absolutePath] = resource;
}

template<typename Type, typename Parameters>
void NzResourceManager<Type, Parameters>::SetDefaultParameters(const Parameters& params)
{
	Type::s_managerParameters = params;
}

template<typename Type, typename Parameters>
void NzResourceManager<Type, Parameters>::Unregister(const NzString& filePath)
{
	NzString absolutePath = NzFile::AbsolutePath(filePath);

	Type::s_managerMap.erase(absolutePath);
}

template<typename Type, typename Parameters>
bool NzResourceManager<Type, Parameters>::Initialize()
{
	return true;
}

template<typename Type, typename Parameters>
void NzResourceManager<Type, Parameters>::Uninitialize()
{
	Clear();
}

#include <Nazara/Core/DebugOff.hpp>
