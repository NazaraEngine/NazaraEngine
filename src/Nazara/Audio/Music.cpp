// Copyright (C) 2014 Jérôme Leclercq
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

bool NzMusicParams::IsValid() const
{
	return true;
}

struct NzMusicImpl
{
	ALenum audioFormat;
	std::unique_ptr<NzSoundStream> stream;
	std::vector<nzInt16> chunkSamples;
	NzThread thread;
	bool loop = false;
	bool paused = false;
	bool streaming = false;
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
	m_impl->audioFormat = NzOpenAL::AudioFormat[format];
	m_impl->chunkSamples.resize(format * m_impl->sampleRate); // Une seconde de samples
	m_impl->stream.reset(soundStream);

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

nzAudioFormat NzMusic::GetFormat() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Music not created");
		return nzAudioFormat_Unknown;
	}
	#endif

	return m_impl->stream->GetFormat();
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

	if (m_impl->streaming && status == nzSoundStatus_Stopped)
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
	alSourcePause(m_source);
}

void NzMusic::Play()
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
		if (GetStatus() != nzSoundStatus_Playing)
			alSourcePlay(m_source);

		return;
	}

	m_impl->stream->Seek(0);
	m_impl->streaming = true;
	m_impl->thread = NzThread(&NzMusic::MusicThread, this);

	return;
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

	if (m_impl->streaming)
	{
		m_impl->streaming = false;
		m_impl->thread.Join();
	}
}

bool NzMusic::FillAndQueueBuffer(unsigned int buffer)
{
	unsigned int sampleCount = m_impl->chunkSamples.size();
	unsigned int sampleRead = 0;

	for (;;)
	{
		sampleRead += m_impl->stream->Read(&m_impl->chunkSamples[sampleRead], sampleCount - sampleRead);
		if (sampleRead < sampleCount && m_impl->loop)
			m_impl->stream->Seek(0);
		else
			break;
	}

	if (sampleRead > 0)
	{
		alBufferData(buffer, m_impl->audioFormat, &m_impl->chunkSamples[0], sampleRead*sizeof(nzInt16), m_impl->sampleRate);
		alSourceQueueBuffers(m_source, 1, &buffer);
	}

	return sampleRead != sampleCount; // Fin du fichier (N'arrive pas en cas de loop)
}

void NzMusic::MusicThread()
{
	ALuint buffers[NAZARA_AUDIO_STREAMED_BUFFER_COUNT];
	alGenBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);

	for (unsigned int i = 0; i < NAZARA_AUDIO_STREAMED_BUFFER_COUNT; ++i)
	{
		if (FillAndQueueBuffer(buffers[i])) // Fin du fichier ?
			break; // Nous avons atteint la fin du fichier, inutile de rajouter des buffers
	}

	alSourcePlay(m_source);

	while (m_impl->streaming)
	{
		nzSoundStatus status = GetInternalStatus();
		if (status == nzSoundStatus_Stopped)
		{
			m_impl->streaming = false;
			break;
		}

		ALint processedCount = 0;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedCount);

		ALuint buffer;
		while (processedCount--)
		{
			alSourceUnqueueBuffers(m_source, 1, &buffer);
			if (FillAndQueueBuffer(buffer))
				break;
		}

		NzThread::Sleep(50);
	}

	alSourceStop(m_source);

	ALint queuedBufferCount;
	alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

	ALuint buffer;
	for (ALint i = 0; i < queuedBufferCount; ++i)
		alSourceUnqueueBuffers(m_source, 1, &buffer);

	alDeleteBuffers(NAZARA_AUDIO_STREAMED_BUFFER_COUNT, buffers);
}

NzMusicLoader::LoaderList NzMusic::s_loaders;
