// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUND_HPP
#define NAZARA_AUDIO_SOUND_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Sound final : public SoundEmitter
	{
		public:
			using SoundEmitter::SoundEmitter;
			Sound();
			Sound(AudioDevice& audioDevice, std::shared_ptr<SoundBuffer> soundBuffer);
			Sound(const Sound&) = delete;
			Sound(Sound&&) = default;
			~Sound();

			void EnableLooping(bool loop) override;

			const std::shared_ptr<SoundBuffer>& GetBuffer() const;
			Time GetDuration() const override;
			Time GetPlayingOffset() const override;
			UInt64 GetSampleOffset() const override;
			UInt32 GetSampleRate() const override;
			SoundStatus GetStatus() const override;

			bool IsLooping() const override;
			bool IsPlayable() const;

			bool LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

			void Pause() override;
			void Play() override;

			void SetBuffer(std::shared_ptr<SoundBuffer> soundBuffer);

			void SeekToSampleOffset(UInt64 offset) override;

			void Stop() override;

			Sound& operator=(const Sound&) = delete;
			Sound& operator=(Sound&&) = default;

		private:
			std::shared_ptr<SoundBuffer> m_buffer;
	};
}

#endif // NAZARA_AUDIO_SOUND_HPP
