// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ImageStream.hpp>
#include <Nazara/Core/Core.hpp>

namespace Nz
{
	bool ImageStreamParams::IsValid() const
	{
		return true;
	}

	ImageStream::~ImageStream() = default;

	/*!
	* \brief Opens the sound stream from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound stream
	*
	* \remark The file must stay valid until the sound stream is destroyed
	*/
	std::shared_ptr<ImageStream> ImageStream::OpenFromFile(const std::filesystem::path& filePath, const ImageStreamParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetImageStreamLoader().LoadFromFile(filePath, params);
	}

	/*!
	* \brief Opens the sound stream from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the sound stream
	*
	* \remark The memory block must stay valid until the sound stream is destroyed
	*/
	std::shared_ptr<ImageStream> ImageStream::OpenFromMemory(const void* data, std::size_t size, const ImageStreamParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetImageStreamLoader().LoadFromMemory(data, size, params);
	}

	/*!
	* \brief Opens the sound stream from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the sound stream
	* \param params Parameters for the sound stream
	*
	* \remark The stream must stay valid until the sound stream is destroyed
	*/
	std::shared_ptr<ImageStream> ImageStream::OpenFromStream(Stream& stream, const ImageStreamParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssert(core, "Core module has not been initialized");

		return core->GetImageStreamLoader().LoadFromStream(stream, params);
	}
}
