// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Audio : public Module<Audio>
	{
		friend Module;

		using Dependencies = TypeList<Core>;

		public:
			Audio();
			~Audio();

			AudioFormat GetAudioFormat(unsigned int channelCount);
			float GetDopplerFactor();
			float GetGlobalVolume();
			Vector3f GetListenerDirection();
			Vector3f GetListenerPosition();
			Quaternionf GetListenerRotation();
			Vector3f GetListenerVelocity();
			float GetSpeedOfSound();

			bool IsFormatSupported(AudioFormat format);
			void SetDopplerFactor(float dopplerFactor);
			void SetGlobalVolume(float volume);
			void SetListenerDirection(const Vector3f& direction);
			void SetListenerDirection(float dirX, float dirY, float dirZ);
			void SetListenerPosition(const Vector3f& position);
			void SetListenerPosition(float x, float y, float z);
			void SetListenerRotation(const Quaternionf& rotation);
			void SetListenerVelocity(const Vector3f& velocity);
			void SetListenerVelocity(float velX, float velY, float velZ);
			void SetSpeedOfSound(float speed);

		private:
			static Audio* s_instance;
	};
}

#endif // NAZARA_AUDIO_HPP
