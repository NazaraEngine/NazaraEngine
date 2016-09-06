// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUSIC_HPP
#define NAZARA_MUSIC_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>

namespace Nz
{
	struct MusicParams : ResourceParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

	class Music;
	class SoundStream;

	using MusicLoader = ResourceLoader<Music, MusicParams>;

	struct MusicImpl;

	class NAZARA_AUDIO_API Music : public Resource, public SoundEmitter
	{
		friend MusicLoader;

		public:
			Music() = default;
			Music(const Music&) = delete;
			Music(Music&&) = delete; ///TODO
			~Music();

			bool Create(SoundStream* soundStream);
			void Destroy();

			void EnableLooping(bool loop);

			UInt32 GetDuration() const;
			AudioFormat GetFormat() const;
			UInt32 GetPlayingOffset() const;
			UInt64 GetSampleCount() const;
			UInt32 GetSampleRate() const;
			SoundStatus GetStatus() const;

			bool IsLooping() const;

			bool OpenFromFile(const String& filePath, const MusicParams& params = MusicParams());
			bool OpenFromMemory(const void* data, std::size_t size, const MusicParams& params = MusicParams());
			bool OpenFromStream(Stream& stream, const MusicParams& params = MusicParams());

			void Pause();
			void Play();

			void SetPlayingOffset(UInt32 offset);

			void Stop();

			Music& operator=(const Music&) = delete;
			Music& operator=(Music&&) = delete; ///TODO

		private:
			MusicImpl* m_impl = nullptr;

			bool FillAndQueueBuffer(unsigned int buffer);
			void MusicThread();

			static MusicLoader::LoaderList s_loaders;
	};
}

#endif // NAZARA_MUSIC_HPP
