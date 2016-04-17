// Copyright (C) 2015 Jérôme Leclercq
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

namespace Nz
{
	bool SoundBufferParams::IsValid() const
	{
		return true;
	}

	struct SoundBufferImpl
	{
		ALuint buffer;
		AudioFormat format;
		UInt32 duration;
		std::unique_ptr<Int16[]> samples;
		UInt32 sampleCount;
		UInt32 sampleRate;
	};

	SoundBuffer::SoundBuffer(AudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const Int16* samples)
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

	SoundBuffer::~SoundBuffer()
	{
		OnSoundBufferRelease(this);

		Destroy();
	}

	bool SoundBuffer::Create(AudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const Int16* samples)
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

		alBufferData(buffer, OpenAL::AudioFormat[format], samples, sampleCount*sizeof(Int16), sampleRate);

		if (alGetError() != AL_NO_ERROR)
		{
			alDeleteBuffers(1, &buffer);

			NazaraError("Failed to set OpenAL buffer");
			return false;
		}

		m_impl = new SoundBufferImpl;
		m_impl->buffer = buffer;
		m_impl->duration = static_cast<UInt32>((1000ULL*sampleCount / (format * sampleRate)));
		m_impl->format = format;
		m_impl->sampleCount = sampleCount;
		m_impl->sampleRate = sampleRate;
		m_impl->samples.reset(new Int16[sampleCount]);
		std::memcpy(&m_impl->samples[0], samples, sampleCount*sizeof(Int16));

		return true;
	}

	void SoundBuffer::Destroy()
	{
		if (m_impl)
		{
			OnSoundBufferDestroy(this);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	UInt32 SoundBuffer::GetDuration() const
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

	AudioFormat SoundBuffer::GetFormat() const
	{
		#if NAZARA_AUDIO_SAFE
		if (!m_impl)
		{
			NazaraError("Sound buffer not created");
			return AudioFormat_Unknown;
		}
		#endif

		return m_impl->format;
	}

	const Int16* SoundBuffer::GetSamples() const
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

	unsigned int SoundBuffer::GetSampleCount() const
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

	unsigned int SoundBuffer::GetSampleRate() const
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

	bool SoundBuffer::IsValid() const
	{
		return m_impl != nullptr;
	}

	bool SoundBuffer::LoadFromFile(const String& filePath, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromFile(this, filePath, params);
	}

	bool SoundBuffer::LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromMemory(this, data, size, params);
	}

	bool SoundBuffer::LoadFromStream(Stream& stream, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromStream(this, stream, params);
	}

	bool SoundBuffer::IsFormatSupported(AudioFormat format)
	{
		return Audio::IsFormatSupported(format);
	}

	unsigned int SoundBuffer::GetOpenALBuffer() const
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

	bool SoundBuffer::Initialize()
	{
		if (!SoundBufferLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!SoundBufferManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		return true;
	}

	void SoundBuffer::Uninitialize()
	{
		SoundBufferManager::Uninitialize();
		SoundBufferLibrary::Uninitialize();
	}

	SoundBufferLibrary::LibraryMap SoundBuffer::s_library;
	SoundBufferLoader::LoaderList SoundBuffer::s_loaders;
	SoundBufferManager::ManagerMap SoundBuffer::s_managerMap;
	SoundBufferManager::ManagerParams SoundBuffer::s_managerParameters;
}
