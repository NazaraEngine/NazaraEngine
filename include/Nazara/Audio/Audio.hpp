// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API Audio : public ModuleBase<Audio>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Audio(Config /*config*/);
			Audio(const Audio&) = delete;
			Audio(Audio&&) = delete;
			~Audio();

			AudioFormat GetAudioFormat(unsigned int channelCount) const;
			float GetDopplerFactor() const;
			float GetGlobalVolume() const;
			Vector3f GetListenerDirection() const;
			Vector3f GetListenerPosition() const;
			Quaternionf GetListenerRotation() const;
			Vector3f GetListenerVelocity() const;
			SoundBufferLoader& GetSoundBufferLoader();
			const SoundBufferLoader& GetSoundBufferLoader() const;
			SoundStreamLoader& GetSoundStreamLoader();
			const SoundStreamLoader& GetSoundStreamLoader() const;
			float GetSpeedOfSound() const;

			bool IsFormatSupported(AudioFormat format) const;

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

			Audio& operator=(const Audio&) = delete;
			Audio& operator=(Audio&&) = delete;

		private:
			SoundBufferLoader m_soundBufferLoader;
			SoundStreamLoader m_soundStreamLoader;

			static Audio* s_instance;
	};
}

#endif // NAZARA_AUDIO_HPP
