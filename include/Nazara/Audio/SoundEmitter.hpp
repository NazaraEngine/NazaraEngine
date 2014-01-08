// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDEMITTER_HPP
#define NAZARA_SOUNDEMITTER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Math/Vector3.hpp>

///TODO: Faire hériter SoundEmitter de Node

class NAZARA_API NzSoundEmitter
{
	public:
		virtual ~NzSoundEmitter();

		virtual void EnableLooping(bool loop) = 0;
		void EnableSpatialization(bool spatialization);

		float GetAttenuation() const;
		virtual nzUInt32 GetDuration() const = 0;
		float GetMinDistance() const;
		float GetPitch() const;
		virtual nzUInt32 GetPlayingOffset() const = 0;
		NzVector3f GetPosition() const;
		NzVector3f GetVelocity() const;
		virtual nzSoundStatus GetStatus() const = 0;
		float GetVolume() const;

		virtual bool IsLooping() const = 0;
		bool IsSpatialized() const;

		virtual void Pause() = 0;
		virtual void Play() = 0;

		void SetAttenuation(float attenuation);
		void SetMinDistance(float minDistance);
		void SetPitch(float pitch);
		void SetPosition(const NzVector3f& position);
		void SetPosition(float x, float y, float z);
		void SetVelocity(const NzVector3f& velocity);
		void SetVelocity(float velX, float velY, float velZ);
		void SetVolume(float volume);

		virtual void Stop() = 0;

	protected:
		NzSoundEmitter();
		NzSoundEmitter(const NzSoundEmitter& emitter);

		nzSoundStatus GetInternalStatus() const;

		unsigned int m_source;
};

#endif // NAZARA_SOUNDEMITTER_HPP
