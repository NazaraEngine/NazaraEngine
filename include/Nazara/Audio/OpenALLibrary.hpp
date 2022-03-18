// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALLIBRARY_HPP
#define NAZARA_AUDIO_OPENALLIBRARY_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_AUDIO_API OpenALLibrary
	{
		public:
			OpenALLibrary() = default;
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

#define NAZARA_AUDIO_FUNC(name, sig) sig name;
			NAZARA_AUDIO_FOREACH_AL_FUNC(NAZARA_AUDIO_FUNC)
			NAZARA_AUDIO_FOREACH_ALC_FUNC(NAZARA_AUDIO_FUNC)
#undef NAZARA_AUDIO_FUNC

		private:
			std::vector<std::string> ParseDevices(const char* deviceString);

			DynLib m_library;
			bool m_hasCaptureSupport;
	};
}

#include <Nazara/Audio/OpenALLibrary.inl>

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALLIBRARY_HPP
