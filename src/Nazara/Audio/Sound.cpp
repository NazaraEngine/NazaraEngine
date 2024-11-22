// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::Sound
	* \brief Audio class that represents a sound
	*
	* \remark Module Audio needs to be initialized to use this class
	*/

	Sound::Sound() :
	Sound(*Audio::Instance()->GetDefaultDevice())
	{
	}

	/*!
	* \brief Constructs a Sound object
	*
	* \param soundBuffer Buffer to read sound from
	*/
	Sound::Sound(AudioDevice& audioDevice, std::shared_ptr<SoundBuffer> soundBuffer) :
	Sound(audioDevice)
	{
		SetBuffer(std::move(soundBuffer));
	}

	/*!
	* \brief Destructs the object and calls Stop
	*
	* \see Stop
	*/
	Sound::~Sound()
	{
		if (m_source)
			Stop();
	}

	/*!
	* \brief Enables the looping of the music
	*
	* \param loop Should sound loop
	*/
	void Sound::EnableLooping(bool loop)
	{
		m_source->EnableLooping(loop);
	}

	/*!
	* \brief Gets the internal buffer
	* \return Internal buffer
	*/
	const std::shared_ptr<SoundBuffer>& Sound::GetBuffer() const
	{
		return m_buffer;
	}

	/*!
	* \brief Gets the duration of the sound
	* \return Duration of the music in milliseconds
	*
	* \remark Produces a NazaraError if there is no buffer
	*/
	Time Sound::GetDuration() const
	{
		NazaraAssert(m_buffer, "Invalid sound buffer");

		return m_buffer->GetDuration();
	}

	/*!
	* \brief Gets the current playing offset of the sound
	* \return Offset
	*/
	Time Sound::GetPlayingOffset() const
	{
		return m_source->GetPlayingOffset();
	}

	/*!
	* \brief Gets the current sample offset of the sound
	* \return Offset
	*/
	UInt64 Sound::GetSampleOffset() const
	{
		return m_source->GetSampleOffset();
	}

	/*!
	* \brief Gets the sample rate of the sound
	* \return Offset
	*/
	UInt32 Sound::GetSampleRate() const
	{
		return m_buffer->GetSampleRate();
	}

	/*!
	* \brief Gets the status of the music
	* \return Enumeration of type SoundStatus (Playing, Stopped, ...)
	*/
	SoundStatus Sound::GetStatus() const
	{
		return m_source->GetStatus();
	}

	/*!
	* \brief Checks whether the sound is looping
	* \return true if it is the case
	*/
	bool Sound::IsLooping() const
	{
		return m_source->IsLooping();
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
	* \brief Loads the sound from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound
	*
	* \remark Produces a NazaraError if loading failed
	*/
	bool Sound::LoadFromFile(const std::filesystem::path& filePath, const SoundBufferParams& params)
	{
		std::shared_ptr<SoundBuffer> buffer = SoundBuffer::LoadFromFile(filePath, params);
		if (!buffer)
		{
			NazaraError("failed to load buffer from file ({0})", filePath);
			return false;
		}

		SetBuffer(std::move(buffer));
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
		std::shared_ptr<SoundBuffer> buffer = SoundBuffer::LoadFromMemory(data, size, params);
		if (!buffer)
		{
			NazaraError("failed to load buffer from memory ({0})", PointerToString(data));
			return false;
		}

		SetBuffer(std::move(buffer));
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
		std::shared_ptr<SoundBuffer> buffer = SoundBuffer::LoadFromStream(stream, params);
		if (!buffer)
		{
			NazaraError("failed to load buffer from stream");
			return false;
		}

		SetBuffer(std::move(buffer));
		return true;
	}

	/*!
	* \brief Pauses the sound
	*/
	void Sound::Pause()
	{
		m_source->Pause();
	}

	/*!
	* \brief Plays the music
	*/
	void Sound::Play()
	{
		NazaraAssert(IsPlayable(), "Sound is not playable");

		m_source->Play();
	}

	/*!
	* \brief Sets the audio buffer
	*
	* \param buffer Audio buffer
	*/
	void Sound::SetBuffer(std::shared_ptr<SoundBuffer> buffer)
	{
		NazaraAssert(buffer, "Invalid sound buffer");

		if (m_buffer == buffer)
			return;

		Stop();

		m_buffer = std::move(buffer);
		m_source->SetBuffer(m_buffer->GetAudioBuffer(m_source->GetAudioDevice().get()));
	}

	/*!
	* \brief Sets the source to a sample offset
	*
	* \param offset Sample offset
	*/
	void Sound::SeekToSampleOffset(UInt64 offset)
	{
		m_source->SetSampleOffset(SafeCast<UInt32>(offset));
	}

	/*!
	* \brief Stops the sound
	*/
	void Sound::Stop()
	{
		m_source->Stop();
	}
}
