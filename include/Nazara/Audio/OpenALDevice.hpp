// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALDEVICE_HPP
#define NAZARA_AUDIO_OPENALDEVICE_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <array>
#include <string>

namespace Nz
{
	class OpenALLibrary;

	enum class OpenALExtension
	{
		SourceLatency,
		ThreadLocalContext,

		Max = SourceLatency
	};

	using ALFunction = void(*)(void);

	class NAZARA_AUDIO_API OpenALDevice : public AudioDevice
	{
		friend OpenALLibrary;
		struct SymbolLoader;
		friend SymbolLoader;

		public:
			OpenALDevice(OpenALLibrary& library, ALCdevice* device);
			OpenALDevice(const OpenALDevice&) = delete;
			OpenALDevice(OpenALDevice&&) = delete;
			~OpenALDevice();

			bool ClearErrorFlag() const;

			std::shared_ptr<AudioBuffer> CreateBuffer() override;
			std::shared_ptr<AudioSource> CreateSource() override;

			float GetDopplerFactor() const override;
			inline ALFunction GetFunctionByIndex(std::size_t funcIndex) const;
			float GetGlobalVolume() const override;
			inline OpenALLibrary& GetLibrary();
			inline const OpenALLibrary& GetLibrary() const;
			Vector3f GetListenerDirection(Vector3f* up = nullptr) const override;
			Vector3f GetListenerPosition() const override;
			Quaternionf GetListenerRotation() const override;
			Vector3f GetListenerVelocity() const override;
			float GetSpeedOfSound() const override;
			const void* GetSubSystemIdentifier() const override;

			inline bool IsExtensionSupported(OpenALExtension extension) const;
			bool IsFormatSupported(AudioFormat format) const override;

			void MakeContextCurrent() const;

			template<typename... Args> void PrintFunctionCall(std::size_t funcIndex, Args... args) const;
			bool ProcessErrorFlag() const;

			void SetDopplerFactor(float dopplerFactor) override;
			void SetGlobalVolume(float volume) override;
			void SetListenerDirection(const Vector3f& direction, const Vector3f& up = Vector3f::Up()) override;
			void SetListenerPosition(const Vector3f& position) override;
			void SetListenerVelocity(const Vector3f& velocity) override;
			void SetSpeedOfSound(float speed) override;

			inline ALenum TranslateAudioFormat(AudioFormat format) const;

			OpenALDevice& operator=(const OpenALDevice&) = delete;
			OpenALDevice& operator=(OpenALDevice&&) = delete;

			static const OpenALDevice* GetCurrentDevice();

			// We give each device its own set of function pointer, even though regular OpenAL extensions are always the same (for a set library).
			// This makes it easier to wrap them (for error handling), and extension pointers are device-local anyway.
#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) decltype(&::name) name;
#include <Nazara/Audio/OpenALFunctions.hpp>

		private:
			enum class FunctionIndex
			{
#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) name,
#include <Nazara/Audio/OpenALFunctions.hpp>

				Count
			};

			std::array<ALFunction, UnderlyingCast(FunctionIndex::Count)> m_originalFunctionPointer;
			std::string m_renderer;
			std::string m_vendor;
			EnumArray<AudioFormat, ALenum> m_audioFormatValues;
			EnumArray<OpenALExtension, ALenum> m_extensionStatus;
			OpenALLibrary& m_library;
			MovablePtr<ALCcontext> m_context;
			MovablePtr<ALCdevice> m_device;
	};
}

#include <Nazara/Audio/OpenALDevice.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALDEVICE_HPP
