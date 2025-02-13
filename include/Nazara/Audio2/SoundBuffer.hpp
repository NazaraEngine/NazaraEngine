// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_SOUNDBUFFER_HPP
#define NAZARA_AUDIO2_SOUNDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <span>
#include <unordered_map>
#include <vector>

namespace Nz
{
	struct SoundBufferParams : ResourceParameters
	{
		AudioFormat format = AudioFormat::Signed16;
		bool mixToMono = false;

		bool IsValid() const;
	};

	class Sound;
	class SoundBuffer;

	using SoundBufferLibrary = ObjectLibrary<SoundBuffer>;
	using SoundBufferLoader = ResourceLoader<SoundBuffer, SoundBufferParams>;
	using SoundBufferManager = ResourceManager<SoundBuffer, SoundBufferParams>;

	class NAZARA_AUDIO2_API SoundBuffer : public Resource
	{
		friend Sound;

		public:
			using Params = SoundBufferParams;

			SoundBuffer() = default;
			SoundBuffer(AudioFormat format, std::span<const AudioChannel> channels, UInt64 frameCount, UInt32 sampleRate, const void* samples);
			SoundBuffer(const SoundBuffer&) = delete;
			SoundBuffer(SoundBuffer&&) = delete;
			~SoundBuffer() = default;

			void ConvertFormat(AudioFormat format, AudioDitherMode ditherMode = AudioDitherMode::None);

			inline std::span<const AudioChannel> GetChannels() const;
			inline Time GetDuration() const;
			inline AudioFormat GetFormat() const;
			inline UInt64 GetFrameCount() const;
			inline void* GetSamples();
			inline const void* GetSamples() const;
			inline UInt64 GetSampleCount() const;
			inline UInt32 GetSampleRate() const;

			SoundBuffer& operator=(const SoundBuffer&) = delete;
			SoundBuffer& operator=(SoundBuffer&&) = delete;

			static std::shared_ptr<SoundBuffer> LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

		private:
			std::vector<UInt8> m_samples;
			AudioFormat m_format;
			HybridVector<AudioChannel, 4> m_channels;
			Time m_duration;
			UInt32 m_sampleRate;
			UInt64 m_frameCount;
	};
}

#include <Nazara/Audio2/SoundBuffer.inl>

#endif // NAZARA_AUDIO2_SOUNDBUFFER_HPP
