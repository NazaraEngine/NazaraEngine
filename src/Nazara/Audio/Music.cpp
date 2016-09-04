// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <memory>
#include <vector>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::Music
	* \brief Audio class that represents a music
	*
	* \remark Module Audio needs to be initialized to use this class
	*/

	/*!
	* \brief Checks whether the parameters for the loading of the music are correct
	* \return true If parameters are valid
	*/

	bool MusicParams::IsValid() const
	{
		return true;
	}

	struct MusicImpl
	{
		ALenum audioFormat;
		std::unique_ptr<SoundStream> stream;
		std::vector<Int16> chunkSamples;
		Mutex bufferLock;
		Thread thread;
		UInt64 processedSamples;
		bool loop = false;
		bool streaming = false;
		unsigned int sampleRate;
	};

	/*!
	* \brief Destructs the object and calls Destroy
	*
	* \see Destroy
	*/

	Music::~Music()
	{
		Destroy();
	}

	/*!
	* \brief Creates a music with a sound stream
	* \return true if creation was succesful
	*
	* \param soundStream Sound stream which is the source for the music
	*
	* \remark Produces a NazaraError if soundStream is invalid with NAZARA_AUDIO_SAFE defined
	*/

	bool Music::Create(SoundStream* soundStream)
	{
		NazaraAssert(soundStream, "Invalid stream");

		Destroy();

		AudioFormat format = soundStream->GetFormat();

		m_impl = new MusicImpl;
		m_impl->sampleRate = soundStream->GetSampleRate();
		m_impl->audioFormat = OpenAL::AudioFormat[format];
		m_impl->chunkSamples.resize(format * m_impl->sampleRate); // One second of samples
		m_impl->stream.reset(soundStream);

		SetPlayingOffset(0);

		return true;
	}

	/*!
	* \brief Destroys the current music and frees resources
	*/

	void Music::Destroy()
	{
		if (m_impl)
		{
			Stop();

			delete m_impl;
			m_impl = nullptr;
		}
	}

	/*!
	* \brief Enables the looping of the music
	*
	* \param loop Should music loop
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	void Music::EnableLooping(bool loop)
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return;
		}
		#endif

		m_impl->loop = loop;
	}

	/*!
	* \brief Gets the duration of the music
	* \return Duration of the music in milliseconds
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	UInt32 Music::GetDuration() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return 0;
		}
		#endif

		return m_impl->stream->GetDuration();
	}

	/*!
	* \brief Gets the format of the music
	* \return Enumeration of type AudioFormat (mono, stereo, ...)
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	AudioFormat Music::GetFormat() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return AudioFormat_Unknown;
		}
		#endif

		return m_impl->stream->GetFormat();
	}

	/*!
	* \brief Gets the current offset in the music
	* \return Offset in milliseconds (works with entire seconds)
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/
	UInt32 Music::GetPlayingOffset() const
	{
		NazaraAssert(m_impl, "Music not created");

		// Prevent music thread from enqueing new buffers while we're getting the count
		Nz::LockGuard lock(m_impl->bufferLock);

		ALint samples = 0;
		alGetSourcei(m_source, AL_SAMPLE_OFFSET, &samples);

		return static_cast<UInt32>((1000ULL * (samples + (m_impl->processedSamples / m_impl->stream->GetFormat()))) / m_impl->sampleRate);
	}

	/*!
	* \brief Gets the number of samples in the music
	* \return Count of samples (number of seconds * sample rate * channel count)
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/
	UInt64 Music::GetSampleCount() const
	{
		NazaraAssert(m_impl, "Music not created");

		return m_impl->stream->GetSampleCount();
	}

	/*!
	* \brief Gets the rates of sample in the music
	* \return Rate of sample in Hertz (Hz)
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/
	UInt32 Music::GetSampleRate() const
	{
		NazaraAssert(m_impl, "Music not created");

		return m_impl->sampleRate;
	}

	/*!
	* \brief Gets the status of the music
	* \return Enumeration of type SoundStatus (Playing, Stopped, ...)
	*
	* \remark If the music is not playing, Stopped is returned
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/
	SoundStatus Music::GetStatus() const
	{
		NazaraAssert(m_impl, "Music not created");

		SoundStatus status = GetInternalStatus();

		// To compensate any delays (or the timelaps between Play() and the thread startup)
		if (m_impl->streaming && status == SoundStatus_Stopped)
			status = SoundStatus_Playing;

		return status;
	}

	/*!
	* \brief Checks whether the music is looping
	* \return true if it is the case
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	bool Music::IsLooping() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return false;
		}
		#endif

		return m_impl->loop;
	}

	/*!
	* \brief Loads the music from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the music
	*/

	bool Music::OpenFromFile(const String& filePath, const MusicParams& params)
	{
		return MusicLoader::LoadFromFile(this, filePath, params);
	}

	/*!
	* \brief Loads the music from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the music
	*/

	bool Music::OpenFromMemory(const void* data, std::size_t size, const MusicParams& params)
	{
		return MusicLoader::LoadFromMemory(this, data, size, params);
	}

	/*!
	* \brief Loads the music from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the music
	* \param params Parameters for the music
	*/

	bool Music::OpenFromStream(Stream& stream, const MusicParams& params)
	{
		return MusicLoader::LoadFromStream(this, stream, params);
	}

	/*!
	* \brief Pauses the music
	*/

	void Music::Pause()
	{
		alSourcePause(m_source);
	}

	/*!
	* \brief Plays the music
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	void Music::Play()
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return;
		}
		#endif

		// Maybe we are already playing
		if (m_impl->streaming)
		{
			switch (GetStatus())
			{
				case SoundStatus_Playing:
					SetPlayingOffset(0);
					break;

				case SoundStatus_Paused:
					alSourcePlay(m_source);
					break;

				default:
					break; // We shouldn't be stopped
			}
		}
		else
		{
			// Starting streaming's thread
			m_impl->streaming = true;
			m_impl->thread = Thread(&Music::MusicThread, this);
		}
	}

	/*!
	* \brief Sets the playing offset for the music
	*
	* \param offset Offset in the music in milliseconds
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	void Music::SetPlayingOffset(UInt32 offset)
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return;
		}
		#endif

		bool isPlaying = m_impl->streaming;
		
		if (isPlaying)
			Stop();

		m_impl->stream->Seek(offset);
		m_impl->processedSamples = UInt64(offset) * m_impl->sampleRate * m_impl->stream->GetFormat() / 1000ULL;

		if (isPlaying)
			Play();
	}

	/*!
	* \brief Stops the music
	*
	* \remark Produces a NazaraError if there is no music with NAZARA_AUDIO_SAFE defined
	*/

	void Music::Stop()
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return;
		}
		#endif

		if (m_impl->streaming)
		{
			m_impl->streaming = false;
			m_impl->thread.Join();
		}
	}

	/*!
	* \brief Fills the buffer and queues it up
	* \return true if operation was successful
	*
	* \param buffer Index of the buffer
	*/

	bool Music::FillAndQueueBuffer(unsigned int buffer)
	{
		std::size_t sampleCount = m_impl->chunkSamples.size();
		std::size_t sampleRead = 0;

		// Fill the buffer by reading from the stream
		for (;;)
		{
			sampleRead += m_impl->stream->Read(&m_impl->chunkSamples[sampleRead], sampleCount - sampleRead);
			if (sampleRead < sampleCount && m_impl->loop)
			{
				// In case we read less than expected, assume we reached the end of the stream and seek back to the beginning
				m_impl->stream->Seek(0);
				continue;
			}

			// Either we read the size we wanted, either we're not looping
			break;
		}

		// Update the buffer (send it to OpenAL) and queue it if we got any data
		if (sampleRead > 0)
		{
			alBufferData(buffer, m_impl->audioFormat, &m_impl->chunkSamples[0], static_cast<ALsizei>(sampleRead*sizeof(Int16)), static_cast<ALsizei>(m_impl->sampleRate));
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		return sampleRead != sampleCount; // End of stream (Does not happen when looping)
	}

	/*!
	* \brief Thread function for the music
	*/

	void Music::MusicThread()
	{
		// Allocation of streaming buffers
		ALuint buffers[NAZARA_AUDIO_STREAMED_BUFFER_COUNT];
		alGenBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);

		for (unsigned int i = 0; i < NAZARA_AUDIO_STREAMED_BUFFER_COUNT; ++i)
		{
			if (FillAndQueueBuffer(buffers[i]))
				break; // We have reached the end of the stream, there is no use to add new buffers
		}

		alSourcePlay(m_source);

		// Reading loop (Filling new buffers as playing)
		while (m_impl->streaming)
		{
			// The reading has stopped, we have reached the end of the stream
			SoundStatus status = GetInternalStatus();
			if (status == SoundStatus_Stopped)
			{
				m_impl->streaming = false;
				break;
			}

			Nz::LockGuard lock(m_impl->bufferLock);

			// We treat read buffers
			ALint processedCount = 0;
			alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedCount);
			while (processedCount--)
			{
				ALuint buffer;
				alSourceUnqueueBuffers(m_source, 1, &buffer);
				
				ALint bits, size;
				alGetBufferi(buffer, AL_BITS, &bits);
				alGetBufferi(buffer, AL_SIZE, &size);
				
				if (bits != 0)
					m_impl->processedSamples += (8 * size) / bits;

				if (FillAndQueueBuffer(buffer))
					break;
			}

			lock.Unlock();

			// We go back to sleep
			Thread::Sleep(50);
		}

		// Stop playing of the sound (in the case where it has not been already done)
		alSourceStop(m_source);

		// We delete buffers from the stream
		ALint queuedBufferCount;
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

		ALuint buffer;
		for (ALint i = 0; i < queuedBufferCount; ++i)
			alSourceUnqueueBuffers(m_source, 1, &buffer);

		alDeleteBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);
	}

	MusicLoader::LoaderList Music::s_loaders;
}
