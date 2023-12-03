// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alDeleteBuffers(1, &m_bufferId);
	}

	UInt64 OpenALBuffer::GetSampleCount() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint bits, size;
		device.alGetBufferi(m_bufferId, AL_BITS, &bits);
		device.alGetBufferi(m_bufferId, AL_SIZE, &size);

		UInt64 sampleCount = 0;
		if (bits != 0)
			sampleCount += (8 * SafeCast<UInt64>(size)) / SafeCast<UInt64>(bits);

		return sampleCount;
	}

	UInt64 OpenALBuffer::GetSize() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint size;
		device.alGetBufferi(m_bufferId, AL_SIZE, &size);

		return SafeCast<UInt64>(size);
	}

	UInt32 OpenALBuffer::GetSampleRate() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint sampleRate;
		device.alGetBufferi(m_bufferId, AL_FREQUENCY, &sampleRate);

		return SafeCast<UInt32>(sampleRate);
	}

	bool OpenALBuffer::IsCompatibleWith(const AudioDevice& device) const
	{
		// OpenAL buffers are shared among contexts and thus devices
		return device.GetSubSystemIdentifier() == &GetDevice().GetLibrary();
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
		while (device.alGetError() != AL_NO_ERROR);

		device.alBufferData(m_bufferId, alFormat, samples, SafeCast<ALsizei>(sampleCount * sizeof(Int16)), SafeCast<ALsizei>(sampleRate));

		if (ALenum lastError = device.alGetError(); lastError != AL_NO_ERROR)
		{
			NazaraErrorFmt("failed to reset OpenAL buffer: {0}", std::to_string(lastError));
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
