// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUND_HPP
#define NAZARA_SOUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Sound : public SoundEmitter
	{
		public:
			Sound() = default;
			Sound(const SoundBuffer* soundBuffer);
			Sound(const Sound& sound);
			Sound(Sound&&) = default;
			~Sound();

			void EnableLooping(bool loop);

			const SoundBuffer* GetBuffer() const;
			UInt32 GetDuration() const;
			UInt32 GetPlayingOffset() const;
			SoundStatus GetStatus() const;

			bool IsLooping() const;
			bool IsPlayable() const;
			bool IsPlaying() const;

			bool LoadFromFile(const String& filePath, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

			void Pause();
			void Play();

			void SetBuffer(const SoundBuffer* buffer);
			void SetPlayingOffset(UInt32 offset);

			void Stop();

			Sound& operator=(const Sound&) = delete; ///TODO?
			Sound& operator=(Sound&&) = default;

		private:
			SoundBufferConstRef m_buffer;
	};
}

#endif // NAZARA_SOUND_HPP
