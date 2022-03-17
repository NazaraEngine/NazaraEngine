// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALBuffer.hpp>
#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	OpenALBuffer::~OpenALBuffer()
	{
		GetDevice().MakeContextCurrent();

		m_library.alDeleteBuffers(1, &m_bufferId);
	}

	UInt32 OpenALBuffer::GetSampleCount() const
	{
		GetDevice().MakeContextCurrent();

		ALint bits, size;
		m_library.alGetBufferi(m_bufferId, AL_BITS, &bits);
		m_library.alGetBufferi(m_bufferId, AL_SIZE, &size);

		UInt32 sampleCount = 0;
		if (bits != 0)
			sampleCount += (8 * SafeCast<UInt32>(size)) / SafeCast<UInt32>(bits);

		return sampleCount;
	}

	UInt32 OpenALBuffer::GetSize() const
	{
		GetDevice().MakeContextCurrent();

		ALint size;
		m_library.alGetBufferi(m_bufferId, AL_SIZE, &size);

		return SafeCast<UInt32>(size);
	}

	UInt32 OpenALBuffer::GetSampleRate() const
	{
		GetDevice().MakeContextCurrent();

		ALint sampleRate;
		m_library.alGetBufferi(m_bufferId, AL_FREQUENCY, &sampleRate);

		return SafeCast<UInt32>(sampleRate);
	}

	bool OpenALBuffer::IsCompatibleWith(const AudioDevice& device) const
	{
		// OpenAL buffers are shared among contexts and thus devices
		return device.GetSubSystemIdentifier() == &m_library;
	}

	bool OpenALBuffer::Reset(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const void* samples)
	{
		OpenALDevice& device = GetDevice();

		ALenum alFormat = device.TranslateAudioFormat(format);
		if (!alFormat)
		{
			NazaraError("unsupported format");
			return false;
		}

		device.MakeContextCurrent();

		// We empty the error stack
		while (m_library.alGetError() != AL_NO_ERROR);

		// TODO: Use SafeCast
		m_library.alBufferData(m_bufferId, alFormat, samples, static_cast<ALsizei>(sampleCount * sizeof(Int16)), static_cast<ALsizei>(sampleRate));

		if (ALenum lastError = m_library.alGetError(); lastError != AL_NO_ERROR)
		{
			NazaraError("failed to reset OpenAL buffer: " + std::to_string(lastError));
			return false;
		}

		return true;
	}

	OpenALDevice& OpenALBuffer::GetDevice()
	{
		return SafeCast<OpenALDevice&>(*GetAudioDevice());
	}

	const OpenALDevice& OpenALBuffer::GetDevice() const
	{
		return SafeCast<OpenALDevice&>(*GetAudioDevice());
	}
}
