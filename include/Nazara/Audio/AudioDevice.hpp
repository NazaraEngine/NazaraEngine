// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIODEVICE_HPP
#define NAZARA_AUDIO_AUDIODEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Export.hpp>
#include <NazaraUtils/PrivateImpl.hpp>
#include <functional>
#include <mutex>

struct ma_context;
struct ma_device;
struct ma_device_config;

namespace Nz
{
	class NAZARA_AUDIO_API AudioDevice
	{
		public:
			using DataCallback = std::function<void(const AudioDevice& device, const void* inputData, void* outputData, UInt32 frameCount)>;
			enum class State;

			AudioDevice(ma_context* context, ma_device_config deviceConfig);
			AudioDevice(const AudioDevice&) = delete;
			AudioDevice(AudioDevice&&) = delete;
			~AudioDevice();

			ma_device* GetInternalDevice();
			const ma_device* GetInternalDevice() const;

			float GetMasterVolume() const;
			State GetState() const;

			void SetDataCallback(DataCallback callback);
			void SetMasterVolume(float volume);

			bool Start();
			void Stop();

			AudioDevice& operator=(const AudioDevice&) = delete;
			AudioDevice& operator=(AudioDevice&&) = delete;

			enum class State
			{
				Starting,
				Started,
				Stopping,
				Stopped
			};

		private:
			std::mutex m_dataCallbackMutex;
			mutable PrivateImpl<ma_device> m_device;
			DataCallback m_dataCallback;
	};
}

#include <Nazara/Audio/AudioDevice.inl>

#endif // NAZARA_AUDIO_AUDIODEVICE_HPP
