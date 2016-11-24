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
	/*!
	* \ingroup audio
	* \class Nz::Sound
	* \brief Audio class that represents a sound
	*
	* \remark Module Audio needs to be initialized to use this class
	*/

	/*!
	* \brief Constructs a Sound object
	*
	* \param soundBuffer Buffer to read sound from
	*/
	Sound::Sound(const SoundBuffer* soundBuffer)
	{
		SetBuffer(soundBuffer);
	}

	/*!
	* \brief Constructs a Sound object which is a copy of another
	*
	* \param sound Sound to copy
	*/
	Sound::Sound(const Sound& sound) :
	SoundEmitter(sound)
	{
		SetBuffer(sound.m_buffer);
	}

	/*!
	* \brief Destructs the object and calls Stop
	*
	* \see Stop
	*/
	Sound::~Sound()
	{
		Stop();
	}

	/*!
	* \brief Enables the looping of the music
	*
	* \param loop Should sound loop
	*/
	void Sound::EnableLooping(bool loop)
	{
		alSourcei(m_source, AL_LOOPING, loop);
	}

	/*!
	* \brief Gets the internal buffer
	* \return Internal buffer
	*/
	const SoundBuffer* Sound::GetBuffer() const
	{
		return m_buffer;
	}

	/*!
	* \brief Gets the duration of the sound
	* \return Duration of the music in milliseconds
	*
	* \remark Produces a NazaraError if there is no buffer
	*/
	UInt32 Sound::GetDuration() const
	{
		NazaraAssert(m_buffer, "Invalid sound buffer");

		return m_buffer->GetDuration();
	}

	/*!
	* \brief Gets the current offset in the sound
	* \return Offset in milliseconds (works with entire seconds)
	*/
	UInt32 Sound::GetPlayingOffset() const
	{
		ALint samples = 0;
		alGetSourcei(m_source, AL_SAMPLE_OFFSET, &samples);

		return static_cast<UInt32>(1000ULL * samples / m_buffer->GetSampleRate());
	}

	/*!
	* \brief Gets the status of the music
	* \return Enumeration of type SoundStatus (Playing, Stopped, ...)
	*/
	SoundStatus Sound::GetStatus() const
	{
		return GetInternalStatus();
	}

	/*!
	* \brief Checks whether the sound is looping
	* \return true if it is the case
	*/
	bool Sound::IsLooping() const
	{
		ALint loop;
		alGetSourcei(m_source, AL_LOOPING, &loop);

		return loop != AL_FALSE;
	}

	/*!
	* \brief Checks whether the sound is playable
	* \return true if it is the case
	*/

	bool Sound::IsPlayable() const
	{
		return m_buffer != nullptr;
	}

	/*!
	* \brief Checks whether the sound is playing
	* \return true if it is the case
	*/
	bool Sound::IsPlaying() const
	{
		return GetStatus() == SoundStatus_Playing;
	}

	/*!
	* \brief Loads the sound from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound
	*
	* \remark Produces a NazaraError if loading failed
	*/
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

	/*!
	* \brief Loads the sound from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the sound
	*
	* \remark Produces a NazaraError if loading failed
	*/
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

	/*!
	* \brief Loads the sound from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the sound
	* \param params Parameters for the sound
	*
	* \remark Produces a NazaraError if loading failed
	*/
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

	/*!
	* \brief Pauses the sound
	*/
	void Sound::Pause()
	{
		alSourcePause(m_source);
	}

	/*!
	* \brief Plays the music
	*
	* \remark Produces a NazaraError if the sound is not playable with NAZARA_AUDIO_SAFE defined
	*/
	void Sound::Play()
	{
		NazaraAssert(IsPlayable(), "Music is not playable");

		alSourcePlay(m_source);
	}

	/*!
	* \brief Sets the internal buffer
	*
	* \param buffer Internal buffer
	*
	* \remark Produces a NazaraError if buffer is invalid with NAZARA_AUDIO_SAFE defined
	*/
	void Sound::SetBuffer(const SoundBuffer* buffer)
	{
		NazaraAssert(!buffer || buffer->IsValid(), "Invalid sound buffer");

		if (m_buffer == buffer)
			return;

		Stop();

		m_buffer = buffer;

		if (m_buffer)
			alSourcei(m_source, AL_BUFFER, m_buffer->GetOpenALBuffer());
		else
			alSourcei(m_source, AL_BUFFER, AL_NONE);
	}

	/*!
	* \brief Sets the playing offset for the sound
	*
	* \param offset Offset in the sound in milliseconds
	*/
	void Sound::SetPlayingOffset(UInt32 offset)
	{
		alSourcei(m_source, AL_SAMPLE_OFFSET, static_cast<ALint>(offset/1000.f * m_buffer->GetSampleRate()));
	}

	/*!
	* \brief Stops the sound
	*/
	void Sound::Stop()
	{
		alSourceStop(m_source);
	}
}
