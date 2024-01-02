// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/PathUtils.hpp>
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
	* \param extension Extension of the file (ex: ".png")
	*/
	template<typename Type, typename Parameters>
	bool ResourceLoader<Type, Parameters>::IsExtensionSupported(std::string_view extension) const
	{
		NazaraAssert(extension.size() >= 2 || extension.front() != '.', "extension should start with a .");

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
	* \return loaded resources or null pointer if failed
	*
	* \param filePath Path to the resource
	* \param parameters Parameters for the load
	*
	* \ret Loaded resources
	*/
	template<typename Type, typename Parameters>
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromFile(const std::filesystem::path& filePath, const Parameters& parameters) const
	{
		NazaraAssert(parameters.IsValid(), "invalid parameters");

		std::string ext = ToLower(PathToString(filePath.extension()));
		if (ext.empty())
		{
			NazaraErrorFmt("failed to get file extension from \"{0}\"", filePath);
			return nullptr;
		}

		File file(filePath); // Open only if needed

		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			if (loader.extensionSupport && !loader.extensionSupport(ext))
				continue;

			if (loader.parameterFilter && !loader.parameterFilter(parameters))
				continue;

			Result<std::shared_ptr<Type>, ResourceLoadingError> result = Nz::Err(ResourceLoadingError::Unrecognized);

			if (loader.fileLoader)
				result = loader.fileLoader(filePath, parameters);
			else if (loader.streamLoader)
			{
				if (!file.IsOpen())
				{
					if (!file.Open(OpenMode::Read))
					{
						NazaraErrorFmt("failed to load resource: unable to open \"{0}\"", filePath);
						return nullptr;
					}
				}
				else
					file.SetCursorPos(0);

				result = loader.streamLoader(file, parameters);
			}

			if (!result)
			{
				ResourceLoadingError error = result.GetError();
				if (error != ResourceLoadingError::Unrecognized)
					NazaraError("failed to load resource: loader failed");
				else
					found = true;

				continue;
			}

			std::shared_ptr<Type> resource = std::move(result).GetValue();
			resource->SetFilePath(filePath);

			return resource;
		}

		if (found)
			NazaraErrorFmt("failed to load resource from file \"{0}\": all loaders failed", filePath);
		else
			NazaraErrorFmt("failed to load resource from file \"{0}\": no loader found for extension \"{1}\"", filePath, ext);

		return nullptr;
	}

	/*!
	* \brief Loads a resource from a raw memory, a size and parameters
	* \return loaded resources or null pointer if failed
	*
	* \param data Raw memory of the resource
	* \param size Size available for the read
	* \param parameters Parameters for the load
	*/
	template<typename Type, typename Parameters>
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromMemory(const void* data, std::size_t size, const Parameters& parameters) const
	{
		NazaraAssert(data, "invalid data pointer");
		NazaraAssert(size, "no data to load");
		NazaraAssert(parameters.IsValid(), "invalid parameters");

		MemoryView stream(data, size);

		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			if (loader.parameterFilter && !loader.parameterFilter(parameters))
				continue;

			Result<std::shared_ptr<Type>, ResourceLoadingError> result = Nz::Err(ResourceLoadingError::Unrecognized);

			if (loader.memoryLoader)
				result = loader.memoryLoader(data, size, parameters);
			else if (loader.streamLoader)
			{
				stream.SetCursorPos(0);

				result = loader.streamLoader(stream, parameters);
			}

			if (!result)
			{
				ResourceLoadingError error = result.GetError();
				if (error != ResourceLoadingError::Unrecognized)
					NazaraError("failed to load resource: loader failed");
				else
					found = true;

				continue;
			}

			return std::move(result).GetValue();
		}

		if (found)
			NazaraError("failed to load resource from memory: all loaders failed");
		else
			NazaraError("failed to load resource from memory: no loader found");

		return nullptr;
	}

	/*!
	* \brief Loads a resource from a stream and parameters
	* \return loaded resources or null pointer if failed
	*
	* \param stream Stream of the resource
	* \param parameters Parameters for the load
	*/
	template<typename Type, typename Parameters>
	std::shared_ptr<Type> ResourceLoader<Type, Parameters>::LoadFromStream(Stream& stream, const Parameters& parameters) const
	{
		NazaraAssert(stream.GetCursorPos() < stream.GetSize(), "no data to load");
		NazaraAssert(parameters.IsValid(), "invalid parameters");

		// Retrieve extension from stream (if any)
		std::string ext = ToLower(PathToString(stream.GetPath().extension()));

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (auto& loaderPtr : m_loaders)
		{
			const Entry& loader = *loaderPtr;

			if (loader.parameterFilter && !loader.parameterFilter(parameters))
				continue;

			if (loader.extensionSupport && !ext.empty() && !loader.extensionSupport(ext))
				continue;

			stream.SetCursorPos(streamPos);

			// Load resource
			Result<std::shared_ptr<Type>, ResourceLoadingError> result = loader.streamLoader(stream, parameters);
			if (!result)
			{
				ResourceLoadingError error = result.GetError();
				if (error != ResourceLoadingError::Unrecognized)
				{
					NazaraError("failed to load resource: loader failed");
					found = true;
				}

				continue;
			}

			return std::move(result).GetValue();
		}

		if (found)
			NazaraError("failed to load resource from stream: all loaders failed");
		else
			NazaraError("failed to load resource from from stream: no loader found");

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
