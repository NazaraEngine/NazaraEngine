// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

///FIXME: Adapt the creation

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::SoundBuffer
	* \brief Audio class that represents a buffer for sound
	*
	* \remark Module Audio needs to be initialized to use this class
	*/

	/*!
	* \brief Checks whether the parameters for the buffer' sound are correct
	* \return true If parameters are valid
	*/

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
		UInt64 sampleCount;
		UInt32 sampleRate;
	};

	/*!
	* \brief Constructs a SoundBuffer object
	*
	* \param format Format for the audio
	* \param sampleCount Number of samples
	* \param sampleRate Rate of samples
	* \param samples Samples raw data
	*
	* \remark Produces a NazaraError if creation went wrong with NAZARA_AUDIO_SAFE defined
	* \remark Produces a std::runtime_error if creation went wrong with NAZARA_AUDIO_SAFE defined
	*
	* \see Create
	*/
	SoundBuffer::SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples)
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

	/*!
	* \brief Destructs the object and calls Destroy
	*
	* \see Destroy
	*/
	SoundBuffer::~SoundBuffer()
	{
		OnSoundBufferRelease(this);

		Destroy();
	}

	/*!
	* \brief Creates the SoundBuffer object
	* \return true if creation is successful
	*
	* \param format Format for the audio
	* \param sampleCount Number of samples
	* \param sampleRate Rate of samples
	* \param samples Samples raw data
	*
	* \remark Produces a NazaraError if creation went wrong with NAZARA_AUDIO_SAFE defined,
	* this could happen if parameters are invalid or creation of OpenAL buffers failed
	*/
	bool SoundBuffer::Create(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples)
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

		// We empty the error stack
		while (alGetError() != AL_NO_ERROR);

		ALuint buffer;
		alGenBuffers(1, &buffer);
		if (alGetError() != AL_NO_ERROR)
		{
			NazaraError("Failed to create OpenAL buffer");
			return false;
		}

		CallOnExit clearBufferOnExit([buffer] () { alDeleteBuffers(1, &buffer); });

		alBufferData(buffer, OpenAL::AudioFormat[format], samples, static_cast<ALsizei>(sampleCount*sizeof(Int16)), static_cast<ALsizei>(sampleRate));

		if (alGetError() != AL_NO_ERROR)
		{
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

		clearBufferOnExit.Reset();

		return true;
	}

	/*!
	* \brief Destroys the current sound buffer and frees resources
	*/

	void SoundBuffer::Destroy()
	{
		if (m_impl)
		{
			OnSoundBufferDestroy(this);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	/*!
	* \brief Gets the duration of the sound buffer
	* \return Duration of the sound buffer in milliseconds
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	UInt32 SoundBuffer::GetDuration() const
	{
		NazaraAssert(m_impl, "Sound buffer not created");

		return m_impl->duration;
	}

	/*!
	* \brief Gets the format of the sound buffer
	* \return Enumeration of type AudioFormat (mono, stereo, ...)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/

	AudioFormat SoundBuffer::GetFormat() const
	{
		NazaraAssert(m_impl, "Sound buffer not created");

		return m_impl->format;
	}

	/*!
	* \brief Gets the internal raw samples
	* \return Pointer to raw data
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	const Int16* SoundBuffer::GetSamples() const
	{
		NazaraAssert(m_impl, "Sound buffer not created");

		return m_impl->samples.get();
	}

	/*!
	* \brief Gets the number of samples in the sound buffer
	* \return Count of samples (number of seconds * sample rate * channel count)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	UInt64 SoundBuffer::GetSampleCount() const
	{
		NazaraAssert(m_impl, "Sound buffer not created");

		return m_impl->sampleCount;
	}

	/*!
	* \brief Gets the rates of sample in the sound buffer
	* \return Rate of sample in Hertz (Hz)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	UInt32 SoundBuffer::GetSampleRate() const
	{
		NazaraAssert(m_impl, "Sound buffer not created");

		return m_impl->sampleRate;
	}

	/*!
	* \brief Checks whether the sound buffer is valid
	* \return true if it is the case
	*/

	bool SoundBuffer::IsValid() const
	{
		return m_impl != nullptr;
	}

	/*!
	* \brief Loads the sound buffer from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the sound buffer
	*/
	bool SoundBuffer::LoadFromFile(const String& filePath, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromFile(this, filePath, params);
	}

	/*!
	* \brief Loads the sound buffer from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the sound buffer
	*/
	bool SoundBuffer::LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromMemory(this, data, size, params);
	}

	/*!
	* \brief Loads the sound buffer from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the sound buffer
	* \param params Parameters for the sound buffer
	*/
	bool SoundBuffer::LoadFromStream(Stream& stream, const SoundBufferParams& params)
	{
		return SoundBufferLoader::LoadFromStream(this, stream, params);
	}

	/*!
	* \brief Checks whether the format is supported by the engine
	* \return true if it is the case
	*
	* \param format Format to check
	*/
	bool SoundBuffer::IsFormatSupported(AudioFormat format)
	{
		return Audio::IsFormatSupported(format);
	}

	/*!
	* \brief Gets the internal OpenAL buffer
	* \return The index of the OpenAL buffer
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
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

	/*!
	* \brief Initializes the libraries and managers
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraError if sub-initialization failed
	*/
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

	/*!
	* \brief Uninitializes the libraries and managers
	*/
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
