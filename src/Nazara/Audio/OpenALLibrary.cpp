// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <array>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	bool OpenALLibrary::Load()
	{
		Unload();

		CallOnExit unloadOnFailure([this] { Unload(); });

#if defined(NAZARA_PLATFORM_WINDOWS)
		std::array libs{
			"soft_oal.dll",
			"wrap_oal.dll",
			"openal32.dll"
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
			if (!m_library.Load(libname))
				continue;

			auto LoadSymbol = [this](const char* name)
			{
				DynLibFunc funcPtr = m_library.GetSymbol(name);
				if (!funcPtr)
					throw std::runtime_error(std::string("failed to load ") + name);

				return funcPtr;
			};

			try
			{
#define NAZARA_AUDIO_FUNC(name, sig) name = reinterpret_cast<sig>(LoadSymbol(#name));
				NAZARA_AUDIO_FOREACH_AL_FUNC(NAZARA_AUDIO_FUNC)
				NAZARA_AUDIO_FOREACH_ALC_FUNC(NAZARA_AUDIO_FUNC)
#undef NAZARA_AUDIO_FUNC
			}
			catch (const std::exception& e)
			{
				NazaraWarning(std::string("failed to load ") + libname + ": " + e.what());
				continue;
			}

			unloadOnFailure.Reset();
			return true;
		}

		m_hasCaptureSupport = alcIsExtensionPresent(nullptr, "ALC_EXT_CAPTURE");

		NazaraError("failed to load OpenAL library");
		return false;
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

#define NAZARA_AUDIO_FUNC(name, sig) name = nullptr;
		NAZARA_AUDIO_FOREACH_AL_FUNC(NAZARA_AUDIO_FUNC)
		NAZARA_AUDIO_FOREACH_ALC_FUNC(NAZARA_AUDIO_FUNC)
#undef NAZARA_AUDIO_FUNC

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
