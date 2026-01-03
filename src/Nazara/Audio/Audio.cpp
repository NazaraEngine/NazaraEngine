// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/AudioDeviceInfo.hpp>
#include <Nazara/Audio/AudioEngine.hpp>
#include <Nazara/Audio/Formats/drmp3Loader.hpp>
#include <Nazara/Audio/Formats/drwavLoader.hpp>
#include <Nazara/Audio/Formats/libflacLoader.hpp>
#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Core/CommandLineParameters.hpp>
#include <Nazara/Core/EnvironmentVariables.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Format.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <miniaudio.h>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::Audio
	* \brief Audio class that represents the module initializer of Audio
	*/

	Audio::Audio(Config config) :
	ModuleBase("Audio", this)
	{
		// Setup miniaudio
		std::unique_ptr<ma_context> maContext = std::make_unique<ma_context>();

		if (ma_result result = ma_log_init(nullptr, &maContext->log); result != MA_SUCCESS)
			throw std::runtime_error(Format("failed to initialize miniaudio logger: {}", ma_result_description(result)));

		CallOnExit uninitLog([&]
		{
			ma_log_uninit(&maContext->log);
		});

		ma_log_register_callback(&maContext->log, ma_log_callback_init([](void* /*pUserData*/, ma_uint32 level, const char* pMessage)
		{
			std::string_view message(pMessage);
			if NAZARA_LIKELY(!message.empty() && message.ends_with('\n'))
				message.remove_suffix(1);

			switch (level)
			{
				case MA_LOG_LEVEL_DEBUG:
					NazaraDebug("[MiniAudio] {}", message);
					break;

				case MA_LOG_LEVEL_INFO:
					NazaraNotice("[MiniAudio] {}", message);
					break;

				case MA_LOG_LEVEL_WARNING:
					NazaraWarning("[MiniAudio] {}", message);
					break;

				case MA_LOG_LEVEL_ERROR:
					NazaraError("[MiniAudio] {}", message);
					break;
			}
		}, nullptr));

		ma_context_config contextConfig = ma_context_config_init();
		contextConfig.pLog = &maContext->log;

		ma_backend nullBackend = ma_backend_null;

		ma_result result = ma_context_init((config.noAudio) ? &nullBackend : nullptr, (config.noAudio) ? 1 : 0, &contextConfig, maContext.get());
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("failed to initialize miniaudio: {}", ma_result_description(result)));

		CallOnExit releaseContext([&] { ma_context_uninit(maContext.get()); });
		uninitLog.Reset(); //< from now, ma_context_uninit will also uninit log

		if (maContext->backend == ma_backend_null && !config.allowNullDevice)
			throw std::runtime_error("no audio backend");

		// Loaders
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_drmp3());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_drmp3());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_drwav());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_drwav());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libflac());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libflac());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libvorbis());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libvorbis());

		releaseContext.Reset();
		m_maContext = maContext.release();
	}

	Audio::~Audio()
	{
		if (ma_result result = ma_context_uninit(m_maContext))
			NazaraWarning("failed to uninit context: {}", ma_result_description(result));

		delete m_maContext;
	}

	/*!
	* \brief Gets the default SoundBuffer loader
	* \return A reference to the default SoundBuffer loader
	*/
	SoundBufferLoader& Audio::GetSoundBufferLoader()
	{
		return m_soundBufferLoader;
	}

	/*!
	* \brief Gets the default SoundBuffer loader
	* \return A constant reference to the default SoundBuffer loader
	*/
	const SoundBufferLoader& Audio::GetSoundBufferLoader() const
	{
		return m_soundBufferLoader;
	}

	/*!
	* \brief Gets the default SoundStream loader
	* \return A reference to the default SoundStream loader
	*/
	SoundStreamLoader& Audio::GetSoundStreamLoader()
	{
		return m_soundStreamLoader;
	}

	/*!
	* \brief Gets the default SoundStream loader
	* \return A constant reference to the default SoundStream loader
	*/
	const SoundStreamLoader& Audio::GetSoundStreamLoader() const
	{
		return m_soundStreamLoader;
	}

	std::shared_ptr<AudioDevice> Audio::OpenCaptureDevice(const AudioDeviceId* captureDevice)
	{
		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
		if (captureDevice)
			deviceConfig.playback.pDeviceID = reinterpret_cast<const ma_device_id*>(&captureDevice->data[0]);

		return std::make_shared<AudioDevice>(m_maContext, deviceConfig);
	}

	std::shared_ptr<AudioDevice> Audio::OpenPlaybackDevice(const AudioDeviceId* playbackDevice)
	{
		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		if (playbackDevice)
			deviceConfig.playback.pDeviceID = reinterpret_cast<const ma_device_id*>(&playbackDevice->data[0]);

		return std::make_shared<AudioDevice>(m_maContext, deviceConfig);
	}

	std::shared_ptr<AudioEngine> Audio::OpenPlaybackEngine(const AudioDeviceId* playbackDevice)
	{
		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		if (playbackDevice)
			deviceConfig.playback.pDeviceID = reinterpret_cast<const ma_device_id*>(&playbackDevice->data[0]);

		// Device config for engines (taken from ma_engine_init)
		deviceConfig.playback.format = ma_format_f32;
		deviceConfig.noPreSilencedOutputBuffer = MA_TRUE;
		deviceConfig.noClip = MA_TRUE;

		auto device = std::make_shared<AudioDevice>(m_maContext, deviceConfig);
		return std::make_shared<AudioEngine>(std::move(device));
	}

	std::vector<AudioDeviceInfo> Audio::QueryDevices() const
	{
		std::vector<AudioDeviceInfo> devices;

		ma_context_enumerate_devices(m_maContext, [](ma_context* /*context*/, ma_device_type deviceType, const ma_device_info* info, void* userdata) -> ma_bool32
		{
			std::vector<AudioDeviceInfo>& deviceList = *reinterpret_cast<std::vector<AudioDeviceInfo>*>(userdata);

			static_assert(sizeof(AudioDeviceId) >= sizeof(ma_device_id));
			static_assert(alignof(AudioDeviceId) >= alignof(ma_device_id));
			static_assert(std::tuple_size_v<decltype(AudioDeviceInfo::deviceName)> >= MA_MAX_DEVICE_NAME_LENGTH + 1);

			auto& device = deviceList.emplace_back();
			std::memcpy(&device.deviceId.data[0], &info->id, sizeof(ma_device_id));
			std::strcpy(&device.deviceName[0], &info->name[0]);

			device.isDefault = info->isDefault == MA_TRUE;
			if (deviceType == ma_device_type_playback)
				device.deviceType = AudioDeviceType::Playback;
			else
			{
				assert(deviceType == ma_device_type_capture);
				device.deviceType = AudioDeviceType::Capture;
			}

			return MA_TRUE;
		}, &devices);

		return devices;
	}

	Audio* Audio::s_instance = nullptr;

	void Audio::Config::Override(const CommandLineParameters& parameters)
	{
		if (parameters.HasFlag("no-audio") || TestEnvironmentVariable("NAZARA_NO_AUDIO"))
			noAudio = true;
	}
}
