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
	* \class Nz::ResourceSaver
	* \brief Core class that represents a list of saver functions for a specific resource type
	*/

	/*!
	* \brief Checks whether the extension of the file is supported
	* \return true if supported
	*
	* \param extension Extension of the file
	*/
	template<typename Type, typename Parameters>
	bool ResourceSaver<Type, Parameters>::IsFormatSupported(const String& extension)
	{
		for (Saver& saver : Type::s_savers)
		{
			ExtensionGetter isExtensionSupported = std::get<0>(saver);

			if (isExtensionSupported && isExtensionSupported(extension))
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
	bool ResourceSaver<Type, Parameters>::SaveToFile(const Type& resource, const String& filePath, const Parameters& parameters)
	{
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		String path = File::NormalizePath(filePath);
		String ext = path.SubStringFrom('.', -1, true).ToLower();
		if (ext.IsEmpty())
		{
			NazaraError("Failed to get file extension from \"" + filePath + '"');
			return false;
		}

		File file(path); // Opened only is required

		bool found = false;
		for (Saver& saver : Type::s_savers)
		{
			FormatQuerier formatQuerier = std::get<0>(saver);
			if (!formatQuerier || !formatQuerier(ext))
				continue;

			found = true;

			StreamSaver streamSeaver = std::get<1>(saver);
			FileSaver fileSaver = std::get<2>(saver);

			if (fileSaver)
			{
				if (fileSaver(resource, filePath, parameters))
					return true;
			}
			else
			{
				if (!file.Open(OpenMode_WriteOnly | OpenMode_Truncate))
				{
					NazaraError("Failed to save to file: unable to open \"" + filePath + "\" in write mode");
					return false;
				}

				if (streamSeaver(resource, ext, file, parameters))
					return true;
			}

			NazaraWarning("Saver failed");
		}

		if (found)
			NazaraError("Failed to save resource: all savers failed");
		else
			NazaraError("Failed to save resource: no saver found for extension \"" + ext + '"');

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
	bool ResourceSaver<Type, Parameters>::SaveToStream(const Type& resource, Stream& stream, const String& format, const Parameters& parameters)
	{
		NazaraAssert(stream.IsWritable(), "Stream is not writable");
		NazaraAssert(parameters.IsValid(), "Invalid parameters");

		UInt64 streamPos = stream.GetCursorPos();
		bool found = false;
		for (Saver& saver : Type::s_savers)
		{
			FormatQuerier formatQuerier = std::get<0>(saver);
			if (!formatQuerier || !formatQuerier(format))
				continue;

			found = true;

			StreamSaver streamSeaver = std::get<1>(saver);

			// We move the stream to its old position
			stream.SetCursorPos(streamPos);

			// Load of the resource
			if (streamSeaver(resource, format, stream, parameters))
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
	*
	* \param formatQuerier A function to test whether the format (as a string) is supported by this saver
	* \param streamSaver A function which saves the resource to a stream
	* \param fileSaver Optional function which saves the resource directly to a file given a file path
	*
	* \remark The fileSaver argument is only present for compatibility with external savers which cannot be interfaced with streams
	* \remark At least one saver is required
	*/
	template<typename Type, typename Parameters>
	void ResourceSaver<Type, Parameters>::RegisterSaver(FormatQuerier formatQuerier, StreamSaver streamSaver, FileSaver fileSaver)
	{
		NazaraAssert(formatQuerier, "A format querier is mandaroty");
		NazaraAssert(streamSaver || fileSaver, "A saver function is mandaroty");

		Type::s_savers.push_front(std::make_tuple(formatQuerier, streamSaver, fileSaver));
	}

	/*!
	* \brief Unregisters a saver
	*
	* \param formatQuerier A function to test whether the format (as a string) is supported by this saver
	* \param streamSaver A function which saves the resource to a stream
	* \param fileSaver A function function which saves the resource directly to a file given a file path
	*
	* \remark The saver will only be unregistered if the function pointers are exactly the same
	*/
	template<typename Type, typename Parameters>
	void ResourceSaver<Type, Parameters>::UnregisterSaver(FormatQuerier formatQuerier, StreamSaver streamSaver, FileSaver fileSaver)
	{
		Type::s_savers.remove(std::make_tuple(formatQuerier, streamSaver, fileSaver));
	}
}

#include <Nazara/Core/DebugOff.hpp>
