// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/AudioDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Log.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <miniaudio.h>
#include <stdexcept>

namespace Nz
{
	AudioDevice::AudioDevice(ma_context* context, ma_device_config deviceConfig)
	{
		deviceConfig.pUserData = this;
		deviceConfig.dataCallback = [](ma_device* device, void* pOutput, const void* pInput, ma_uint32 frameCount)
		{
			AudioDevice& audioDevice = *static_cast<AudioDevice*>(device->pUserData);
			if NAZARA_LIKELY(audioDevice.m_dataCallback)
				audioDevice.m_dataCallback(audioDevice, pInput, pOutput, frameCount);
		};

		/*
		deviceConfig.notificationCallback = [](const ma_device_notification* pNotification)
		{
			char deviceName[MA_MAX_DEVICE_NAME_LENGTH + 1];
			ma_device_get_name(pNotification->pDevice, pNotification->pDevice->type, deviceName, sizeof(deviceName), nullptr);

			switch (pNotification->type)
			{
				case ma_device_notification_type_started:
					NazaraDebug("ma_device_notification_type_started ({})", deviceName);
					break;

				case ma_device_notification_type_stopped:
					NazaraDebug("ma_device_notification_type_stopped ({})", deviceName);
					break;

				case ma_device_notification_type_rerouted:
					NazaraDebug("ma_device_notification_type_rerouted ({})", deviceName);
					break;

				case ma_device_notification_type_interruption_began:
					NazaraDebug("ma_device_notification_type_interruption_began ({})", deviceName);
					break;

				case ma_device_notification_type_interruption_ended:
					NazaraDebug("ma_device_notification_type_interruption_ended ({})", deviceName);
					break;

				case ma_device_notification_type_unlocked:
					NazaraDebug("ma_device_notification_type_unlocked ({})", deviceName);
					break;
			}
		};
		*/

		ma_result result = ma_device_init(context, &deviceConfig, m_device);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_device_init failed: {}", ma_result_description(result)));
	}

	AudioDevice::~AudioDevice()
	{
		ma_device_uninit(m_device);
	}

	ma_device* AudioDevice::GetInternalDevice()
	{
		return m_device;
	}

	const ma_device* AudioDevice::GetInternalDevice() const
	{
		return m_device;
	}

	float AudioDevice::GetMasterVolume() const
	{
		float masterVolume;
		if NAZARA_UNLIKELY(ma_result result = ma_device_get_master_volume(m_device, &masterVolume); result != MA_SUCCESS)
		{
			NazaraError("ma_device_get_master_volume failed: {}", ma_result_description(result));
			return 0.f;
		}

		return masterVolume;
	}

	auto AudioDevice::GetState() const -> State
	{
		ma_device_state state = ma_device_get_state(m_device);
		switch (state)
		{
			case ma_device_state_stopped:       return State::Stopped;
			case ma_device_state_started:       return State::Started;
			case ma_device_state_starting:      return State::Starting;
			case ma_device_state_stopping:      return State::Stopping;
			case ma_device_state_uninitialized: break;
		}

		NazaraError("unexpected device state {:#x}", UnderlyingCast(state));
		return State::Stopped;
	}

	void AudioDevice::SetMasterVolume(float volume)
	{
		if NAZARA_UNLIKELY(ma_result result = ma_device_set_master_volume(m_device, volume); result != MA_SUCCESS)
			NazaraError("ma_device_set_master_volume failed: {}", ma_result_description(result));
	}

	bool AudioDevice::Start()
	{
		if (ma_result result = ma_device_start(m_device); result != MA_SUCCESS)
		{
			NazaraError("ma_device_start failed: {}", ma_result_description(result));
			return false;
		}

		return true;
	}

	void AudioDevice::Stop()
	{
		if NAZARA_UNLIKELY(ma_result result = ma_device_stop(m_device); result != MA_SUCCESS)
			NazaraError("ma_device_stop failed: {}", ma_result_description(result));
	}
}
