// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ResourceLoader
	* \brief Core class that represents a loader of resources
	*/

	/*!
	* \brief Unregister every loader registered
	*/
	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::Clear()
	{
		m_loaders.clear();
	}

	/*!
	* \brief Checks whether the extension of the file is supported
	* \return true if supported
	*
	* \param extension Extension of the file (ex: "png")
	*/
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::IsExtensionSupported(const std::string_view& extension) const
	{
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			if (loader.extensionSupport && loader.extensionSupport(extension))
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
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromFile(const std::filesystem::path& filePath, const Parameters& parameters) const
	{
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		std::string ext = ToLower(PathToString(filePath.extension()));
		if (ext.empty())
		{
			NazaraError("Failed to get file extension from \"" + PathToString(filePath) + '"');
			return nullptr;
		}

		if (ext[0] == '.')
			ext.erase(ext.begin());

		File file(filePath); // Open only if needed

		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			if (loader.extensionSupport && !loader.extensionSupport(ext))
				continue;

			if (loader.streamChecker && !file.IsOpen())
			{
				if (!file.Open(OpenMode::ReadOnly))
				{
					NazaraError("Failed to load file: unable to open \"" + PathToString(filePath) + '"');
					return nullptr;
				}
			}

			Ternary recognized = Ternary::Unknown;
			if (loader.fileLoader)
			{
				if (loader.streamChecker)
				{
					file.SetCursorPos(0);

					recognized = loader.streamChecker(file, parameters);
					if (recognized == Ternary::False)
						continue;
					else
						found = true;
				}
				else
				{
					recognized = Ternary::Unknown;
					found = true;
				}

				std::shared_ptr<Type> resource = loader.fileLoader(filePath, parameters);
				if (resource)
				{
					resource->SetFilePath(filePath);
					return resource;
				}
			}
			else
			{
				assert(loader.streamChecker);

				file.SetCursorPos(0);

				recognized = loader.streamChecker(file, parameters);
				if (recognized == Ternary::False)
					continue;
				else if (recognized == Ternary::True)
					found = true;

				file.SetCursorPos(0);

				std::shared_ptr<Type> resource = loader.streamLoader(file, parameters);
				if (resource)
				{
					resource->SetFilePath(filePath);
					return resource;
				}
			}

			if (recognized == Ternary::True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found for extension \"" + ext + '"');

		return nullptr;
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
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromMemory(const void* data, std::size_t size, const Parameters& parameters) const
	{
		NazaraAssert(data, "Invalid data pointer");
		NazaraAssert(size, "No data to load");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		MemoryView stream(data, size);

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			Ternary recognized = Ternary::Unknown;
			if (loader.memoryLoader)
			{
				if (loader.streamChecker)
				{
					stream.SetCursorPos(streamPos);

					recognized = loader.streamChecker(stream, parameters);
					if (recognized == Ternary::False)
						continue;
					else
						found = true;
				}
				else
				{
					recognized = Ternary::Unknown;
					found = true;
				}

				std::shared_ptr<Type> resource = loader.memoryLoader(data, size, parameters);
				if (resource)
					return resource;
			}
			else
			{
				stream.SetCursorPos(streamPos);

				recognized = loader.streamChecker(stream, parameters);
				if (recognized == Ternary::False)
					continue;
				else if (recognized == Ternary::True)
					found = true;

				stream.SetCursorPos(streamPos);

				std::shared_ptr<Type> resource = loader.streamLoader(stream, parameters);
				if (resource)
					return resource;
			}

			if (recognized == Ternary::True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found");

		return nullptr;
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
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromStream(Stream& stream, const Parameters& parameters) const
	{
		NazaraAssert(stream.GetCursorPos() < stream.GetSize(), "No data to load");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			stream.SetCursorPos(streamPos);

			// Does the loader support these data ?
			Ternary recognized = loader.streamChecker(stream, parameters);
			if (recognized == Ternary::False)
				continue;
			else if (recognized == Ternary::True)
				found = true;

			// We move the stream to its old position
			stream.SetCursorPos(streamPos);

			// Load of the resource
			std::shared_ptr<Type> resource = loader.streamLoader(stream, parameters);
			if (resource)
				return resource;

			if (recognized == Ternary::True)
				NazaraWarning("Loader failed");
		}

		if (found)
			NazaraError("Failed to load file: all loaders failed");
		else
			NazaraError("Failed to load file: no loader found");

		return nullptr;
	}

	/*!
	* \brief Registers a loader
	* \return A pointer to the registered Entry which can be unsed to unregister it later
	*
	* \param loader A collection of loader callbacks that will be registered
	*
	* \see UnregisterLoader
	*/
	template<typename Type, typename Parameters>
	auto ResourceLoader<Type, Parameters>::RegisterLoader(Entry loader) -> const Entry*
	{
		NazaraAssert(loader.streamChecker || !loader.streamLoader, "StreamLoader present without StreamChecker");
		NazaraAssert(loader.fileLoader || loader.memoryLoader || loader.streamLoader, "A loader function is mandatory");

		auto it = m_loaders.emplace(m_loaders.begin(), std::make_unique<Entry>(std::move(loader)));
		return it->get();
	}

	/*!
	* \brief Unregisters a loader
	*
	* \param loader A pointer to a loader returned by RegisterLoad
	*
	* \see RegisterLoader
	*/
	template<typename Type, typename Parameters>
	void ResourceLoader<Type, Parameters>::UnregisterLoader(const Entry* loader)
	{
		auto it = std::find_if(m_loaders.begin(), m_loaders.end(), [&](const std::unique_ptr<Entry>& loaderPtr) { return loaderPtr.get() == loader; });
		if (it != m_loaders.end())
			m_loaders.erase(it);
	}
}

#include <Nazara/Core/DebugOff.hpp>
