// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Gets the duration of the sound buffer
	* \return Duration of the sound buffer in milliseconds
	*/
	inline Time SoundBuffer::GetDuration() const
	{
		return m_duration;
	}

	/*!
	* \brief Gets the format of the sound buffer
	* \return Enumeration of type AudioFormat (mono, stereo, ...)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	inline AudioFormat SoundBuffer::GetFormat() const
	{
		return m_format;
	}

	/*!
	* \brief Gets the internal raw samples
	* \return Pointer to raw data
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	inline const Int16* SoundBuffer::GetSamples() const
	{
		return m_samples.get();
	}

	/*!
	* \brief Gets the number of samples in the sound buffer
	* \return Count of samples (number of seconds * sample rate * channel count)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	inline UInt64 SoundBuffer::GetSampleCount() const
	{
		return m_sampleCount;
	}

	/*!
	* \brief Gets the rates of sample in the sound buffer
	* \return Rate of sample in Hertz (Hz)
	*
	* \remark Produces a NazaraError if there is no sound buffer with NAZARA_AUDIO_SAFE defined
	*/
	inline UInt32 SoundBuffer::GetSampleRate() const
	{
		return m_sampleRate;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
