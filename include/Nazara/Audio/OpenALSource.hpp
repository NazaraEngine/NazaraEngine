// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALSOURCE_HPP
#define NAZARA_AUDIO_OPENALSOURCE_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/OpenAL.hpp>

namespace Nz
{
	class OpenALBuffer;
	class OpenALDevice;
	class OpenALLibrary;

	class NAZARA_AUDIO_API OpenALSource final : public AudioSource
	{
		public:
			inline OpenALSource(std::shared_ptr<AudioDevice> device, OpenALLibrary& library, ALuint sourceId);
			OpenALSource(const OpenALSource&) = delete;
			OpenALSource(OpenALSource&&) = delete;
			~OpenALSource();

			void EnableLooping(bool loop) override;
			void EnableSpatialization(bool spatialization) override;

			float GetAttenuation() const override;
			float GetMinDistance() const override;
			float GetPitch() const override;
			Vector3f GetPosition() const override;
			UInt32 GetSampleOffset() const override;
			OffsetWithLatency GetSampleOffsetAndLatency() const override;
			Vector3f GetVelocity() const override;
			SoundStatus GetStatus() const override;
			float GetVolume() const override;

			bool IsLooping() const override;
			bool IsSpatializationEnabled() const override;

			void QueueBuffer(std::shared_ptr<AudioBuffer> audioBuffer) override;

			void Pause() override;
			void Play() override;

			void SetAttenuation(float attenuation) override;
			void SetBuffer(std::shared_ptr<AudioBuffer> audioBuffer) override;
			void SetMinDistance(float minDistance) override;
			void SetPitch(float pitch) override;
			void SetPosition(const Vector3f& position) override;
			void SetSampleOffset(UInt32 offset) override;
			void SetVelocity(const Vector3f& velocity) override;
			void SetVolume(float volume) override;

			void Stop() override;

			std::shared_ptr<AudioBuffer> TryUnqueueProcessedBuffer() override;

			void UnqueueAllBuffers() override;

			OpenALSource& operator=(const OpenALSource&) = delete;
			OpenALSource& operator=(OpenALSource&&) = delete;

		private:
			OpenALDevice& GetDevice();
			const OpenALDevice& GetDevice() const;

			std::shared_ptr<OpenALBuffer> m_currentBuffer;
			std::vector<std::shared_ptr<OpenALBuffer>> m_queuedBuffers;
			ALuint m_sourceId;
			OpenALLibrary& m_library;
	};
}

#include <Nazara/Audio/OpenALSource.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALSOURCE_HPP
