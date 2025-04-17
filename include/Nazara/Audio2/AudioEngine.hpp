// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_AUDIOENGINE_HPP
#define NAZARA_AUDIO2_AUDIOENGINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <NazaraUtils/PrivateImpl.hpp>
#include <memory>

struct ma_engine;
struct ma_sound;
using ma_sound_group = ma_sound;

namespace Nz
{
	class AudioDevice;
	class Sound;
	class SoundGroup;

	class NAZARA_AUDIO2_API AudioEngine
	{
		public:
			AudioEngine(std::shared_ptr<AudioDevice> audioDevice);
			AudioEngine(const AudioEngine&) = delete;
			AudioEngine(AudioEngine&&) = delete;
			~AudioEngine();

			ma_sound* AllocateInternalSound(std::size_t& soundIndex);
			ma_sound_group* AllocateInternalSoundGroup(std::size_t& soundIndex);

			void FreeInternalSound(std::size_t soundIndex);
			void FreeInternalSoundGroup(std::size_t soundGroupIndex);

			ma_engine* GetInternalHandle() const;

			void GetListenerCone(UInt32 listenerIndex, RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const;
			Vector3f GetListenerDirection(UInt32 listenerIndex) const;
			Vector3f GetListenerPosition(UInt32 listenerIndex) const;
			Quaternionf GetListenerRotation(UInt32 listenerIndex) const;
			Vector3f GetListenerVelocity(UInt32 listenerIndex) const;
			Vector3f GetListenerWorldUp(UInt32 listenerIndex) const;
			UInt32 GetListenerCount() const;
			float GetVolume() const;

			bool IsListenerActive(UInt32 listenerIndex) const;

			void SetListenerActive(UInt32 listenerIndex, bool active);
			void SetListenerCone(UInt32 listenerIndex, RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain);
			void SetListenerDirection(UInt32 listenerIndex, const Vector3f& direction);
			void SetListenerPosition(UInt32 listenerIndex, const Vector3f& position);
			void SetListenerRotation(UInt32 listenerIndex, const Quaternionf& rotation);
			void SetListenerVelocity(UInt32 listenerIndex, const Vector3f& velocity);
			void SetListenerWorldUp(UInt32 listenerIndex, const Vector3f& worldUp);
			void SetVolume(float volume);

			AudioEngine& operator=(const AudioEngine&) = delete;
			AudioEngine& operator=(AudioEngine&&) = delete;

		private:
			struct Impl;
			std::unique_ptr<Impl> m_impl;
	};
}

#include <Nazara/Audio2/AudioEngine.inl>

#endif // NAZARA_AUDIO2_AUDIOENGINE_HPP
