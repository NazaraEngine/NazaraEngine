// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUND_HPP
#define NAZARA_SOUND_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Sound : public SoundEmitter
	{
		public:
			Sound() = default;
			Sound(std::shared_ptr<const SoundBuffer> soundBuffer);
			Sound(const Sound&) = default;
			Sound(Sound&&) noexcept = default;
			~Sound();

			void EnableLooping(bool loop) override;

			const std::shared_ptr<const SoundBuffer>& GetBuffer() const;
			UInt32 GetDuration() const override;
			UInt32 GetPlayingOffset() const override;
			SoundStatus GetStatus() const override;

			bool IsLooping() const override;
			bool IsPlayable() const;

			bool LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

			void Pause() override;
			void Play() override;

			void SetBuffer(std::shared_ptr<const SoundBuffer> soundBuffer);
			void SetPlayingOffset(UInt32 offset);

			void Stop() override;

			Sound& operator=(const Sound&) = delete; ///TODO?
			Sound& operator=(Sound&&) noexcept = default;

		private:
			std::shared_ptr<const SoundBuffer> m_buffer;
	};
}

#endif // NAZARA_SOUND_HPP
