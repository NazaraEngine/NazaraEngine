// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

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
	*
	* \remark Produces a NazaraError if creation went wrong with NAZARA_AUDIO_SAFE defined
	* \remark Produces a std::runtime_error if creation went wrong with NAZARA_AUDIO_SAFE defined
	*/
	SoundBuffer::SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples)
	{
		NazaraAssert(sampleCount > 0, "sample count must be different from zero");
		NazaraAssert(sampleRate > 0, "sample rate must be different from zero");
		NazaraAssert(samples, "invalid samples");

		m_duration = SafeCast<UInt32>((1000ULL*sampleCount / (GetChannelCount(format) * sampleRate)));
		m_format = format;
		m_sampleCount = sampleCount;
		m_sampleRate = sampleRate;
		m_samples = std::make_unique<Int16[]>(sampleCount);
		std::memcpy(&m_samples[0], samples, sampleCount * sizeof(Int16));
	}

	const std::shared_ptr<AudioBuffer>& SoundBuffer::GetBuffer(AudioDevice* device)
	{
		auto it = m_audioBufferByDevice.find(device);
		if (it == m_audioBufferByDevice.end())
		{
			auto audioBuffer = device->CreateBuffer();
			if (!audioBuffer->Reset(m_format, m_sampleCount, m_sampleRate, m_samples.get()))
				throw std::runtime_error("failed to initialize audio buffer");

			it = m_audioBufferByDevice.emplace(device, AudioDeviceEntry{}).first;

			AudioDeviceEntry& entry = it->second;
			entry.audioBuffer = std::move(audioBuffer);
			entry.audioDeviceReleaseSlot.Connect(device->OnAudioDeviceRelease, [this](AudioDevice* device)
			{
				m_audioBufferByDevice.erase(device);
			});
		}

		return it->second.audioBuffer;
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
		NazaraAssert(audio, "Audio module has not been initialized");

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
		NazaraAssert(audio, "Audio module has not been initialized");

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
		NazaraAssert(audio, "Audio module has not been initialized");

		return audio->GetSoundBufferLoader().LoadFromStream(stream, params);
	}
}
