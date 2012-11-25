// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

class NAZARA_API NzAudio
{
	friend class NzMusic;
	friend class NzSoundBuffer;

	public:
		NzAudio() = delete;
		~NzAudio() = delete;

		static nzAudioFormat GetAudioFormat(unsigned int channelCount);
		static float GetDopplerFactor();
		static float GetGlobalVolume();
		static NzVector3f GetListenerDirection();
		static NzVector3f GetListenerPosition();
		//static NzQuaternionf GetListenerRotation();
		static NzVector3f GetListenerVelocity();
		static float GetSpeedOfSound();

		static bool Initialize();

		static bool IsFormatSupported(nzAudioFormat format);
		static bool IsInitialized();

		static void SetDopplerFactor(float dopplerFactor);
		static void SetGlobalVolume(float volume);
		static void SetListenerDirection(const NzVector3f& direction);
		static void SetListenerDirection(float dirX, float dirY, float dirZ);
		static void SetListenerPosition(const NzVector3f& position);
		static void SetListenerPosition(float x, float y, float z);
		//static void SetListenerRotation(const NzQuaternionf& rotation);
		static void SetListenerVelocity(const NzVector3f& velocity);
		static void SetListenerVelocity(float velX, float velY, float velZ);
		static void SetSpeedOfSound(float speed);

		static void Uninitialize();

	private:
		static unsigned int GetOpenALFormat(nzAudioFormat format);

		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_AUDIO_HPP
