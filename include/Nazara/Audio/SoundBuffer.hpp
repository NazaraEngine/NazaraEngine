// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDBUFFER_HPP
#define NAZARA_AUDIO_SOUNDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	struct SoundBufferParams : ResourceParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

	class AudioBuffer;
	class AudioDevice;
	class Sound;
	class SoundBuffer;

	using SoundBufferLibrary = ObjectLibrary<SoundBuffer>;
	using SoundBufferLoader = ResourceLoader<SoundBuffer, SoundBufferParams>;
	using SoundBufferManager = ResourceManager<SoundBuffer, SoundBufferParams>;

	struct SoundBufferImpl;

	class NAZARA_AUDIO_API SoundBuffer : public Resource
	{
		friend Sound;

		public:
			SoundBuffer() = default;
			SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			SoundBuffer(const SoundBuffer&) = delete;
			SoundBuffer(SoundBuffer&&) = delete;
			~SoundBuffer() = default;

			const std::shared_ptr<AudioBuffer>& GetBuffer(AudioDevice* device);

			inline UInt32 GetDuration() const;
			inline AudioFormat GetFormat() const;
			inline const Int16* GetSamples() const;
			inline UInt64 GetSampleCount() const;
			inline UInt32 GetSampleRate() const;

			SoundBuffer& operator=(const SoundBuffer&) = delete;
			SoundBuffer& operator=(SoundBuffer&&) = delete;

			static std::shared_ptr<SoundBuffer> LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

		private:
			struct AudioDeviceEntry
			{
				std::shared_ptr<AudioBuffer> audioBuffer;

				NazaraSlot(AudioDevice, OnAudioDeviceRelease, audioDeviceReleaseSlot);
			};

			std::unordered_map<AudioDevice*, AudioDeviceEntry> m_audioBufferByDevice;
			std::unique_ptr<Int16[]> m_samples;
			AudioFormat m_format;
			UInt32 m_duration;
			UInt32 m_sampleRate;
			UInt64 m_sampleCount;
	};
}

#include <Nazara/Audio/SoundBuffer.inl>

#endif // NAZARA_AUDIO_SOUNDBUFFER_HPP
