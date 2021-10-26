// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDBUFFER_HPP
#define NAZARA_AUDIO_SOUNDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>

namespace Nz
{
	struct SoundBufferParams : ResourceParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

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
			SoundBuffer();
			SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			SoundBuffer(const SoundBuffer&) = delete;
			SoundBuffer(SoundBuffer&&) = delete;
			~SoundBuffer();

			bool Create(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			void Destroy();

			UInt32 GetDuration() const;
			AudioFormat GetFormat() const;
			const Int16* GetSamples() const;
			UInt64 GetSampleCount() const;
			UInt32 GetSampleRate() const;

			bool IsValid() const;

			SoundBuffer& operator=(const SoundBuffer&) = delete;
			SoundBuffer& operator=(SoundBuffer&&) = delete;

			static bool IsFormatSupported(AudioFormat format);

			static std::shared_ptr<SoundBuffer> LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			static std::shared_ptr<SoundBuffer> LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

		private:
			unsigned int GetOpenALBuffer() const;

			std::unique_ptr<SoundBufferImpl> m_impl;
	};
}

#include <Nazara/Audio/SoundBuffer.inl>

#endif // NAZARA_AUDIO_SOUNDBUFFER_HPP
