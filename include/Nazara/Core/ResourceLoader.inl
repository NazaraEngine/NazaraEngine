// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters)
{
	#if NAZARA_CORE_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid parameters");
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

	NzFile file(path, NzFile::ReadOnly);
	if (!file.IsOpen())
	{
		NazaraError("Failed to open \"" + path + '"');
		return false;
	}

	for (auto loader = Type::s_loaders.begin(); loader != Type::s_loaders.end(); ++loader)
	{
		for (const NzString& loaderExt : std::get<0>(*loader))
		{
			int cmp = NzString::Compare(loaderExt, ext);
			if (cmp == 0)
			{
				file.SetCursorPos(0);

				if (!std::get<1>(*loader)(file, parameters))
					continue;

				file.SetCursorPos(0);

				// Chargement de la ressource
				if (std::get<2>(*loader)(resource, file, parameters))
					return true;

				NazaraWarning("Loader failed");
			}
			else if (cmp < 0) // S'il est encore possible que l'extension se situe après
				continue;

			break;
		}
	}

	NazaraError("Failed to load file: no loader");

	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
{
	NzMemoryStream stream(data, size);

	return LoadFromStream(resource, stream, parameters);
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters)
{
	#if NAZARA_CORE_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid parameters");
		return false;
	}

	if (stream.GetSize() == 0 || stream.GetCursorPos() >= stream.GetSize())
	{
		NazaraError("No data to load");
		return false;
	}
	#endif

	nzUInt64 streamPos = stream.GetCursorPos();
	for (auto loader = Type::s_loaders.begin(); loader != Type::s_loaders.end(); ++loader)
	{
		stream.SetCursorPos(streamPos);

		// Le loader supporte-t-il les données ?
		if (!std::get<1>(*loader)(stream, parameters))
			continue;

		// On repositionne le stream à son ancienne position
		stream.SetCursorPos(streamPos);

		// Chargement de la ressource
		if (std::get<2>(*loader)(resource, stream, parameters))
			return true;

		NazaraWarning("Loader failed");
	}

	NazaraError("Failed to load file: no loader");
	return false;
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterLoader(const NzString& fileExtensions, CheckFunction checkFunc, LoadFunction loadFunc)
{
	///FIXME: Trouver une alternative à ce code monstrueux
	std::vector<NzString> exts;
	fileExtensions.SplitAny(exts, " /\\.,;|-_");

	std::set<NzString> extensions;
	std::copy(exts.begin(), exts.end(), std::inserter(extensions, extensions.begin()));

	Type::s_loaders.push_front(std::make_tuple(std::move(extensions), checkFunc, loadFunc));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterLoader(const NzString& fileExtensions, CheckFunction checkFunc, LoadFunction loadFunc)
{
	std::vector<NzString> exts;
	fileExtensions.SplitAny(exts, " /\\.,;|-_");

	std::set<NzString> extensions;
	std::copy(exts.begin(), exts.end(), std::inserter(extensions, extensions.begin()));

	Type::s_loaders.remove(std::make_tuple(std::move(extensions), checkFunc, loadFunc));
}

#include <Nazara/Core/DebugOff.hpp>
