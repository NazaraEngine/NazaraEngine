// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Thread.hpp>
#include <vector>
#include <AL/al.h>
#include <Nazara/Audio/Debug.hpp>

bool NzMusicParams::IsValid() const
{
	return true;
}

struct NzMusicImpl
{
	ALenum audioFormat;
	std::vector<nzInt16> chunkSamples;
	NzSoundStream* stream;
	NzThread thread;
	bool loop = false;
	bool playing = false;
	bool paused = false;
	unsigned int sampleRate;
};

NzMusic::~NzMusic()
{
	Destroy();
}

bool NzMusic::Create(NzSoundStream* soundStream)
{
	Destroy();

	#if NAZARA_AUDIO_SAFE
	if (!soundStream)
	{
		NazaraError("Sound stream must be valid");
		return false;
	}
	#endif

	nzAudioFormat format = soundStream->GetFormat();

	m_impl = new NzMusicImpl;
	m_impl->sampleRate = soundStream->GetSampleRate();
	m_impl->audioFormat = NzAudio::GetOpenALFormat(format);
	m_impl->chunkSamples.resize(format * m_impl->sampleRate); // Une seconde de samples
	m_impl->stream = soundStream;

	return true;
}

void NzMusic::Destroy()
{
	if (m_impl)
	{
		Stop();

		delete m_impl;
		m_impl = nullptr;
	}
}

void NzMusic::EnableLooping(bool loop)
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

nzUInt32 NzMusic::GetDuration() const
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

nzUInt32 NzMusic::GetPlayingOffset() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Music not created");
		return 0;
	}
	#endif

	return 0;
}

nzSoundStatus NzMusic::GetStatus() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Music not created");
		return nzSoundStatus_Stopped;
	}
	#endif

	nzSoundStatus status = GetInternalStatus();

	if (m_impl->playing && status == nzSoundStatus_Stopped)
		status = nzSoundStatus_Playing;

	return status;
}

bool NzMusic::IsLooping() const
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

bool NzMusic::OpenFromFile(const NzString& filePath, const NzMusicParams& params)
{
	return NzMusicLoader::LoadFromFile(this, filePath, params);
}

bool NzMusic::OpenFromMemory(const void* data, std::size_t size, const NzMusicParams& params)
{
	return NzMusicLoader::LoadFromMemory(this, data, size, params);
}

bool NzMusic::OpenFromStream(NzInputStream& stream, const NzMusicParams& params)
{
	return NzMusicLoader::LoadFromStream(this, stream, params);
}

void NzMusic::Pause()
{
	return;
}

bool NzMusic::Play()
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Music not created");
		return false;
	}
	#endif

	/*if (m_impl->playing)
	{
		if (m_impl->paused)
			alSourcePlay(m_source);
		else
			// On repositionne au début
			m_impl->stream->Seek(0);

		return true;
	}*/

	m_impl->playing = true;
	m_impl->thread = NzThread(&NzMusic::MusicThread, this);

	return true;
}

void NzMusic::Stop()
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Music not created");
		return;
	}
	#endif

	if (m_impl->playing)
	{
		m_impl->playing = false;
		m_impl->thread.Join();
	}
}

bool NzMusic::FillBuffer(unsigned int buffer)
{
	unsigned int sampleRead = m_impl->stream->Read(&m_impl->chunkSamples[0], m_impl->chunkSamples.size());
	if (sampleRead > 0)
		alBufferData(buffer, m_impl->audioFormat, &m_impl->chunkSamples[0], sampleRead*sizeof(nzInt16), m_impl->sampleRate);

	return sampleRead != m_impl->chunkSamples.size(); // Fin du fichier
}

void NzMusic::MusicThread()
{
	ALuint buffers[NAZARA_AUDIO_STREAMEDBUFFERCOUNT];
	alGenBuffers(NAZARA_AUDIO_STREAMEDBUFFERCOUNT, buffers);

	for (unsigned int i = 0; i < NAZARA_AUDIO_STREAMEDBUFFERCOUNT; ++i)
	{
		FillBuffer(buffers[i]);
		alSourceQueueBuffers(m_source, 1, &buffers[i]);
	}

	alSourcePlay(m_source);

	while (m_impl->playing)
	{
		nzSoundStatus status = GetInternalStatus();
		if (status == nzSoundStatus_Stopped)
		{
			NazaraError("Stopped !");
			if (m_impl->loop)
			{
				m_impl->stream->Seek(0);
				for (unsigned int i = 0; i < NAZARA_AUDIO_STREAMEDBUFFERCOUNT; ++i)
				{
					FillBuffer(buffers[i]);
					alSourceQueueBuffers(m_source, 1, &buffers[i]);
				}

				alSourcePlay(m_source);
			}
			else
				m_impl->playing = false;

			break;
		}

		ALint processedCount = 0;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedCount);

		if (processedCount > 0)
			NazaraWarning(NzString::Number(processedCount));

		ALuint buffer;
		while (processedCount--)
		{
			alSourceUnqueueBuffers(m_source, 1, &buffer);
			FillBuffer(buffer);
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		NzThread::Sleep(NAZARA_AUDIO_STREAMEDBUFFERCOUNT*500);
	}

	ALint queuedBufferCount;
	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

	ALuint buffer;
	for (ALint i = 0; i < queuedBufferCount; ++i)
		alSourceUnqueueBuffers(m_source, 1, &buffer);

	alDeleteBuffers(NAZARA_AUDIO_STREAMEDBUFFERCOUNT, buffers);
}

NzMusicLoader::LoaderList NzMusic::s_loaders;
