// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadResourceFromFile(Type* resource, const NzString& filePath, const Parameters& parameters)
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
	auto range = s_fileLoaders.equal_range(ext);
	if (range.first == range.second)
	{
		NazaraError("No loader found for extension \"" + ext + '"');
		return false;
	}

	for (auto it = range.first; it != range.second; ++it)
	{
		// Chargement de la ressource
		if (it->second(resource, filePath, parameters))
			return true;

		NazaraWarning("Loader failed");
	}

	NazaraError("Failed to load file: all loaders failed");

	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadResourceFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
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

	for (auto loader = s_memoryLoaders.begin(); loader != s_memoryLoaders.end(); ++loader)
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
bool NzResourceLoader<Type, Parameters>::LoadResourceFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters)
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
	for (auto loader = s_streamLoaders.begin(); loader != s_streamLoaders.end(); ++loader)
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
void NzResourceLoader<Type, Parameters>::RegisterResourceFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	std::vector<NzString> exts;
	extensions.SplitAny(exts, " /\\*.,;|-_");

	for (const NzString& ext : exts)
		s_fileLoaders.insert(std::make_pair(ext, loadFile));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterResourceMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory)
{
	s_memoryLoaders.push_back(std::make_pair(isLoadingSupported, loadMemory));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterResourceStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream)
{
	s_streamLoaders.push_back(std::make_pair(isLoadingSupported, loadStream));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterResourceFileLoader(const NzString& extensions, LoadFileFunction loadFile)
{
	std::vector<NzString> exts;
	extensions.SplitAny(exts, " /\\*.,;|-_");

	for (const NzString& ext : exts)
	{
		// Récupération de tous les loaders de cette extension
		auto range = s_fileLoaders.equal_range(ext);

		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second == loadFile)
			{
				s_fileLoaders.erase(it);
				break;
			}
		}
	}
		//s_fileLoaders.erase(std::make_pair(ext, loadFile));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterResourceMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory)
{
	s_memoryLoaders.remove(std::make_pair(isLoadingSupported, loadMemory));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterResourceStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream)
{
	s_streamLoaders.remove(std::make_pair(isLoadingSupported, loadStream));
}

template<typename T, typename P> std::list<typename NzResourceLoader<T, P>::MemoryLoader> NzResourceLoader<T, P>::s_memoryLoaders;
template<typename T, typename P> std::list<typename NzResourceLoader<T, P>::StreamLoader> NzResourceLoader<T, P>::s_streamLoaders;
template<typename T, typename P> std::multimap<NzString, typename NzResourceLoader<T, P>::LoadFileFunction> NzResourceLoader<T, P>::s_fileLoaders;

#include <Nazara/Utility/DebugOff.hpp>
