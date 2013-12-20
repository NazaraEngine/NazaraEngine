// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUND_HPP
#define NAZARA_SOUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>

class NAZARA_API NzSound : public NzSoundEmitter
{
	public:
		NzSound() = default;
		NzSound(const NzSoundBuffer* soundBuffer);
		NzSound(const NzSound& sound);
		~NzSound();

		void EnableLooping(bool loop);

		const NzSoundBuffer* GetBuffer() const;
		nzUInt32 GetDuration() const;
		nzUInt32 GetPlayingOffset() const;
		nzSoundStatus GetStatus() const;

		bool IsLooping() const;
		bool IsPlayable() const;
		bool IsPlaying() const;

		bool LoadFromFile(const NzString& filePath, const NzSoundBufferParams& params = NzSoundBufferParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzSoundBufferParams& params = NzSoundBufferParams());
		bool LoadFromStream(NzInputStream& stream, const NzSoundBufferParams& params = NzSoundBufferParams());

		void Pause();
		bool Play();

		void SetBuffer(const NzSoundBuffer* buffer);
		void SetPlayingOffset(nzUInt32 offset);

		void Stop();

	private:
		NzSoundBufferConstRef m_buffer;
};

#endif // NAZARA_SOUND_HPP
