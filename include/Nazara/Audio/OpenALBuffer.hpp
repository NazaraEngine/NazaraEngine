// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALBUFFER_HPP
#define NAZARA_AUDIO_OPENALBUFFER_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/OpenAL.hpp>

namespace Nz
{
	class OpenALDevice;
	class OpenALLibrary;

	class NAZARA_AUDIO_API OpenALBuffer final : public AudioBuffer
	{
		public:
			inline OpenALBuffer(std::shared_ptr<AudioDevice> device, OpenALLibrary& library, ALuint bufferId);
			OpenALBuffer(const OpenALBuffer&) = delete;
			OpenALBuffer(OpenALBuffer&&) = delete;
			~OpenALBuffer();

			inline ALuint GetBufferId() const;
			UInt32 GetSampleCount() const override;
			UInt32 GetSize() const override;
			UInt32 GetSampleRate() const override;

			bool IsCompatibleWith(const AudioDevice& device) const override;

			bool Reset(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const void* samples) override;

			OpenALBuffer& operator=(const OpenALBuffer&) = delete;
			OpenALBuffer& operator=(OpenALBuffer&&) = delete;

		private:
			OpenALDevice& GetDevice();
			const OpenALDevice& GetDevice() const;

			ALuint m_bufferId;
			OpenALLibrary& m_library;
	};
}

#include <Nazara/Audio/OpenALBuffer.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALBUFFER_HPP
