// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALLIBRARY_HPP
#define NAZARA_AUDIO_OPENALLIBRARY_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class OpenALDevice;

	class NAZARA_AUDIO_API OpenALLibrary
	{
		public:
			inline OpenALLibrary();
			OpenALLibrary(const OpenALLibrary&) = delete;
			OpenALLibrary(OpenALLibrary&&) = delete;
			inline ~OpenALLibrary();

			inline bool IsLoaded() const;

			bool Load();

			std::vector<std::string> QueryInputDevices();
			std::vector<std::string> QueryOutputDevices();

			std::shared_ptr<OpenALDevice> OpenDevice(const char* name = nullptr);

			void Unload();

			OpenALLibrary& operator=(const OpenALLibrary&) = delete;
			OpenALLibrary& operator=(OpenALLibrary&&) = delete;

#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) decltype(&::name) name;
#define NAZARA_AUDIO_AL_EXT_FUNCTION(name)
#include <Nazara/Audio/OpenALFunctions.hpp>

		private:
			std::vector<std::string> ParseDevices(const char* deviceString);

			DynLib m_library;
			bool m_hasCaptureSupport;
			bool m_isLoaded;
	};
}

#include <Nazara/Audio/OpenALLibrary.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALLIBRARY_HPP
