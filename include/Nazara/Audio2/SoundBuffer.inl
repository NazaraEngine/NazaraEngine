// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	/*!
	* \brief Gets the channel map of this sound buffer
	* \return Channel map
	*/
	inline std::span<const AudioChannel> SoundBuffer::GetChannels() const
	{
		return std::span<const AudioChannel>(m_channels.data(), m_channels.size());
	}

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
	* \return Enumeration of type AudioFormat (i16, f32, ...)
	*/
	inline AudioFormat SoundBuffer::GetFormat() const
	{
		return m_format;
	}

	/*!
	* \brief Gets the number of samples in the sound buffer
	* \return Count of samples (number of seconds * sample rate * channel count)
	*/
	inline UInt64 SoundBuffer::GetFrameCount() const
	{
		return m_frameCount;
	}

	inline std::mutex* Nz::SoundBuffer::GetMutex()
	{
		// no need for locking when reading as everything is in memory
		return nullptr;
	}

	/*!
	* \brief Gets the internal raw samples
	* \return Pointer to raw data
	*/
	inline void* SoundBuffer::GetSamples()
	{
		return m_samples.get();
	}

	/*!
	* \brief Gets the internal raw samples
	* \return Pointer to raw data
	*/
	inline const void* SoundBuffer::GetSamples() const
	{
		return m_samples.get();
	}

	/*!
	* \brief Gets the rates of sample in the sound buffer
	* \return Rate of sample in Hertz (Hz)
	*/
	inline UInt32 SoundBuffer::GetSampleRate() const
	{
		return m_sampleRate;
	}
}
