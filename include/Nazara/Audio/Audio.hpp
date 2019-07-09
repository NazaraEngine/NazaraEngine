// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Audio
	{
		public:
			Audio() = delete;
			~Audio() = delete;

			static AudioFormat GetAudioFormat(unsigned int channelCount);
			static float GetDopplerFactor();
			static float GetGlobalVolume();
			static Vector3f GetListenerDirection();
			static Vector3f GetListenerPosition();
			static Quaternionf GetListenerRotation();
			static Vector3f GetListenerVelocity();
			static float GetSpeedOfSound();

			static bool Initialize();

			static bool IsFormatSupported(AudioFormat format);
			static bool IsInitialized();

			static void SetDopplerFactor(float dopplerFactor);
			static void SetGlobalVolume(float volume);
			static void SetListenerDirection(const Vector3f& direction);
			static void SetListenerDirection(float dirX, float dirY, float dirZ);
			static void SetListenerPosition(const Vector3f& position);
			static void SetListenerPosition(float x, float y, float z);
			static void SetListenerRotation(const Quaternionf& rotation);
			static void SetListenerVelocity(const Vector3f& velocity);
			static void SetListenerVelocity(float velX, float velY, float velZ);
			static void SetSpeedOfSound(float speed);

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_AUDIO_HPP
