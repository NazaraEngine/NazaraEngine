// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/ThreadExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <array>
#include <chrono>
#include <optional>
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

	Music::Music() :
	Music(*Audio::Instance()->GetDefaultDevice())
	{
	}
	
	Music::Music(AudioDevice& device) :
	SoundEmitter(device),
	m_streaming(false),
	m_bufferCount(2),
	m_looping(false)
	{
	}

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
	* \return true if creation was successful
	*
	* \param soundStream Sound stream which is the source for the music
	*/
	bool Music::Create(std::shared_ptr<SoundStream> soundStream)
	{
		NazaraAssert(soundStream, "Invalid stream");

		Destroy();

		AudioFormat format = soundStream->GetFormat();

		m_sampleRate = soundStream->GetSampleRate();
		m_audioFormat = soundStream->GetFormat();
		m_chunkSamples.resize(GetChannelCount(format) * m_sampleRate); // One second of samples
		m_stream = std::move(soundStream);

		SeekToSampleOffset(0);

		return true;
	}

	/*!
	* \brief Destroys the current music and frees resources
	*
	* \remark If the Music is playing, it is stopped first.
	*/
	void Music::Destroy()
	{
		StopThread();
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
		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		m_looping = loop;
	}

	/*!
	* \brief Gets the duration of the music
	* \return Duration of the music in milliseconds
	*
	* \remark Music must be valid when calling this function
	*/
	Time Music::GetDuration() const
	{
		NazaraAssert(m_stream, "Music not created");

		return m_stream->GetDuration();
	}

	/*!
	* \brief Gets the format of the music
	* \return Enumeration of type AudioFormat (mono, stereo, ...)
	*
	* \remark Music must be valid when calling this function
	*/
	AudioFormat Music::GetFormat() const
	{
		NazaraAssert(m_stream, "Music not created");

		return m_stream->GetFormat();
	}

	/*!
	* \brief Gets the current playing offset of the music
	* \return Time offset
	*/
	Time Music::GetPlayingOffset() const
	{
		NazaraAssert(m_stream, "Music not created");

		if (!m_streaming)
			return Time::Zero();

		// Prevent music thread from enqueuing new buffers while we're getting the count
		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		Time playingOffset = m_source->GetPlayingOffset();
		Time processedTime = Time::Microseconds(1'000'000ll * m_processedSamples / (GetChannelCount(m_stream->GetFormat()) * m_sampleRate));
		playingOffset += processedTime;

		Time sampleCount = m_stream->GetDuration();
		if (playingOffset > sampleCount)
		{
			if (m_looping)
				playingOffset %= sampleCount;
			else
				playingOffset = Time::Zero(); //< stopped
		}

		return playingOffset;
	}

	/*!
	* \brief Gets the number of samples in the music
	* \return Count of samples (number of seconds * sample rate * channel count)
	*
	* \remark Music must be valid when calling this function
	*/
	UInt64 Music::GetSampleCount() const
	{
		NazaraAssert(m_stream, "Music not created");

		return m_stream->GetSampleCount();
	}

	/*!
	* \brief Gets the current offset in the music
	* \return Offset in samples
	*/
	UInt64 Music::GetSampleOffset() const
	{
		NazaraAssert(m_stream, "Music not created");

		if (!m_streaming)
			return 0;

		// Prevent music thread from enqueuing new buffers while we're getting the count
		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		UInt64 sampleOffset = m_processedSamples + m_source->GetSampleOffset();
		UInt64 sampleCount = m_stream->GetSampleCount();
		if (sampleOffset > sampleCount)
		{
			if (m_looping)
				sampleOffset %= sampleCount;
			else
				sampleOffset = 0; //< stopped
		}

		return sampleOffset;
	}

	/*!
	* \brief Gets the rates of sample in the music
	* \return Rate of sample in Hertz (Hz)
	*
	* \remark Music must be valid when calling this function
	*/
	UInt32 Music::GetSampleRate() const
	{
		NazaraAssert(m_stream, "Music not created");

		return m_sampleRate;
	}

	/*!
	* \brief Gets the status of the music
	* \return Enumeration of type SoundStatus (Playing, Stopped, ...)
	*
	* \remark Music must be valid when calling this function
	*/
	SoundStatus Music::GetStatus() const
	{
		NazaraAssert(m_stream, "Music not created");

		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		SoundStatus status = m_source->GetStatus();

		// To compensate any delays (or the timelaps between Play() and the thread startup)
		if (m_streaming && status == SoundStatus::Stopped)
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
		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		return m_looping;
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
		std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

		m_source->Pause();
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
		NazaraAssert(m_stream, "Music not created");

		// Maybe we are already playing
		if (m_streaming)
		{
			std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

			switch (GetStatus())
			{
				case SoundStatus::Playing:
					SeekToSampleOffset(0);
					break;

				case SoundStatus::Paused:
					m_source->Play();
					break;

				default:
					break; // We shouldn't be stopped
			}
		}
		else
		{
			// Ensure we're restarting
			StopThread();

			// Special case of SetPlayingOffset(end) before Play(), restart from beginning
			if (m_streamOffset >= m_stream->GetSampleCount())
				m_streamOffset = 0;

			StartThread(false);
		}
	}

	/*!
	* \brief Changes the playing offset of the music
	*
	* If the music is not playing, this sets the playing offset for the next Play call
	*
	* \param offset The offset in samples
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::SeekToSampleOffset(UInt64 offset)
	{
		NazaraAssert(m_stream, "Music not created");

		bool isPlaying = m_streaming;
		bool isPaused = GetStatus() == SoundStatus::Paused;

		if (isPlaying)
			StopThread();

		UInt64 sampleOffset = offset * GetChannelCount(m_stream->GetFormat());

		m_processedSamples = sampleOffset;
		m_streamOffset = sampleOffset;

		if (isPlaying)
			StartThread(isPaused);
	}

	/*!
	* \brief Stops the music
	*
	* \remark Music must be valid when calling this function
	*/
	void Music::Stop()
	{
		StopThread();
		SeekToSampleOffset(0);
	}

	bool Music::FillAndQueueBuffer(std::shared_ptr<AudioBuffer> buffer)
	{
		std::size_t sampleCount = m_chunkSamples.size();
		std::size_t sampleRead = 0;
		{
			std::lock_guard<std::mutex> lock(m_stream->GetMutex());

			m_stream->Seek(m_streamOffset);

			// Fill the buffer by reading from the stream
			for (;;)
			{
				sampleRead += m_stream->Read(&m_chunkSamples[sampleRead], sampleCount - sampleRead);
				if (sampleRead < sampleCount && m_looping)
				{
					// In case we read less than expected, assume we reached the end of the stream and seek back to the beginning
					m_stream->Seek(0);
					continue;
				}

				// Either we read the size we wanted, either we're not looping
				break;
			}

			m_streamOffset = m_stream->Tell();
		}

		// Update the buffer on the AudioDevice and queue it if we got any data
		if (sampleRead > 0)
		{
			buffer->Reset(m_audioFormat, sampleRead, m_sampleRate, &m_chunkSamples[0]);
			m_source->QueueBuffer(buffer);
		}

		return sampleRead != sampleCount; // End of stream (Does not happen when looping)
	}

	void Music::MusicThread(std::condition_variable& cv, std::mutex& m, std::exception_ptr& err, bool startPaused)
	{
		SetCurrentThreadName("MusicThread");

		std::optional<std::lock_guard<std::recursive_mutex>> exitLock;

		// Allocation of streaming buffers
		CallOnExit unqueueBuffers([&]
		{
			m_source->UnqueueAllBuffers();
		});

		try
		{
			for (std::size_t i = 0; i < m_bufferCount; ++i)
			{
				std::shared_ptr<AudioBuffer> buffer = m_source->GetAudioDevice()->CreateBuffer();

				if (FillAndQueueBuffer(std::move(buffer)))
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

		m_source->Play();
		if (startPaused)
		{
			// little hack to start paused (required by SetPlayingOffset)
			m_source->Pause();
			m_source->SetSampleOffset(0);
		}

		CallOnExit stopSource([&]
		{
			// Stop playing of the sound (in the case where it has not been already done)
			m_source->Stop();
		});

		// Signal we're good
		{
			std::unique_lock<std::mutex> lock(m);
			m_musicStarted = true;
			cv.notify_all();
		} // m & cv no longer exists from here

		// From now, the source can be accessed from another thread, lock it before others destructors
		CallOnExit lockSource([&]
		{
			exitLock.emplace(m_sourceLock);
		});

		// Reading loop (Filling new buffers as playing)
		while (m_streaming)
		{
			// Wait until buffers are processed
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			std::lock_guard<std::recursive_mutex> lock(m_sourceLock);

			SoundStatus status = m_source->GetStatus();
			if (status == SoundStatus::Stopped)
			{
				// The reading has stopped, we have reached the end of the stream
				m_streaming = false;
				break;
			}

			// We treat read buffers
			while (std::shared_ptr<AudioBuffer> buffer = m_source->TryUnqueueProcessedBuffer())
			{
				m_processedSamples += buffer->GetSampleCount();

				if (FillAndQueueBuffer(std::move(buffer)))
					break;
			}
		}
	}

	void Music::StartThread(bool startPaused)
	{
		std::mutex mutex;
		std::condition_variable cv;

		// Starting streaming thread
		m_streaming = true;

		std::exception_ptr exceptionPtr;

		std::unique_lock<std::mutex> lock(mutex);
		m_musicStarted = false;
		m_thread = std::thread(&Music::MusicThread, this, std::ref(cv), std::ref(mutex), std::ref(exceptionPtr), startPaused);

		// Wait until thread signal it has properly started (or an error occurred)
		cv.wait(lock, [&] { return exceptionPtr || m_musicStarted; });

		if (exceptionPtr)
			std::rethrow_exception(exceptionPtr);
	}

	void Music::StopThread()
	{
		if (m_streaming)
			m_streaming = false;

		if (m_thread.joinable())
			m_thread.join();
	}
}
