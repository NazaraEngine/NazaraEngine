// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDSTREAM_HPP
#define NAZARA_AUDIO_SOUNDSTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Time.hpp>
#include <mutex>

namespace Nz
{
	struct SoundStreamParams : ResourceParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

	class Mutex;
	class SoundStream;

	using SoundStreamLoader = ResourceLoader<SoundStream, SoundStreamParams>;

	class NAZARA_AUDIO_API SoundStream : public Resource
	{
		public:
			using Params = SoundStreamParams;

			SoundStream() = default;
			virtual ~SoundStream();

			virtual Time GetDuration() const = 0;
			virtual AudioFormat GetFormat() const = 0;
			virtual std::mutex& GetMutex() = 0;
			virtual UInt64 GetSampleCount() const = 0;
			virtual UInt32 GetSampleRate() const = 0;

			virtual UInt64 Read(void* buffer, UInt64 sampleCount) = 0;
			virtual void Seek(UInt64 offset) = 0;
			virtual UInt64 Tell() = 0;

			static std::shared_ptr<SoundStream> OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params = SoundStreamParams());
			static std::shared_ptr<SoundStream> OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params = SoundStreamParams());
			static std::shared_ptr<SoundStream> OpenFromStream(Stream& stream, const SoundStreamParams& params = SoundStreamParams());
	};
}

#endif // NAZARA_AUDIO_SOUNDSTREAM_HPP
