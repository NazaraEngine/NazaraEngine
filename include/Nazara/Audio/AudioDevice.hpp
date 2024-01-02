// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIODEVICE_HPP
#define NAZARA_AUDIO_AUDIODEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>

namespace Nz
{
	class AudioBuffer;
	class AudioSource;

	class NAZARA_AUDIO_API AudioDevice : public std::enable_shared_from_this<AudioDevice>
	{
		public:
			AudioDevice() = default;
			AudioDevice(const AudioDevice&) = delete;
			AudioDevice(AudioDevice&&) = delete;
			virtual ~AudioDevice();

			virtual std::shared_ptr<AudioBuffer> CreateBuffer() = 0;
			virtual std::shared_ptr<AudioSource> CreateSource() = 0;

			virtual float GetDopplerFactor() const = 0;
			virtual float GetGlobalVolume() const = 0;
			virtual Vector3f GetListenerDirection(Vector3f* up = nullptr) const = 0;
			virtual Vector3f GetListenerPosition() const = 0;
			virtual Quaternionf GetListenerRotation() const = 0;
			virtual Vector3f GetListenerVelocity() const = 0;
			virtual float GetSpeedOfSound() const = 0;
			virtual const void* GetSubSystemIdentifier() const = 0;

			virtual bool IsFormatSupported(AudioFormat format) const = 0;

			virtual void SetDopplerFactor(float dopplerFactor) = 0;
			virtual void SetGlobalVolume(float volume) = 0;
			virtual void SetListenerDirection(const Vector3f& direction, const Vector3f& up = Vector3f::Up()) = 0;
			virtual void SetListenerPosition(const Vector3f& position) = 0;
			inline void SetListenerRotation(const Quaternionf& rotation);
			virtual void SetListenerVelocity(const Vector3f& velocity) = 0;
			virtual void SetSpeedOfSound(float speed) = 0;

			AudioDevice& operator=(const AudioDevice&) = delete;
			AudioDevice& operator=(AudioDevice&&) = delete;

			NazaraSignal(OnAudioDeviceRelease, AudioDevice* /*audioDevice*/);
	};
}

#include <Nazara/Audio/AudioDevice.inl>

#endif // NAZARA_AUDIO_AUDIODEVICE_HPP
