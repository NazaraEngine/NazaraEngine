// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_DUMMYAUDIOSOURCE_HPP
#define NAZARA_AUDIO_DUMMYAUDIOSOURCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Clock.hpp>

namespace Nz
{
	class DummyAudioBuffer;

	class NAZARA_AUDIO_API DummyAudioSource final : public AudioSource
	{
		public:
			inline DummyAudioSource(std::shared_ptr<AudioDevice> device);
			DummyAudioSource(const DummyAudioSource&) = delete;
			DummyAudioSource(DummyAudioSource&&) = delete;
			~DummyAudioSource() = default;

			void EnableLooping(bool loop) override;
			void EnableSpatialization(bool spatialization) override;

			float GetAttenuation() const override;
			float GetMinDistance() const override;
			float GetPitch() const override;
			Time GetPlayingOffset() const override;
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
			void SetPlayingOffset(Time offset) override;
			void SetPosition(const Vector3f& position) override;
			void SetSampleOffset(UInt32 offset) override;
			void SetVelocity(const Vector3f& velocity) override;
			void SetVolume(float volume) override;

			void Stop() override;

			std::shared_ptr<AudioBuffer> TryUnqueueProcessedBuffer() override;

			void UnqueueAllBuffers() override;

			DummyAudioSource& operator=(const DummyAudioSource&) = delete;
			DummyAudioSource& operator=(DummyAudioSource&&) = delete;

		private:
			void RequeueBuffers();
			Time UpdateTime() const;

			mutable std::vector<std::shared_ptr<DummyAudioBuffer>> m_queuedBuffers;
			mutable std::vector<std::shared_ptr<DummyAudioBuffer>> m_processedBuffers;
			mutable MillisecondClock m_playClock;
			mutable SoundStatus m_status;
			Vector3f m_position;
			Vector3f m_velocity;
			bool m_isLooping;
			bool m_isSpatialized;
			float m_attenuation;
			float m_minDistance;
			float m_pitch;
			float m_volume;
	};
}

#include <Nazara/Audio/DummyAudioSource.inl>

#endif // NAZARA_AUDIO_DUMMYAUDIOSOURCE_HPP
