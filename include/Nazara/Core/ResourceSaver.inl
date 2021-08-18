// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
	* \class Nz::ResourceSaver
	* \brief Core class that represents a list of saver functions for a specific resource type
	*/

	/*!
	* \brief Unregister every saver registered
	*/
	template<typename Type, typename Parameters>
	void ResourceSaver<Type, Parameters>::Clear()
	{
		m_savers.clear();
	}

	/*!
	* \brief Checks whether the extension of the file is supported
	* \return true if supported
	*
	* \param extension Extension of the file
	*/
	template<typename Type, typename Parameters>
	bool ResourceSaver<Type, Parameters>::IsExtensionSupported(const std::string_view& extension) const
	{
		for (const auto& saverPtr : m_savers)
		{
			const Entry& saver = *saverPtr;
			if (saver.formatSupport(extension))
				return true;
		}

		return false;
	}

	/*!
	* \brief Saves a resource to a file
	* \return true if successfully saved
	*
	* \param resource Resource to save
	* \param filePath Path to the file where the resource will be written
	* \param parameters Parameters for the save
	*
	* \remark The previous file content will be discarded, to prevent this behavior you should use SaveToStream
	* \remark The file extension will be used as format for the saver ("image.png" => "png", to write a specified format to a user-specified extension you should use SaveToStream
	*
	* \see SaveToStream
	*/
	template<typename Type, typename Parameters>
	bool ResourceSaver<Type, Parameters>::SaveToFile(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters) const
	{
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		std::string extension = ToLower(filePath.extension().generic_u8string());
		if (extension.empty())
		{
			NazaraError("Failed to get file extension from \"" + filePath.generic_u8string() + '"');
			return false;
		}

		bool found = false;
		for (const auto& saverPtr : m_savers)
		{
			const Entry& saver = *saverPtr;
			if (!saver.formatSupport(extension))
				continue;

			found = true;

			if (saver.fileSaver)
			{
				if (saver.fileSaver(resource, filePath, parameters))
					return true;
			}
			else
			{
				File file(filePath.generic_u8string());

				if (!file.Open(OpenMode::WriteOnly | OpenMode::Truncate))
				{
					NazaraError("Failed to save to file: unable to open \"" + filePath.generic_u8string() + "\" in write mode");
					return false;
				}

				if (saver.streamSaver(resource, extension, file, parameters))
					return true;
			}

			NazaraWarning("Saver failed");
		}

		if (found)
			NazaraError("Failed to save resource: all savers failed");
		else
			NazaraError("Failed to save resource: no saver found for extension \"" + extension + '"');

		return false;
	}

	/*!
	* \brief Saves a resource to a stream
	* \return true if successfully saved
	*
	* \param resource Resource to load
	* \param stream Stream with write access where the resource data will be written
	* \param format Data format to save the resource to
	* \param parameters Parameters for the saving
	*
	* \see SaveToFile
	*/
	template<typename Type, typename Parameters>
	bool ResourceSaver<Type, Parameters>::SaveToStream(const Type& resource, Stream& stream, const std::string& format, const Parameters& parameters) const
	{
		NazaraAssert(stream.IsWritable(), "Stream is not writable");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (const auto& saverPtr : m_savers)
		{
			const Entry& saver = *saverPtr;
			if (!saver.formatSupport(format))
				continue;

			found = true;

			// We move the stream to its old position
			stream.SetCursorPos(streamPos);

			// Load of the resource
			if (saver.streamSaver(resource, format, stream, parameters))
				return true;

			NazaraWarning("Saver failed");
		}

		if (found)
			NazaraError("Failed to save resource: all savers failed");
		else
			NazaraError("Failed to save resource: no saver found for format \"" + format + '"');

		return false;
	}

	/*!
	* \brief Registers a saver
	* \return A pointer to the registered Entry which can be unsed to unregister it later
	*
	* \param loader A collection of saver callbacks that will be registered
	*
	* \see UnregisterLoader
	*/
	template<typename Type, typename Parameters>
	auto ResourceSaver<Type, Parameters>::RegisterSaver(Entry saver) -> const Entry*
	{
		NazaraAssert(saver.formatSupport, "A format support callback is mandatory");
		NazaraAssert(saver.streamSaver || saver.fileSaver, "A saver function is mandatory");

		auto it = m_savers.emplace(m_savers.begin(), std::make_unique<Entry>(std::move(saver)));
		return it->get();
	}

	/*!
	* \brief Unregisters a saver
	*
	* \param saver A pointer to a loader returned by RegisterSaver
	*
	* \see RegisterSaver
	*/
	template<typename Type, typename Parameters>
	void ResourceSaver<Type, Parameters>::UnregisterSaver(const Entry* saver)
	{
		auto it = std::find_if(m_savers.begin(), m_savers.end(), [&](const std::unique_ptr<Entry>& saverPtr) { return saverPtr.get() == saver; });
		if (it != m_savers.end())
			m_savers.erase(it);
	}
}

#include <Nazara/Core/DebugOff.hpp>
