// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDBUFFER_HPP
#define NAZARA_AUDIO_SOUNDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/Asset.hpp>
#include <Nazara/Core/AssetLoader.hpp>
#include <Nazara/Core/AssetManager.hpp>
#include <Nazara/Core/AssetParameters.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Time.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	struct SoundBufferParams : AssetParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

	class AudioBuffer;
	class AudioDevice;
	class Sound;
	class SoundBuffer;

	using SoundBufferLibrary = ObjectLibrary<SoundBuffer>;
	using SoundBufferLoader = AssetLoader<SoundBuffer, SoundBufferParams>;
	using SoundBufferManager = AssetManager<SoundBuffer, SoundBufferParams>;

	struct SoundBufferImpl;

	class NAZARA_AUDIO_API SoundBuffer : public Asset
	{
		friend Sound;

		public:
			using Params = SoundBufferParams;

			SoundBuffer() = default;
			SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			SoundBuffer(const SoundBuffer&) = delete;
			SoundBuffer(SoundBuffer&&) = delete;
			~SoundBuffer() = default;

			const std::shared_ptr<AudioBuffer>& GetAudioBuffer(AudioDevice* device);

			inline Time GetDuration() const;
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
			Time m_duration;
			UInt32 m_sampleRate;
			UInt64 m_sampleCount;
	};
}

#include <Nazara/Audio/SoundBuffer.inl>

#endif // NAZARA_AUDIO_SOUNDBUFFER_HPP
