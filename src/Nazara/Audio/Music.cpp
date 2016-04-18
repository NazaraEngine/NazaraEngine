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

	Music::~Music()
	{
		Destroy();
	}

	bool Music::Create(SoundStream* soundStream)
	{
		NazaraAssert(soundStream, "Invalid stream");

		Destroy();

		AudioFormat format = soundStream->GetFormat();

		m_impl = new MusicImpl;
		m_impl->sampleRate = soundStream->GetSampleRate();
		m_impl->audioFormat = OpenAL::AudioFormat[format];
		m_impl->chunkSamples.resize(format * m_impl->sampleRate); // Une seconde de samples
		m_impl->stream.reset(soundStream);

		SetPlayingOffset(0);

		return true;
	}

	void Music::Destroy()
	{
		if (m_impl)
		{
			Stop();

			delete m_impl;
			m_impl = nullptr;
		}
	}

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

	UInt32 Music::GetPlayingOffset() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return 0;
		}
		#endif
		
		// Prevent music thread from enqueing new buffers while we're getting the count
		Nz::LockGuard lock(m_impl->bufferLock);

		ALint samples = 0;
		alGetSourcei(m_source, AL_SAMPLE_OFFSET, &samples);

		return static_cast<UInt32>((1000ULL * (samples + (m_impl->processedSamples / m_impl->stream->GetFormat()))) / m_impl->sampleRate);
	}

	UInt32 Music::GetSampleCount() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return 0;
		}
		#endif

		return m_impl->stream->GetSampleCount();
	}

	UInt32 Music::GetSampleRate() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return 0;
		}
		#endif

		return m_impl->sampleRate;
	}

	SoundStatus Music::GetStatus() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Music not created");
			return SoundStatus_Stopped;
		}
		#endif

		SoundStatus status = GetInternalStatus();

		// Pour compenser les éventuels retards (ou le laps de temps entre Play() et la mise en route du thread)
		if (m_impl->streaming && status == SoundStatus_Stopped)
			status = SoundStatus_Playing;

		return status;
	}

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

	bool Music::OpenFromFile(const String& filePath, const MusicParams& params)
	{
		return MusicLoader::LoadFromFile(this, filePath, params);
	}

	bool Music::OpenFromMemory(const void* data, std::size_t size, const MusicParams& params)
	{
		return MusicLoader::LoadFromMemory(this, data, size, params);
	}

	bool Music::OpenFromStream(Stream& stream, const MusicParams& params)
	{
		return MusicLoader::LoadFromStream(this, stream, params);
	}

	void Music::Pause()
	{
		alSourcePause(m_source);
	}

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

	bool Music::FillAndQueueBuffer(unsigned int buffer)
	{
		unsigned int sampleCount = m_impl->chunkSamples.size();
		unsigned int sampleRead = 0;

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
			alBufferData(buffer, m_impl->audioFormat, &m_impl->chunkSamples[0], sampleRead*sizeof(Int16), m_impl->sampleRate);
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		return sampleRead != sampleCount; // Fin du stream (N'arrive pas en cas de loop)
	}

	void Music::MusicThread()
	{
		// Allocation des buffers de streaming
		ALuint buffers[NAZARA_AUDIO_STREAMED_BUFFER_COUNT];
		alGenBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);

		for (unsigned int i = 0; i < NAZARA_AUDIO_STREAMED_BUFFER_COUNT; ++i)
		{
			if (FillAndQueueBuffer(buffers[i]))
				break; // Nous avons atteint la fin du stream, inutile de rajouter des buffers
		}

		alSourcePlay(m_source);

		// Boucle de lecture (remplissage de nouveaux buffers au fur et à mesure)
		while (m_impl->streaming)
		{
			// La lecture s'est arrêtée, nous avons atteint la fin du stream
			SoundStatus status = GetInternalStatus();
			if (status == SoundStatus_Stopped)
			{
				m_impl->streaming = false;
				break;
			}

			Nz::LockGuard lock(m_impl->bufferLock);

			// On traite les buffers lus
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

			// On retourne dormir un peu
			Thread::Sleep(50);
		}

		// Arrêt de la lecture du son (dans le cas où ça ne serait pas déjà fait)
		alSourceStop(m_source);

		// On supprime les buffers du stream
		ALint queuedBufferCount;
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

		ALuint buffer;
		for (ALint i = 0; i < queuedBufferCount; ++i)
			alSourceUnqueueBuffers(m_source, 1, &buffer);

		alDeleteBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);
	}

	MusicLoader::LoaderList Music::s_loaders;
}
