// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	Sound::Sound(const SoundBuffer* soundBuffer)
	{
		SetBuffer(soundBuffer);
	}

	Sound::Sound(const Sound& sound) :
	SoundEmitter(sound)
	{
		SetBuffer(sound.m_buffer);
	}

	Sound::~Sound()
	{
		Stop();
	}

	void Sound::EnableLooping(bool loop)
	{
		alSourcei(m_source, AL_LOOPING, loop);
	}

	const SoundBuffer* Sound::GetBuffer() const
	{
		return m_buffer;
	}

	UInt32 Sound::GetDuration() const
	{
		NazaraAssert(m_buffer, "Invalid sound buffer");

		return m_buffer->GetDuration();
	}

	UInt32 Sound::GetPlayingOffset() const
	{
		ALint samples = 0;
		alGetSourcei(m_source, AL_SAMPLE_OFFSET, &samples);

		return static_cast<UInt32>(1000ULL * samples / m_buffer->GetSampleRate());
	}

	SoundStatus Sound::GetStatus() const
	{
		return GetInternalStatus();
	}

	bool Sound::IsLooping() const
	{
		ALint loop;
		alGetSourcei(m_source, AL_LOOPING, &loop);

		return loop != AL_FALSE;
	}

	bool Sound::IsPlayable() const
	{
		return m_buffer != nullptr;
	}

	bool Sound::IsPlaying() const
	{
		return GetStatus() == SoundStatus_Playing;
	}

	bool Sound::LoadFromFile(const String& filePath, const SoundBufferParams& params)
	{
		SoundBufferRef buffer = SoundBuffer::New();
		if (!buffer->LoadFromFile(filePath, params))
		{
			NazaraError("Failed to load buffer from file (" + filePath + ')');
			return false;
		}

		SetBuffer(buffer);
		return true;
	}

	bool Sound::LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params)
	{
		SoundBufferRef buffer = SoundBuffer::New();
		if (!buffer->LoadFromMemory(data, size, params))
		{
			NazaraError("Failed to load buffer from memory (" + String::Pointer(data) + ')');
			return false;
		}

		SetBuffer(buffer);
		return true;
	}

	bool Sound::LoadFromStream(Stream& stream, const SoundBufferParams& params)
	{
		SoundBufferRef buffer = SoundBuffer::New();
		if (!buffer->LoadFromStream(stream, params))
		{
			NazaraError("Failed to load buffer from stream");
			return false;
		}

		SetBuffer(buffer);
		return true;
	}

	void Sound::Pause()
	{
		alSourcePause(m_source);
	}

	void Sound::Play()
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_buffer)
		{
			NazaraError("Invalid sound buffer");
			return;
		}
		#endif

		alSourcePlay(m_source);
	}

	void Sound::SetBuffer(const SoundBuffer* buffer)
	{
		#if NAZARA_AUDIO_SAFE
		if (buffer && !buffer->IsValid())
		{
			NazaraError("Invalid sound buffer");
			return;
		}
		#endif

		if (m_buffer == buffer)
			return;

		Stop();

		m_buffer = buffer;

		if (m_buffer)
			alSourcei(m_source, AL_BUFFER, m_buffer->GetOpenALBuffer());
		else
			alSourcei(m_source, AL_BUFFER, AL_NONE);
	}

	void Sound::SetPlayingOffset(UInt32 offset)
	{
		alSourcei(m_source, AL_SAMPLE_OFFSET, static_cast<ALint>(offset/1000.f * m_buffer->GetSampleRate()));
	}

	void Sound::Stop()
	{
		alSourceStop(m_source);
	}
}
