// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_DUMMYAUDIOBUFFER_HPP
#define NAZARA_AUDIO_DUMMYAUDIOBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API DummyAudioBuffer final : public AudioBuffer
	{
		public:
			using AudioBuffer::AudioBuffer;
			DummyAudioBuffer(const DummyAudioBuffer&) = delete;
			DummyAudioBuffer(DummyAudioBuffer&&) = delete;
			~DummyAudioBuffer() = default;

			AudioFormat GetAudioFormat() const;
			Time GetDuration() const;
			UInt64 GetSampleCount() const override;
			UInt64 GetSize() const override;
			UInt32 GetSampleRate() const override;

			bool IsCompatibleWith(const AudioDevice& device) const override;

			bool Reset(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const void* samples) override;

			DummyAudioBuffer& operator=(const DummyAudioBuffer&) = delete;
			DummyAudioBuffer& operator=(DummyAudioBuffer&&) = delete;

		private:
			AudioFormat m_format;
			UInt64 m_sampleCount;
			UInt32 m_sampleRate;
	};
}

#include <Nazara/Audio/DummyAudioBuffer.inl>

#endif // NAZARA_AUDIO_DUMMYAUDIOBUFFER_HPP
