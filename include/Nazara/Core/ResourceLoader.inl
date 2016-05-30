// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ResourceLoader
	* \brief Core class that represents a loader of resources
	*/

	/*!
	* \brief Checks whether the extension of the file is supported
	* \return true if supported
	*
	* \param extension Extension of the file
	*/
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

	/*!
	* \brief Loads a resource from a file
	* \return true if successfully loaded
	*
	* \param resource Resource to load
	* \param filePath Path to the resource
	* \param parameters Parameters for the load
	*
	* \remark Produces a NazaraError if resource is nullptr with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if parameters are invalid with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if filePath has no extension
	* \remark Produces a NazaraError if file count not be opened
	* \remark Produces a NazaraWarning if loader failed
	* \remark Produces a NazaraError if all loaders failed or no loader was found
	*/
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromFile(Type* resource, const String& filePath, const Parameters& parameters)
	{
		NazaraAssert(resource, "Invalid resource");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		String path = File::NormalizePath(filePath);
		String ext = path.SubStringFrom('.', -1, true).ToLower();
		if (ext.IsEmpty())
		{
			NazaraError("Failed to get file extension from \"" + filePath + '"');
			return false;
		}

		File file(path); // Open only if needed

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

	/*!
	* \brief Loads a resource from a raw memory, a size and parameters
	* \return true if successfully loaded
	*
	* \param resource Resource to load
	* \param data Raw memory of the resource
	* \param size Size available for the read
	* \param parameters Parameters for the load
	*
	* \remark Produces a NazaraError if resource is nullptr with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if size is 0 with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if parameters are invalid with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraWarning if loader failed
	* \remark Produces a NazaraError if all loaders failed or no loader was found
	*/
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
	{
		NazaraAssert(resource, "Invalid resource");
		NazaraAssert(data, "Invalid data pointer");
		NazaraAssert(size, "No data to load");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

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

	/*!
	* \brief Loads a resource from a stream and parameters
	* \return true if successfully loaded
	*
	* \param resource Resource to load
	* \param stream Stream of the resource
	* \param parameters Parameters for the load
	*
	* \remark Produces a NazaraError if resource is nullptr with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if stream has no data with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraError if parameters are invalid with NAZARA_CORE_SAFE defined
	* \remark Produces a NazaraWarning if loader failed
	* \remark Produces a NazaraError if all loaders failed or no loader was found
	*/
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::LoadFromStream(Type* resource, Stream& stream, const Parameters& parameters)
	{
		NazaraAssert(resource, "Invalid resource");
		NazaraAssert(stream.GetCursorPos() < stream.GetSize(), "No data to load");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (Loader& loader : Type::s_loaders)
		{
			StreamChecker checkFunc = std::get<1>(loader);
			StreamLoader streamLoader = std::get<2>(loader);

			stream.SetCursorPos(streamPos);

			// Does the loader support these data ?
			Ternary recognized = checkFunc(stream, parameters);
			if (recognized == Ternary_False)
				continue;
			else if (recognized == Ternary_True)
				found = true;

			// We move the stream to its old position
			stream.SetCursorPos(streamPos);

			// Load of the resource
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

	/*!
	* \brief Registers the loader
	*
	* \param extensionGetter A function to test whether the extension (as a string) is supported by this loader
	* \param checkFunc A function to check the stream with the parser
	* \param streamLoader A function to load the data from a stream in the resource
	* \param fileLoader Optional function to load the data from a file in the resource
	* \param memoryLoader Optional function to load the data from a raw memory in the resource
	*/
	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::RegisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader, MemoryLoader memoryLoader)
	{
		NazaraAssert(checkFunc || !streamLoader, "StreamLoader present without StreamChecker");
		NazaraAssert(fileLoader || memoryLoader || streamLoader, "A loader function is mandatory");

		Type::s_loaders.push_front(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader, memoryLoader));
	}

	/*!
	* \brief Unregisters the loader
	*
	* \param extensionGetter A function to test whether the extension (as a string) is supported by this loader
	* \param checkFunc A function to check the stream with the parser
	* \param streamLoader A function to load the data from a stream in the resource
	* \param fileLoader Optional function to load the data from a file in the resource
	* \param memoryLoader Optional function to load the data from a raw memory in the resource
	*/
	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::UnregisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader, MemoryLoader memoryLoader)
	{
		Type::s_loaders.remove(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader, memoryLoader));
	}
}

#include <Nazara/Core/DebugOff.hpp>
