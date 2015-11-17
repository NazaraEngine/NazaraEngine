// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::IsExtensionSupported(const String& extension)
	{
		for (Loader& loader : Type::s_loaders)
		{
			ExtensionGetter isExtensionSupported = std::get<0>(loader);

			if (isExtensionSupported && isExtensionSupported(extension))
				return true;
		}

		return false;
	}

	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromFile(Type* resource, const String& filePath, const Parameters& parameters)
	{
		#if NAZARA_CORE_SAFE
		if (!parameters.IsValid())
		{
			NazaraError("Invalid parameters");
			return false;
		}
		#endif

		String path = File::NormalizePath(filePath);
		String ext = path.SubStringFrom('.', -1, true).ToLower();
		if (ext.IsEmpty())
		{
			NazaraError("Failed to get file extension from \"" + filePath + '"');
			return false;
		}

		File file(path); // Ouvert seulement en cas de besoin

		bool found = false;
		for (Loader& loader : Type::s_loaders)
		{
			ExtensionGetter isExtensionSupported = std::get<0>(loader);
			if (!isExtensionSupported || !isExtensionSupported(ext))
				continue;

			StreamChecker checkFunc = std::get<1>(loader);
			StreamLoader streamLoader = std::get<2>(loader);
			FileLoader fileLoader = std::get<3>(loader);

			if (checkFunc && !file.IsOpen())
			{
				if (!file.Open(OpenMode_ReadOnly))
				{
					NazaraError("Failed to load file: unable to open \"" + filePath + '"');
					return false;
				}
			}

			Ternary recognized = Ternary_Unknown;
			if (fileLoader)
			{
				if (checkFunc)
				{
					file.SetCursorPos(0);

					recognized = checkFunc(file, parameters);
					if (recognized == Ternary_False)
						continue;
					else
						found = true;
				}
				else
				{
					recognized = Ternary_Unknown;
					found = true;
				}

				if (fileLoader(resource, filePath, parameters))
				{
					resource->SetFilePath(filePath);
					return true;
				}
			}
			else
			{
				file.SetCursorPos(0);

				recognized = checkFunc(file, parameters);
				if (recognized == Ternary_False)
					continue;
				else if (recognized == Ternary_True)
					found = true;

				file.SetCursorPos(0);

				if (streamLoader(resource, file, parameters))
				{
					resource->SetFilePath(filePath);
					return true;
				}
			}

			if (recognized == Ternary_True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found for extension \"" + ext + '"');

		return false;
	}

	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
	{
		#if NAZARA_CORE_SAFE
		if (!parameters.IsValid())
		{
			NazaraError("Invalid parameters");
			return false;
		}

		if (size == 0)
		{
			NazaraError("No data to load");
			return false;
		}
		#endif

		MemoryView stream(data, size);

		bool found = false;
		for (Loader& loader : Type::s_loaders)
		{
			StreamChecker checkFunc = std::get<1>(loader);
			StreamLoader streamLoader = std::get<2>(loader);
			MemoryLoader memoryLoader = std::get<4>(loader);

			Ternary recognized = Ternary_Unknown;
			if (memoryLoader)
			{
				if (checkFunc)
				{
					stream.SetCursorPos(0);

					recognized = checkFunc(stream, parameters);
					if (recognized == Ternary_False)
						continue;
					else
						found = true;
				}
				else
				{
					recognized = Ternary_Unknown;
					found = true;
				}

				if (memoryLoader(resource, data, size, parameters))
					return true;
			}
			else
			{
				stream.SetCursorPos(0);

				recognized = checkFunc(stream, parameters);
				if (recognized == Ternary_False)
					continue;
				else if (recognized == Ternary_True)
					found = true;

				stream.SetCursorPos(0);

				if (streamLoader(resource, stream, parameters))
					return true;
			}

			if (recognized == Ternary_True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found");

		return false;
	}

	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromStream(Type* resource, InputStream& stream, const Parameters& parameters)
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

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (Loader& loader : Type::s_loaders)
		{
			StreamChecker checkFunc = std::get<1>(loader);
			StreamLoader streamLoader = std::get<2>(loader);

			stream.SetCursorPos(streamPos);

			// Le loader supporte-t-il les données ?
			Ternary recognized = checkFunc(stream, parameters);
			if (recognized == Ternary_False)
				continue;
			else if (recognized == Ternary_True)
				found = true;

			// On repositionne le stream à son ancienne position
			stream.SetCursorPos(streamPos);

			// Chargement de la ressource
			if (streamLoader(resource, stream, parameters))
				return true;

			if (recognized == Ternary_True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found");

		return false;
	}

	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::RegisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader, MemoryLoader memoryLoader)
	{
		#if NAZARA_CORE_SAFE
		if (streamLoader)
		{
			if (!checkFunc)
			{
				NazaraError("StreamLoader present without StreamChecker");
				return;
			}
		}
		else if (!fileLoader && !memoryLoader)
		{
			NazaraError("Neither FileLoader nor MemoryLoader nor StreamLoader were present");
			return;
		}
		#endif

		Type::s_loaders.push_front(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader, memoryLoader));
	}

	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::UnregisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader, MemoryLoader memoryLoader)
	{
		Type::s_loaders.remove(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader, memoryLoader));
	}
}

#include <Nazara/Core/DebugOff.hpp>
