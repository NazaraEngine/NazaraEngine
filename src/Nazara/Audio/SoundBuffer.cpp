// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::SoundBuffer
	* \brief Audio class that represents a buffer for sound
	*
	* \remark Module Audio needs to be initialized to use this class
	*/

	/*!
	* \brief Checks whether the parameters for the buffer' sound are correct
	* \return true If parameters are valid
	*/

	bool SoundBufferParams::IsValid() const
	{
		return true;
	}

	/*!
	* \brief Constructs a SoundBuffer object
	*
	* \param format Format for the audio
	* \param sampleCount Number of samples
	* \param sampleRate Rate of samples
	* \param samples Samples raw data
	*/
	SoundBuffer::SoundBuffer(AudioFormat format, std::span<const AudioChannel> channels, UInt64 frameCount, UInt32 sampleRate, const void* samples) :
	m_channels(channels.begin(), channels.end())
	{
		NazaraAssertMsg(!m_channels.empty(), "channel map cannot be empty");
		NazaraAssertMsg(frameCount > 0, "frameCount must be different from zero");
		NazaraAssertMsg(sampleRate > 0, "sampleRate must be different from zero");

		m_duration = Time::Microseconds((1'000'000LL * frameCount / sampleRate));
		m_format = format;
		m_frameCount = frameCount;
		m_sampleRate = sampleRate;

		std::size_t bufferSize = frameCount * s_AudioFormatSize[format] * channels.size();
		m_samples = std::make_unique_for_overwrite<UInt8[]>(bufferSize);
		if (samples)
			std::memcpy(&m_samples[0], samples, bufferSize);
	}

	void SoundBuffer::ConvertFormat(AudioFormat format, AudioDitherMode ditherMode)
	{
		if (m_format == format)
			return;

		std::unique_ptr<UInt8[]> convertedSamples = std::make_unique_for_overwrite<UInt8[]>(m_frameCount * s_AudioFormatSize[format] * m_channels.size());
		ConvertAudioFormat(m_format, m_samples.get(), format, convertedSamples.get(), m_frameCount, ditherMode);

		m_samples = std::move(convertedSamples);
		m_format = format;
	}

	auto SoundBuffer::Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) -> Result<ReadData, std::string>
	{
		NazaraAssert(startingFrameIndex <= m_frameCount);
		frameCount = std::min(m_frameCount - startingFrameIndex, frameCount);
		if (frameCount > 0)
		{
			std::size_t frameSize = s_AudioFormatSize[m_format] * m_channels.size();
			std::memcpy(frameOut, &m_samples[startingFrameIndex * frameSize], frameCount * frameSize);
		}

		return Ok(ReadData{ frameCount, startingFrameIndex + frameCount });
	}

	/*!
	* \brief Loads the sound buffer from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound buffer
	*/
	std::shared_ptr<SoundBuffer> SoundBuffer::LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params)
	{
		Audio* audio = Audio::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundBufferLoader().LoadFromFile(filePath, params);
	}

	/*!
	* \brief Loads the sound buffer from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the sound buffer
	*/
	std::shared_ptr<SoundBuffer> SoundBuffer::LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params)
	{
		Audio* audio = Audio::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundBufferLoader().LoadFromMemory(data, size, params);
	}

	/*!
	* \brief Loads the sound buffer from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the sound buffer
	* \param params Parameters for the sound buffer
	*/
	std::shared_ptr<SoundBuffer> SoundBuffer::LoadFromStream(Stream& stream, const SoundBufferParams& params)
	{
		Audio* audio = Audio::Instance();
		NazaraAssertMsg(audio, "Audio module has not been initialized");

		return audio->GetSoundBufferLoader().LoadFromStream(stream, params);
	}
}
