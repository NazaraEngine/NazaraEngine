// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters)
{
	#if NAZARA_UTILITY_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid Parameters");
		return false;
	}
	#endif

	NzString path = NzFile::NormalizePath(filePath);
	NzString ext = path.SubstrFrom('.', -1, true);
	if (ext.IsEmpty())
	{
		NazaraError("Failed to get file extension");
		return false;
	}

	// Récupération de tous les loaders de cette extension
	auto range = Type::s_fileLoaders.equal_range(ext);
	if (range.first == range.second)
	{
		NazaraError("No loader found for extension \"" + ext + '"');
		return false;
	}

	// range.second est un itérateur vers l'élément après le dernier élémént du range
	auto it = range.second;

	do
	{
		it--;

		// Chargement de la ressource
		if (it->second(resource, filePath, parameters))
			return true;

		NazaraWarning("Loader failed");
	}
	while (it != range.first);

	NazaraError("Failed to load file: all loaders failed");

	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
{
	#if NAZARA_UTILITY_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid Parameters");
		return false;
	}

	if (!data || size == 0)
	{
		NazaraError("No data to load");
		return false;
	}
	#endif

	for (auto loader = Type::s_memoryLoaders.rbegin(); loader != Type::s_memoryLoaders.rend(); ++loader)
	{
		// Le loader supporte-t-il les données ?
		if (!loader->first(data, size, parameters))
			continue;

		// Chargement de la ressource
		if (loader->second(resource, data, size, parameters))
			return true;

		NazaraWarning("Loader failed");
	}

	NazaraError("Failed to load file: all loaders failed");
	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters)
{
	#if NAZARA_UTILITY_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid Parameters");
		return false;
	}

	if (stream.GetSize() == 0 || stream.GetCursorPos() >= stream.GetSize())
	{
		NazaraError("No data to load");
		return false;
	}
	#endif

	nzUInt64 streamPos = stream.GetCursorPos();
	for (auto loader = Type::s_streamLoaders.rbegin(); loader != Type::s_streamLoaders.rend(); ++loader)
	{
		stream.SetCursorPos(streamPos);

		// Le loader supporte-t-il les données ?
		if (!loader->first(stream, parameters))
			continue;

		stream.SetCursorPos(streamPos);

		// Chargement de la ressource
		if (loader->second(resource, stream, parameters))
			return true;

		NazaraWarning("Loader failed");
	}

	NazaraError("Failed to load file: all loaders failed");
	return false;
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	std::vector<NzString> exts;
	extensions.SplitAny(exts, " /\\*.,;|-_");

	for (const NzString& ext : exts)
		Type::s_fileLoaders.insert(std::make_pair(ext, loadFile));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterMemoryLoader(IdentifyMemoryFunction identifyMemory, LoadMemoryFunction loadMemory)
{
	Type::s_memoryLoaders.push_back(std::make_pair(identifyMemory, loadMemory));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterStreamLoader(IdentifyStreamFunction identifyStream, LoadStreamFunction loadStream)
{
	Type::s_streamLoaders.push_back(std::make_pair(identifyStream, loadStream));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	std::vector<NzString> exts;
	extensions.SplitAny(exts, " /\\*.,;|-_");

	for (const NzString& ext : exts)
	{
		// Récupération de tous les loaders de cette extension
		auto range = Type::s_fileLoaders.equal_range(ext);

		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second == loadFile)
			{
				Type::s_fileLoaders.erase(it);
				break;
			}
		}
	}
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterMemoryLoader(IdentifyMemoryFunction identifyMemory, LoadMemoryFunction loadMemory)
{
	Type::s_memoryLoaders.remove(std::make_pair(identifyMemory, loadMemory));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterStreamLoader(IdentifyStreamFunction identifyStream, LoadStreamFunction loadStream)
{
	Type::s_streamLoaders.remove(std::make_pair(identifyStream, loadStream));
}

#include <Nazara/Utility/DebugOff.hpp>
