// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDSTREAM_HPP
#define NAZARA_AUDIO_SOUNDSTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundDataSource.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Time.hpp>
#include <mutex>

namespace Nz
{
	struct SoundStreamParams : ResourceParameters
	{
		bool IsValid() const;
	};

	class Mutex;
	class SoundStream;

	using SoundStreamLoader = ResourceLoader<SoundStream, SoundStreamParams>;

	class NAZARA_AUDIO_API SoundStream : public Resource, public SoundDataSource
	{
		public:
			using Params = SoundStreamParams;

			SoundStream() = default;
			~SoundStream();

			static std::shared_ptr<SoundStream> OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params = SoundStreamParams());
			static std::shared_ptr<SoundStream> OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params = SoundStreamParams());
			static std::shared_ptr<SoundStream> OpenFromStream(Stream& stream, const SoundStreamParams& params = SoundStreamParams());
	};
}

#include <Nazara/Audio/SoundStream.inl>

#endif // NAZARA_AUDIO_SOUNDSTREAM_HPP
