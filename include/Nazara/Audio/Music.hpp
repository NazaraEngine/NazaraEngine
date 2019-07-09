// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUSIC_HPP
#define NAZARA_MUSIC_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/MovablePtr.hpp>

namespace Nz
{
	struct MusicImpl;

	class NAZARA_AUDIO_API Music : public Resource, public SoundEmitter
	{
		public:
			Music() = default;
			Music(const Music&) = delete;
			Music(Music&&) noexcept = default;
			~Music();

			bool Create(SoundStream* soundStream);
			void Destroy();

			void EnableLooping(bool loop) override;

			UInt32 GetDuration() const override;
			AudioFormat GetFormat() const;
			UInt32 GetPlayingOffset() const override;
			UInt64 GetSampleCount() const;
			UInt32 GetSampleRate() const;
			SoundStatus GetStatus() const override;

			bool IsLooping() const override;

			bool OpenFromFile(const String& filePath, const SoundStreamParams& params = SoundStreamParams());
			bool OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params = SoundStreamParams());
			bool OpenFromStream(Stream& stream, const SoundStreamParams& params = SoundStreamParams());

			void Pause() override;
			void Play() override;

			void SetPlayingOffset(UInt32 offset);

			void Stop() override;

			Music& operator=(const Music&) = delete;
			Music& operator=(Music&&) noexcept = default;

		private:
			MovablePtr<MusicImpl> m_impl;

			bool FillAndQueueBuffer(unsigned int buffer);
			void MusicThread();
			void StopThread();
	};
}

#endif // NAZARA_MUSIC_HPP
