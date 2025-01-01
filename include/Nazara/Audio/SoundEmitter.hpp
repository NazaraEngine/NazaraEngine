// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDEMITTER_HPP
#define NAZARA_AUDIO_SOUNDEMITTER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <limits>

namespace Nz
{
	class AudioDevice;
	class AudioSource;

	class NAZARA_AUDIO_API SoundEmitter
	{
		public:
			SoundEmitter(AudioDevice& audioDevice);
			SoundEmitter(const SoundEmitter&) = delete;
			SoundEmitter(SoundEmitter&&) noexcept = default;
			virtual ~SoundEmitter();

			virtual void EnableLooping(bool loop) = 0;
			void EnableSpatialization(bool spatialization);

			float GetAttenuation() const;
			virtual Time GetDuration() const = 0;
			float GetMinDistance() const;
			float GetPitch() const;
			virtual Time GetPlayingOffset() const = 0;
			Vector3f GetPosition() const;
			virtual UInt64 GetSampleOffset() const = 0;
			virtual UInt32 GetSampleRate() const = 0;
			Vector3f GetVelocity() const;
			virtual SoundStatus GetStatus() const = 0;
			float GetVolume() const;

			virtual bool IsLooping() const = 0;
			inline bool IsPlaying() const;
			bool IsSpatializationEnabled() const;

			virtual void Pause() = 0;
			virtual void Play() = 0;

			virtual void SeekToPlayingOffset(Time offset);
			virtual void SeekToSampleOffset(UInt64 offset) = 0;

			void SetAttenuation(float attenuation);
			void SetMinDistance(float minDistance);
			void SetPitch(float pitch);
			void SetPosition(const Vector3f& position);
			void SetVelocity(const Vector3f& velocity);
			void SetVolume(float volume);

			virtual void Stop() = 0;

			SoundEmitter& operator=(const SoundEmitter&) = default;
			SoundEmitter& operator=(SoundEmitter&&) noexcept = default;

		protected:
			std::shared_ptr<AudioSource> m_source;
	};
}

#include <Nazara/Audio/SoundEmitter.inl>

#endif // NAZARA_AUDIO_SOUNDEMITTER_HPP
