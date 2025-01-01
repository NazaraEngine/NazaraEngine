// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_DUMMYAUDIODEVICE_HPP
#define NAZARA_AUDIO_DUMMYAUDIODEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <array>
#include <string>

namespace Nz
{
	class NAZARA_AUDIO_API DummyAudioDevice : public AudioDevice
	{
		public:
			DummyAudioDevice();
			DummyAudioDevice(const DummyAudioDevice&) = delete;
			DummyAudioDevice(DummyAudioDevice&&) = delete;
			~DummyAudioDevice() = default;

			std::shared_ptr<AudioBuffer> CreateBuffer() override;
			std::shared_ptr<AudioSource> CreateSource() override;

			void DetachThread() const override;

			float GetDopplerFactor() const override;
			float GetGlobalVolume() const override;
			Vector3f GetListenerDirection(Vector3f* up = nullptr) const override;
			Vector3f GetListenerPosition() const override;
			Quaternionf GetListenerRotation() const override;
			Vector3f GetListenerVelocity() const override;
			float GetSpeedOfSound() const override;
			const void* GetSubSystemIdentifier() const override;

			bool IsFormatSupported(AudioFormat format) const override;

			void SetDopplerFactor(float dopplerFactor) override;
			void SetGlobalVolume(float volume) override;
			void SetListenerDirection(const Vector3f& direction, const Vector3f& up = Vector3f::Up()) override;
			void SetListenerPosition(const Vector3f& position) override;
			void SetListenerVelocity(const Vector3f& velocity) override;
			void SetSpeedOfSound(float speed) override;

			DummyAudioDevice& operator=(const DummyAudioDevice&) = delete;
			DummyAudioDevice& operator=(DummyAudioDevice&&) = delete;

		private:
			Quaternionf m_listenerRotation;
			Vector3f m_listenerVelocity;
			Vector3f m_listenerPosition;
			float m_dopplerFactor;
			float m_globalVolume;
			float m_speedOfSound;
	};
}

#include <Nazara/Audio/DummyAudioDevice.inl>

#endif // NAZARA_AUDIO_DUMMYAUDIODEVICE_HPP
