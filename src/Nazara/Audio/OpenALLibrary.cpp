// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <array>
#include <sstream>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	bool OpenALLibrary::Load()
	{
		Unload();

		CallOnExit unloadOnFailure([this] { Unload(); });

		auto PostLoad = [&]
		{
			m_hasCaptureSupport = alcIsExtensionPresent(nullptr, "ALC_EXT_CAPTURE");
			m_isLoaded = true;

			unloadOnFailure.Reset();
		};

#ifndef NAZARA_AUDIO_OPENAL_LINK
		// Dynamically load OpenAL
#if defined(NAZARA_PLATFORM_WINDOWS)
		std::array libs{
			"soft_oal.dll",
			"openal32.dll",
			"wrap_oal.dll"
		};
#elif defined(NAZARA_PLATFORM_LINUX)
		std::array libs {
			"libopenal.so.1",
			"libopenal.so.0",
			"libopenal.so"
		};
#elif defined(NAZARA_PLATFORM_MACOS)
		std::array libs {
			"libopenal.dylib",
			"libopenal.1.dylib",
		};
#else
		NazaraError("unhandled OS");
		return false;
#endif

		for (const char* libname : libs)
		{
			ErrorFlags disableError(ErrorMode::Silent, ~ErrorMode::ThrowException);

			if (!m_library.Load(Utf8Path(libname)))
				continue;

			auto LoadSymbol = [this](const char* name, bool optional)
			{
				DynLibFunc funcPtr = m_library.GetSymbol(name);
				if (!funcPtr && !optional)
					throw std::runtime_error(std::string("failed to load ") + name);

				return funcPtr;
			};

			try
			{
#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) name = reinterpret_cast<decltype(&::name)>(LoadSymbol(#name, false));
#define NAZARA_AUDIO_AL_EXT_FUNCTION(name)
#include <Nazara/Audio/OpenALFunctions.hpp>
			}
			catch (const std::exception& e)
			{
				ErrorFlags disableSilent({}, ~ErrorMode::Silent);

				NazaraWarningFmt("failed to load {0}: {1}", libname, e.what());
				continue;
			}

			PostLoad();
			return true;
		}

		NazaraError("failed to load OpenAL library");
		return false;

#else
		// OpenAL is linked to the executable

		// Load core
#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) name = &::name;
#define NAZARA_AUDIO_AL_EXT_FUNCTION(name)
#include <Nazara/Audio/OpenALFunctions.hpp>

		PostLoad();
		return true;
#endif
		}

	std::vector<std::string> OpenALLibrary::QueryInputDevices()
	{
		if (!m_hasCaptureSupport)
			return {};

		return ParseDevices(alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
	}

	std::vector<std::string> OpenALLibrary::QueryOutputDevices()
	{
		return ParseDevices(alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));
	}

	std::shared_ptr<OpenALDevice> OpenALLibrary::OpenDevice(const char* name)
	{
		ALCdevice* device = alcOpenDevice(name);
		if (!device)
			throw std::runtime_error("failed to open device");

		return std::make_shared<OpenALDevice>(*this, device);
	}

	void OpenALLibrary::Unload()
	{
		if (!m_library.IsLoaded())
			return;

#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) name = nullptr;
#define NAZARA_AUDIO_AL_EXT_FUNCTION(name)
#include <Nazara/Audio/OpenALFunctions.hpp>

		m_library.Unload();
	}

	std::vector<std::string> OpenALLibrary::ParseDevices(const char* deviceString)
	{
		if (!deviceString)
			return {};

		std::vector<std::string> devices;
		std::size_t length;
		while ((length = std::strlen(deviceString)) > 0)
		{
			devices.emplace_back(deviceString, length);
			deviceString += length + 1;
		}

		return devices;
	}
}
