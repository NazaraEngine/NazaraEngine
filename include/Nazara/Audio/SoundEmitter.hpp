// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDEMITTER_HPP
#define NAZARA_SOUNDEMITTER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Math/Vector3.hpp>

///TODO: Inherit SoundEmitter from Node

namespace Nz
{
	class NAZARA_AUDIO_API SoundEmitter
	{
		public:
			virtual ~SoundEmitter();

			virtual void EnableLooping(bool loop) = 0;
			void EnableSpatialization(bool spatialization);

			float GetAttenuation() const;
			virtual UInt32 GetDuration() const = 0;
			float GetMinDistance() const;
			float GetPitch() const;
			virtual UInt32 GetPlayingOffset() const = 0;
			Vector3f GetPosition() const;
			Vector3f GetVelocity() const;
			virtual SoundStatus GetStatus() const = 0;
			float GetVolume() const;

			virtual bool IsLooping() const = 0;
			bool IsSpatialized() const;

			virtual void Pause() = 0;
			virtual void Play() = 0;

			void SetAttenuation(float attenuation);
			void SetMinDistance(float minDistance);
			void SetPitch(float pitch);
			void SetPosition(const Vector3f& position);
			void SetPosition(float x, float y, float z);
			void SetVelocity(const Vector3f& velocity);
			void SetVelocity(float velX, float velY, float velZ);
			void SetVolume(float volume);

			virtual void Stop() = 0;

			SoundEmitter& operator=(const SoundEmitter&) = delete; ///TODO
			SoundEmitter& operator=(SoundEmitter&&) = delete; ///TODO

		protected:
			SoundEmitter();
			SoundEmitter(const SoundEmitter& emitter);
			SoundEmitter(SoundEmitter&&) = delete; ///TODO

			SoundStatus GetInternalStatus() const;

			unsigned int m_source;
	};
}
#endif // NAZARA_SOUNDEMITTER_HPP
