// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

///FIXME: Adapter la création

bool NzSoundBufferParams::IsValid() const
{
	return true;
}

struct NzSoundBufferImpl
{
	ALuint buffer;
	nzAudioFormat format;
	nzUInt32 duration;
	std::unique_ptr<nzInt16[]> samples;
	unsigned int sampleCount;
	unsigned int sampleRate;
};

NzSoundBuffer::NzSoundBuffer(nzAudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const nzInt16* samples)
{
	Create(format, sampleCount, sampleRate, samples);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create sound buffer");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzSoundBuffer::~NzSoundBuffer()
{
	Destroy();
}

bool NzSoundBuffer::Create(nzAudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const nzInt16* samples)
{
	Destroy();

	#if NAZARA_AUDIO_SAFE
	if (!IsFormatSupported(format))
	{
		NazaraError("Audio format is not supported");
		return false;
	}

	if (sampleCount == 0)
	{
		NazaraError("Sample rate must be different from zero");
		return false;
	}

	if (sampleRate == 0)
	{
		NazaraError("Sample rate must be different from zero");
		return false;
	}

	if (!samples)
	{
		NazaraError("Invalid sample source");
		return false;
	}
	#endif

	// On vide le stack d'erreurs
	while (alGetError() != AL_NO_ERROR);

	ALuint buffer;
	alGenBuffers(1, &buffer);

	if (alGetError() != AL_NO_ERROR)
	{
		NazaraError("Failed to create OpenAL buffer");
		return false;
	}

	alBufferData(buffer, NzOpenAL::AudioFormat[format], samples, sampleCount*sizeof(nzInt16), sampleRate);

	if (alGetError() != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &buffer);

		NazaraError("Failed to set OpenAL buffer");
		return false;
	}

	m_impl = new NzSoundBufferImpl;
	m_impl->buffer = buffer;
	m_impl->duration = (1000*sampleCount / (format * sampleRate));
	m_impl->format = format;
	m_impl->sampleCount = sampleCount;
	m_impl->sampleRate = sampleRate;
	m_impl->samples.reset(new nzInt16[sampleCount]);
	std::memcpy(&m_impl->samples[0], samples, sampleCount*sizeof(nzInt16));

	NotifyCreated();
	return true;
}

void NzSoundBuffer::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		delete m_impl;
		m_impl = nullptr;
	}
}

nzUInt32 NzSoundBuffer::GetDuration() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Sound buffer not created");
		return 0;
	}
	#endif

	return m_impl->duration;
}

nzAudioFormat NzSoundBuffer::GetFormat() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Sound buffer not created");
		return nzAudioFormat_Unknown;
	}
	#endif

	return m_impl->format;
}

const nzInt16* NzSoundBuffer::GetSamples() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Sound buffer not created");
		return nullptr;
	}
	#endif

	return m_impl->samples.get();
}

unsigned int NzSoundBuffer::GetSampleCount() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Sound buffer not created");
		return 0;
	}
	#endif

	return m_impl->sampleCount;
}

unsigned int NzSoundBuffer::GetSampleRate() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_impl)
	{
		NazaraError("Sound buffer not created");
		return 0;
	}
	#endif

	return m_impl->sampleRate;
}

bool NzSoundBuffer::IsValid() const
{
	return m_impl != nullptr;
}

bool NzSoundBuffer::LoadFromFile(const NzString& filePath, const NzSoundBufferParams& params)
{
	return NzSoundBufferLoader::LoadFromFile(this, filePath, params);
}

bool NzSoundBuffer::LoadFromMemory(const void* data, std::size_t size, const NzSoundBufferParams& params)
{
	return NzSoundBufferLoader::LoadFromMemory(this, data, size, params);
}

bool NzSoundBuffer::LoadFromStream(NzInputStream& stream, const NzSoundBufferParams& params)
{
	return NzSoundBufferLoader::LoadFromStream(this, stream, params);
}

bool NzSoundBuffer::IsFormatSupported(nzAudioFormat format)
{
	return NzAudio::IsFormatSupported(format);
}

unsigned int NzSoundBuffer::GetOpenALBuffer() const
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraInternalError("Sound buffer not created");
		return AL_NONE;
	}
	#endif

	return m_impl->buffer;
}

NzSoundBufferLoader::LoaderList NzSoundBuffer::s_loaders;
