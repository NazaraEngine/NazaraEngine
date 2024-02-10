// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/DummyAudioBuffer.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	AudioFormat DummyAudioBuffer::GetAudioFormat() const
	{
		return m_format;
	}

	Time DummyAudioBuffer::GetDuration() const
	{
		return Time::Microseconds((1'000'000LL * m_sampleCount / (GetChannelCount(m_format) * m_sampleRate)));
	}

	UInt64 DummyAudioBuffer::GetSampleCount() const
	{
		return m_sampleCount;
	}

	UInt64 DummyAudioBuffer::GetSize() const
	{
		return m_sampleCount * sizeof(Int16);
	}

	UInt32 DummyAudioBuffer::GetSampleRate() const
	{
		return m_sampleRate;
	}

	bool DummyAudioBuffer::IsCompatibleWith(const AudioDevice& device) const
	{
		return GetAudioDevice()->GetSubSystemIdentifier() == device.GetSubSystemIdentifier();
	}

	bool DummyAudioBuffer::Reset(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const void* /*samples*/)
	{
		m_format = format;
		m_sampleCount = sampleCount;
		m_sampleRate = sampleRate;
		return true;
	}
}
