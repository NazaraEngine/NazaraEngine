// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/SoundStream.hpp>
#include <Nazara/Audio2/Audio2.hpp>

namespace Nz
{
	bool SoundStreamParams::IsValid() const
	{
		return true;
	}

	/*!
	* \ingroup audio
	* \class Nz::SoundStream
	* \brief Audio class that represents a sound stream
	*
	* \remark This class is abstract
	*/
	SoundStream::~SoundStream() = default;

	/*!
	* \brief Opens the sound stream from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound stream
	*
	* \remark The file must stay valid until the sound stream is destroyed
	*/
	std::shared_ptr<SoundStream> SoundStream::OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params)
	{
		Audio2* audio = Audio2::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundStreamLoader().LoadFromFile(filePath, params);
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
	std::shared_ptr<SoundStream> SoundStream::OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params)
	{
		Audio2* audio = Audio2::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundStreamLoader().LoadFromMemory(data, size, params);
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
	std::shared_ptr<SoundStream> SoundStream::OpenFromStream(Stream& stream, const SoundStreamParams& params)
	{
		Audio2* audio = Audio2::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundStreamLoader().LoadFromStream(stream, params);
	}
}
