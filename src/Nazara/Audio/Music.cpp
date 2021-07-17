// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
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

	struct MusicImpl
	{
		ALenum audioFormat;
		std::atomic_bool streaming = false;
		std::atomic<UInt64> processedSamples;
		std::vector<Int16> chunkSamples;
		std::mutex bufferLock;
		std::shared_ptr<SoundStream> stream;
		std::thread thread;
		UInt64 streamOffset;
		bool loop = false;
		unsigned int sampleRate;
	};

	Music::Music() = default;
	Music::Music(Music&&) noexcept = default;

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
	*/

	bool Music::Create(std::shared_ptr<SoundStream> soundStream)
	{
		NazaraAssert(soundStream, "Invalid stream");

		Destroy();

		AudioFormat format = soundStream->GetFormat();

		m_impl = std::make_unique<MusicImpl>();
		m_impl->sampleRate = soundStream->GetSampleRate();
		m_impl->audioFormat = OpenAL::AudioFormat[UnderlyingCast(format)];
		m_impl->chunkSamples.resize(GetChannelCount(format) * m_impl->sampleRate); // One second of samples
		m_impl->stream = std::move(soundStream);

		SetPlayingOffset(0);

		return true;
	}

	/*!
	* \brief Destroys the current music and frees resources
	*
	* \remark If the Music is playing, it is stopped first.
	*/
	void Music::Destroy()
	{
		if (m_impl)
		{
			StopThread();

			m_impl.reset();
		}
	}

	/*!
	* \brief Enables the looping of the music
	*
	* \param loop Should music loop
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::EnableLooping(bool loop)
	{
		NazaraAssert(m_impl, "Music not created");

		m_impl->loop = loop;
	}

	/*!
	* \brief Gets the duration of the music
	* \return Duration of the music in milliseconds
	*
	* \remark Music must be valid when calling this function
	*/
	UInt32 Music::GetDuration() const
	{
		NazaraAssert(m_impl, "Music not created");

		return m_impl->stream->GetDuration();
	}

	/*!
	* \brief Gets the format of the music
	* \return Enumeration of type AudioFormat (mono, stereo, ...)
	*
	* \remark Music must be valid when calling this function
	*/
	AudioFormat Music::GetFormat() const
	{
		NazaraAssert(m_impl, "Music not created");

		return m_impl->stream->GetFormat();
	}

	/*!
	* \brief Gets the current offset in the music
	* \return Offset in milliseconds (works with entire seconds)
	*
	* \remark Music must be valid when calling this function
	*/
	UInt32 Music::GetPlayingOffset() const
	{
		NazaraAssert(m_impl, "Music not created");

		// Prevent music thread from enqueing new buffers while we're getting the count
		std::lock_guard<std::mutex> lock(m_impl->bufferLock);

		ALint samples = 0;
		alGetSourcei(m_source, AL_SAMPLE_OFFSET, &samples);

		return static_cast<UInt32>((1000ULL * (samples + (m_impl->processedSamples / GetChannelCount(m_impl->stream->GetFormat())))) / m_impl->sampleRate);
	}

	/*!
	* \brief Gets the number of samples in the music
	* \return Count of samples (number of seconds * sample rate * channel count)
	*
	* \remark Music must be valid when calling this function
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
	* \remark Music must be valid when calling this function
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
	* \remark Music must be valid when calling this function
	*/
	SoundStatus Music::GetStatus() const
	{
		NazaraAssert(m_impl, "Music not created");

		SoundStatus status = GetInternalStatus();

		// To compensate any delays (or the timelaps between Play() and the thread startup)
		if (m_impl->streaming && status == SoundStatus::Stopped)
			status = SoundStatus::Playing;

		return status;
	}

	/*!
	* \brief Checks whether the music is looping
	* \return true if it is the case
	*
	* \remark Music must be valid when calling this function
	*/
	bool Music::IsLooping() const
	{
		NazaraAssert(m_impl, "Music not created");

		return m_impl->loop;
	}

	/*!
	* \brief Opens the music from a file
	* \return true if the file was successfully opened
	*
	* \param filePath Path to the file
	* \param params Parameters for the music
	*/
	bool Music::OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params)
	{
		if (std::shared_ptr<SoundStream> soundStream = SoundStream::OpenFromFile(filePath, params))
			return Create(std::move(soundStream));
		else
			return false;
	}

	/*!
	* \brief Opens the music from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the music
	*
	* \remark The memory pointer must stay valid (accessible) as long as the music is playing
	*/
	bool Music::OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params)
	{
		if (std::shared_ptr<SoundStream> soundStream = SoundStream::OpenFromMemory(data, size, params))
			return Create(std::move(soundStream));
		else
			return false;
	}

	/*!
	* \brief Loads the music from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the music
	* \param params Parameters for the music
	*
	* \remark The stream must stay valid as long as the music is playing
	*/
	bool Music::OpenFromStream(Stream& stream, const SoundStreamParams& params)
	{
		if (std::shared_ptr<SoundStream> soundStream = SoundStream::OpenFromStream(stream, params))
			return Create(std::move(soundStream));
		else
			return false;
	}

	/*!
	* \brief Pauses the music
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::Pause()
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcePause(m_source);
	}

	/*!
	* \brief Plays the music
	*
	* Plays/Resume the music.
	* If the music is currently playing, resets the playing offset to the beginning offset.
	* If the music is currently paused,  resumes the playing.
	* If the music is currently stopped, starts the playing at the previously set playing offset.
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::Play()
	{
		NazaraAssert(m_impl, "Music not created");

		// Maybe we are already playing
		if (m_impl->streaming)
		{
			switch (GetStatus())
			{
				case SoundStatus::Playing:
					SetPlayingOffset(0);
					break;

				case SoundStatus::Paused:
					alSourcePlay(m_source);
					break;

				default:
					break; // We shouldn't be stopped
			}
		}
		else
		{
			std::mutex mutex;
			std::condition_variable cv;

			// Starting streaming thread
			m_impl->streaming = true;

			std::exception_ptr exceptionPtr;

			std::unique_lock<std::mutex> lock(mutex);
			m_impl->thread = std::thread(&Music::MusicThread, this, std::ref(cv), std::ref(mutex), std::ref(exceptionPtr));

			// Wait until thread signal it has properly started (or an error occurred)
			cv.wait(lock);

			if (exceptionPtr)
				std::rethrow_exception(exceptionPtr);
		}
	}

	/*!
	* \brief Changes the playing offset of the music
	*
	* If the music is not playing, this sets the playing offset for the next Play call
	*
	* \param offset The offset in milliseconds
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::SetPlayingOffset(UInt32 offset)
	{
		NazaraAssert(m_impl, "Music not created");

		bool isPlaying = m_impl->streaming;

		if (isPlaying)
			Stop();

		UInt64 sampleOffset = UInt64(offset) * m_impl->sampleRate * GetChannelCount(m_impl->stream->GetFormat()) / 1000ULL;

		m_impl->processedSamples = sampleOffset;
		m_impl->streamOffset = sampleOffset;

		if (isPlaying)
			Play();
	}

	/*!
	* \brief Stops the music
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::Stop()
	{
		NazaraAssert(m_impl, "Music not created");

		StopThread();
		SetPlayingOffset(0);
	}

	Music& Music::operator=(Music&&) noexcept = default;

	bool Music::FillAndQueueBuffer(unsigned int buffer)
	{
		std::size_t sampleCount = m_impl->chunkSamples.size();
		std::size_t sampleRead = 0;
		{
			std::lock_guard<std::mutex> lock(m_impl->stream->GetMutex());

			m_impl->stream->Seek(m_impl->streamOffset);

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

			m_impl->streamOffset = m_impl->stream->Tell();
		}

		// Update the buffer (send it to OpenAL) and queue it if we got any data
		if (sampleRead > 0)
		{
			alBufferData(buffer, m_impl->audioFormat, &m_impl->chunkSamples[0], static_cast<ALsizei>(sampleRead*sizeof(Int16)), static_cast<ALsizei>(m_impl->sampleRate));
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		return sampleRead != sampleCount; // End of stream (Does not happen when looping)
	}

	void Music::MusicThread(std::condition_variable& cv, std::mutex& m, std::exception_ptr& err)
	{
		// Allocation of streaming buffers
		std::array<ALuint, NAZARA_AUDIO_STREAMED_BUFFER_COUNT> buffers;
		alGenBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers.data());

		CallOnExit freebuffers([&]
		{
			alDeleteBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers.data());
		});


		try
		{
			for (ALuint buffer : buffers)
			{
				if (FillAndQueueBuffer(buffer))
					break; // We have reached the end of the stream, there is no use to add new buffers
			}
		}
		catch (const std::exception&)
		{
			err = std::current_exception();

			std::unique_lock<std::mutex> lock(m);
			cv.notify_all();
			return;
		}
		
		CallOnExit unqueueBuffers([&]
		{
			// We delete buffers from the stream
			ALint queuedBufferCount;
			alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

			ALuint buffer;
			for (ALint i = 0; i < queuedBufferCount; ++i)
				alSourceUnqueueBuffers(m_source, 1, &buffer);
		});

		alSourcePlay(m_source);
		
		CallOnExit stopSource([&]
		{
			// Stop playing of the sound (in the case where it has not been already done)
			alSourceStop(m_source);
		});

		// Signal we're good
		{
			std::unique_lock<std::mutex> lock(m);
			cv.notify_all();
		} // m & cv no longer exists from here

		// Reading loop (Filling new buffers as playing)
		while (m_impl->streaming)
		{
			// The reading has stopped, we have reached the end of the stream
			SoundStatus status = GetInternalStatus();
			if (status == SoundStatus::Stopped)
			{
				m_impl->streaming = false;
				break;
			}

			{
				std::lock_guard<std::mutex> lock(m_impl->bufferLock);

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
			}

			// We go back to sleep
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void Music::StopThread()
	{
		if (m_impl->streaming)
			m_impl->streaming = false;

		if (m_impl->thread.joinable())
			m_impl->thread.join();
	}
}
