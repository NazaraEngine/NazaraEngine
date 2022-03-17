// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Audio/Formats/drwavLoader.hpp>
#include <Nazara/Audio/Formats/libflacLoader.hpp>
#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Audio/Formats/minimp3Loader.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

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

	Audio::Audio(Config /*config*/) :
	ModuleBase("Audio", this)
	{
		// Load OpenAL
		if (!s_openalLibrary.Load())
			throw std::runtime_error("failed to load OpenAL");

		// Loaders
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_drwav());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_drwav());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libflac());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libflac());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_libvorbis());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_libvorbis());
		m_soundBufferLoader.RegisterLoader(Loaders::GetSoundBufferLoader_minimp3());
		m_soundStreamLoader.RegisterLoader(Loaders::GetSoundStreamLoader_minimp3());

		m_defaultDevice = s_openalLibrary.OpenDevice();
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
		return s_openalLibrary.OpenDevice(deviceName.c_str());
	}

	std::vector<std::string> Audio::QueryInputDevices() const
	{
		return s_openalLibrary.QueryInputDevices();
	}

	std::vector<std::string> Audio::QueryOutputDevices() const
	{
		return s_openalLibrary.QueryOutputDevices();
	}

	Audio* Audio::s_instance = nullptr;
}
