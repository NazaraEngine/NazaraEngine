// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIOBUFFER_HPP
#define NAZARA_AUDIO_AUDIOBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <memory>

namespace Nz
{
	class AudioDevice;

	class NAZARA_AUDIO_API AudioBuffer
	{
		public:
			inline AudioBuffer(std::shared_ptr<AudioDevice> device);
			AudioBuffer(const AudioBuffer&) = default;
			AudioBuffer(AudioBuffer&&) = default;
			virtual ~AudioBuffer();

			inline const std::shared_ptr<AudioDevice>& GetAudioDevice() const;
			virtual UInt32 GetSampleCount() const = 0;
			virtual UInt32 GetSize() const = 0;
			virtual UInt32 GetSampleRate() const = 0;

			virtual bool Reset(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const void* samples) = 0;

			AudioBuffer& operator=(const AudioBuffer&) = default;
			AudioBuffer& operator=(AudioBuffer&&) = default;

		private:
			std::shared_ptr<AudioDevice> m_device;
	};
}

#include <Nazara/Audio/AudioBuffer.inl>

#endif // NAZARA_AUDIO_AUDIOBUFFER_HPP
