// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIOSOURCE_HPP
#define NAZARA_AUDIO_AUDIOSOURCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class AudioBuffer;
	class AudioDevice;

	class NAZARA_AUDIO_API AudioSource
	{
		public:
			struct OffsetWithLatency;

			inline AudioSource(std::shared_ptr<AudioDevice> device);
			AudioSource(const AudioSource&) = delete;
			AudioSource(AudioSource&&) = delete;
			virtual ~AudioSource();

			virtual void EnableLooping(bool loop) = 0;
			virtual void EnableSpatialization(bool spatialization) = 0;

			inline const std::shared_ptr<AudioDevice>& GetAudioDevice() const;
			virtual float GetAttenuation() const = 0;
			virtual float GetMinDistance() const = 0;
			virtual float GetPitch() const = 0;
			virtual Time GetPlayingOffset() const = 0;
			virtual Vector3f GetPosition() const = 0;
			virtual UInt32 GetSampleOffset() const = 0;
			virtual OffsetWithLatency GetSampleOffsetAndLatency() const = 0;
			virtual Vector3f GetVelocity() const = 0;
			virtual SoundStatus GetStatus() const = 0;
			virtual float GetVolume() const = 0;

			virtual bool IsLooping() const = 0;
			virtual bool IsSpatializationEnabled() const = 0;

			virtual void QueueBuffer(std::shared_ptr<AudioBuffer> audioBuffer) = 0;

			virtual void Pause() = 0;
			virtual void Play() = 0;

			virtual void SetAttenuation(float attenuation) = 0;
			virtual void SetBuffer(std::shared_ptr<AudioBuffer> audioBuffer) = 0;
			virtual void SetMinDistance(float minDistance) = 0;
			virtual void SetPitch(float pitch) = 0;
			virtual void SetPlayingOffset(Time offset) = 0;
			virtual void SetPosition(const Vector3f& position) = 0;
			virtual void SetSampleOffset(UInt32 offset) = 0;
			virtual void SetVelocity(const Vector3f& velocity) = 0;
			virtual void SetVolume(float volume) = 0;

			virtual void Stop() = 0;

			virtual std::shared_ptr<AudioBuffer> TryUnqueueProcessedBuffer() = 0;

			virtual void UnqueueAllBuffers() = 0;

			AudioSource& operator=(const AudioSource&) = delete;
			AudioSource& operator=(AudioSource&&) = delete;

			struct OffsetWithLatency
			{
				UInt64 sampleOffset;
				Time sourceLatency;
			};

		private:
			std::shared_ptr<AudioDevice> m_device;
	};
}

#include <Nazara/Audio/AudioSource.inl>

#endif // NAZARA_AUDIO_AUDIOSOURCE_HPP
