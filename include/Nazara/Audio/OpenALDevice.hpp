// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALDEVICE_HPP
#define NAZARA_AUDIO_OPENALDEVICE_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utils/MovablePtr.hpp>
#include <array>
#include <string>

namespace Nz
{
	class OpenALLibrary;

	enum class OpenALExtension
	{
		SourceLatency,

		Max = SourceLatency
	};

	constexpr std::size_t OpenALExtensionCount = static_cast<std::size_t>(OpenALExtension::Max) + 1;

	class NAZARA_AUDIO_API OpenALDevice : public AudioDevice
	{
		friend OpenALLibrary;

		public:
			OpenALDevice(OpenALLibrary& library, ALCdevice* device);
			OpenALDevice(const OpenALDevice&) = delete;
			OpenALDevice(OpenALDevice&&) = delete;
			~OpenALDevice();

			std::shared_ptr<AudioBuffer> CreateBuffer() override;
			std::shared_ptr<AudioSource> CreateSource() override;

			float GetDopplerFactor() const override;
			float GetGlobalVolume() const override;
			Vector3f GetListenerDirection(Vector3f* up = nullptr) const override;
			Vector3f GetListenerPosition() const override;
			Quaternionf GetListenerRotation() const override;
			Vector3f GetListenerVelocity() const override;
			float GetSpeedOfSound() const override;
			const void* GetSubSystemIdentifier() const override;

			inline bool IsExtensionSupported(OpenALExtension extension) const;
			bool IsFormatSupported(AudioFormat format) const override;

			void MakeContextCurrent() const;

			void SetDopplerFactor(float dopplerFactor) override;
			void SetGlobalVolume(float volume) override;
			void SetListenerDirection(const Vector3f& direction, const Vector3f& up = Vector3f::Up()) override;
			void SetListenerPosition(const Vector3f& position) override;
			void SetListenerVelocity(const Vector3f& velocity) override;
			void SetSpeedOfSound(float speed) override;

			inline ALenum TranslateAudioFormat(AudioFormat format) const;

			OpenALDevice& operator=(const OpenALDevice&) = delete;
			OpenALDevice& operator=(OpenALDevice&&) = delete;

		private:
			std::array<ALenum, AudioFormatCount> m_audioFormatValues;
			std::array<bool, OpenALExtensionCount> m_extensionStatus;
			std::string m_renderer;
			std::string m_vendor;
			OpenALLibrary& m_library;
			MovablePtr<ALCcontext> m_context;
			MovablePtr<ALCdevice> m_device;
	};
}

#include <Nazara/Audio/OpenALDevice.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALDEVICE_HPP
