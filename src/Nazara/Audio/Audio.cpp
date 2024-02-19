// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/DummyAudioDevice.hpp>
#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Audio/Formats/drwavLoader.hpp>
#include <Nazara/Audio/Formats/libflacLoader.hpp>
#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Audio/Formats/minimp3Loader.hpp>
#include <Nazara/Core/CommandLineParameters.hpp>
#include <Nazara/Core/EnvironmentVariables.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>

namespace Nz
{
	namespace
	{
		OpenALLibrary s_openalLibrary;
	}

	/*!
	* \ingroup audio
	* \class Nz::Audio
	* \brief Audio class that represents the module initializer of Audio
	*/

	Audio::Audio(Config config) :
	ModuleBase("Audio", this),
	m_hasDummyDevice(config.allowDummyDevice)
	{
		// Load OpenAL
		if (!config.noAudio)
		{
			if (!s_openalLibrary.Load())
			{
				if (!config.allowDummyDevice)
					throw std::runtime_error("failed to load OpenAL");

				NazaraError("failed to load OpenAL");
			}
		}

		// Loaders
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_drwav());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_drwav());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libflac());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libflac());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libvorbis());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libvorbis());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_minimp3());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_minimp3());

		if (s_openalLibrary.IsLoaded())
		{
			try
			{
				m_defaultDevice = s_openalLibrary.OpenDevice();
			}
			catch (const std::exception& e)
			{
				if (!config.allowDummyDevice)
					throw;

				NazaraErrorFmt("failed to open default OpenAL device: {0}", e.what());
			}
		}

		if (!m_defaultDevice)
			m_defaultDevice = std::make_shared<DummyAudioDevice>();
	}

	Audio::~Audio()
	{
		m_defaultDevice.reset();
		s_openalLibrary.Unload();
	}

	const std::shared_ptr<AudioDevice>& Audio::GetDefaultDevice() const
	{
		return m_defaultDevice;
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

	std::shared_ptr<AudioDevice> Audio::OpenOutputDevice(const std::string& deviceName)
	{
		if (deviceName == "dummy")
			return std::make_shared<DummyAudioDevice>();

		return s_openalLibrary.OpenDevice(deviceName.c_str());
	}

	std::vector<std::string> Audio::QueryInputDevices() const
	{
		if (!s_openalLibrary.IsLoaded())
			return {};

		return s_openalLibrary.QueryInputDevices();
	}

	std::vector<std::string> Audio::QueryOutputDevices() const
	{
		std::vector<std::string> outputDevices;
		if (s_openalLibrary.IsLoaded())
			outputDevices = s_openalLibrary.QueryOutputDevices();

		if (m_hasDummyDevice)
			outputDevices.push_back("dummy");

		return outputDevices;
	}

	Audio* Audio::s_instance = nullptr;

	void Audio::Config::Override(const CommandLineParameters& parameters)
	{
		if (parameters.HasFlag("no-audio") || TestEnvironmentVariable("NAZARA_NO_AUDIO"))
			noAudio = true;
	}
}
