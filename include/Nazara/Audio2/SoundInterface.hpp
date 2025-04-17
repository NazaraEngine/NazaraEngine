// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_SOUNDINTERFACE_HPP
#define NAZARA_AUDIO2_SOUNDINTERFACE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class AudioEngine;

	class NAZARA_AUDIO2_API SoundInterface
	{
		public:
			SoundInterface() = default;
			SoundInterface(const SoundInterface&) = delete;
			SoundInterface(SoundInterface&&) = delete;
			virtual ~SoundInterface();

			virtual SoundAttenuationModel GetAttenuationModel() const = 0;
			virtual void GetCone(RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const = 0;
			virtual Vector3f GetDirection() const = 0;
			virtual float GetDirectionalAttenuationFactor() const = 0;
			virtual float GetDopplerFactor() const = 0;
			virtual float GetMaxDistance() const = 0;
			virtual float GetMaxGain() const = 0;
			virtual float GetMinDistance() const = 0;
			virtual float GetMinGain() const = 0;
			virtual float GetPan() const = 0;
			virtual SoundPanMode GetPanMode() const = 0;
			virtual float GetPitch() const = 0;
			virtual SoundPositioning GetPositioning() const = 0;
			virtual float GetRolloff() const = 0;
			virtual AudioEngine& GetEngine() = 0;
			virtual const AudioEngine& GetEngine() const = 0;
			virtual UInt32 GetListenerIndex() const = 0;
			virtual Vector3f GetPosition() const = 0;
			virtual Vector3f GetVelocity() const = 0;
			virtual float GetVolume() const = 0;

			virtual bool IsPlaying() const = 0;

			virtual void SetAttenuationModel(SoundAttenuationModel attenuationModel) = 0;
			virtual void SetCone(RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain) = 0;
			virtual void SetDirection(const Vector3f& direction) = 0;
			virtual void SetDirectionalAttenuationFactor(float directionalAttenuationFactor) = 0;
			virtual void SetDopplerFactor(float dopplerFactor) = 0;
			virtual void SetMaxDistance(float maxDistance) = 0;
			virtual void SetMaxGain(float maxGain) = 0;
			virtual void SetMinDistance(float minDistance) = 0;
			virtual void SetMinGain(float minGain) = 0;
			virtual void SetPan(float pan) = 0;
			virtual void SetPanMode(SoundPanMode panMode) = 0;
			virtual void SetPitch(float pitch) = 0;
			virtual void SetPositioning(SoundPositioning positioning) = 0;
			virtual void SetRolloff(float rollOff) = 0;
			virtual void SetPosition(const Vector3f& position) = 0;
			virtual void SetVelocity(const Vector3f& velocity) = 0;
			virtual void SetVolume(float volume) = 0;

			SoundInterface& operator=(const SoundInterface&) = delete;
			SoundInterface& operator=(SoundInterface&&) = delete;
	};
}

#include <Nazara/Audio2/SoundInterface.inl>

#endif // NAZARA_AUDIO2_SOUNDINTERFACE_HPP
