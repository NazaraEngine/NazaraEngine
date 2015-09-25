// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/SoundStream.hpp>
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
		Thread thread;
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
		Destroy();

		#if NAZARA_AUDIO_SAFE
		if (!soundStream)
		{
			NazaraError("Sound stream must be valid");
			return false;
		}
		#endif

		AudioFormat format = soundStream->GetFormat();

		m_impl = new MusicImpl;
		m_impl->sampleRate = soundStream->GetSampleRate();
		m_impl->audioFormat = OpenAL::AudioFormat[format];
		m_impl->chunkSamples.resize(format * m_impl->sampleRate); // Une seconde de samples
		m_impl->stream.reset(soundStream);

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

		///TODO
		return 0;
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

		return m_impl->stream->GetSampleRate();
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

	bool Music::OpenFromStream(InputStream& stream, const MusicParams& params)
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

		// Nous sommes déjà en train de jouer
		if (m_impl->streaming)
		{
			// Peut-être sommes-nous en pause
			if (GetStatus() != SoundStatus_Playing)
				alSourcePlay(m_source);

			return;
		}

		// Lancement du thread de streaming
		m_impl->stream->Seek(0);
		m_impl->streaming = true;
		m_impl->thread = Thread(&Music::MusicThread, this);

		return;
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

		///TODO
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

		// Lecture depuis le stream pour remplir le buffer
		for (;;)
		{
			sampleRead += m_impl->stream->Read(&m_impl->chunkSamples[sampleRead], sampleCount - sampleRead);
			if (sampleRead < sampleCount && !m_impl->loop)
				break; // Fin du stream (On ne boucle pas)

			m_impl->stream->Seek(0); // On boucle au début du stream et on remplit à nouveau
		}

		// Mise à jour du buffer (envoi à OpenAL) et placement dans la file d'attente
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

			// On traite les buffers lus
			ALint processedCount = 0;
			alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedCount);

			ALuint buffer;
			while (processedCount--)
			{
				alSourceUnqueueBuffers(m_source, 1, &buffer);
				if (FillAndQueueBuffer(buffer))
					break;
			}

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
