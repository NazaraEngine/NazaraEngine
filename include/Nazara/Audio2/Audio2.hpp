// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_HPP
#define NAZARA_AUDIO2_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/AudioDeviceInfo.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Core/Core.hpp>
#include <memory>

struct ma_context;

namespace Nz
{
	class AudioDevice;
	class CommandLineParameters;

	class NAZARA_AUDIO2_API Audio2 : public ModuleBase<Audio2>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config;

			Audio2(Config config);
			Audio2(const Audio2&) = delete;
			Audio2(Audio2&&) = delete;
			~Audio2();

			//SoundBufferLoader& GetSoundBufferLoader();
			//const SoundBufferLoader& GetSoundBufferLoader() const;
			//SoundStreamLoader& GetSoundStreamLoader();
			//const SoundStreamLoader& GetSoundStreamLoader() const;

			std::shared_ptr<AudioDevice> OpenCaptureDevice(const AudioDeviceId* captureDevice = nullptr);
			std::shared_ptr<AudioDevice> OpenPlaybackDevice(const AudioDeviceId* playbackDevice = nullptr);

			std::vector<AudioDeviceInfo> QueryDevices() const;

			Audio2& operator=(const Audio2&) = delete;
			Audio2& operator=(Audio2&&) = delete;

			struct NAZARA_AUDIO2_API Config
			{
				void Override(const CommandLineParameters& parameters);

				bool allowNullDevice = true;
				bool noAudio = false;
			};

		private:
			//SoundBufferLoader m_soundBufferLoader;
			//SoundStreamLoader m_soundStreamLoader;
			ma_context* m_maContext;

			static Audio2* s_instance;
	};
}

#endif // NAZARA_AUDIO2_HPP
