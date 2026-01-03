// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioDeviceInfo.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Core.hpp>
#include <memory>

struct ma_context;

namespace Nz
{
	class AudioDevice;
	class AudioEngine;
	class CommandLineParameters;

	class NAZARA_AUDIO_API Audio : public ModuleBase<Audio>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config;

			Audio(Config config);
			Audio(const Audio&) = delete;
			Audio(Audio&&) = delete;
			~Audio();

			SoundBufferLoader& GetSoundBufferLoader();
			const SoundBufferLoader& GetSoundBufferLoader() const;
			SoundStreamLoader& GetSoundStreamLoader();
			const SoundStreamLoader& GetSoundStreamLoader() const;

			std::shared_ptr<AudioDevice> OpenCaptureDevice(const AudioDeviceId* captureDevice = nullptr);
			std::shared_ptr<AudioDevice> OpenPlaybackDevice(const AudioDeviceId* playbackDevice = nullptr);
			std::shared_ptr<AudioEngine> OpenPlaybackEngine(const AudioDeviceId* playbackDevice = nullptr);

			std::vector<AudioDeviceInfo> QueryDevices() const;

			Audio& operator=(const Audio&) = delete;
			Audio& operator=(Audio&&) = delete;

			struct NAZARA_AUDIO_API Config
			{
				void Override(const CommandLineParameters& parameters);

				bool allowNullDevice = true;
				bool noAudio = false;
			};

		private:
			SoundBufferLoader m_soundBufferLoader;
			SoundStreamLoader m_soundStreamLoader;
			ma_context* m_maContext;

			static Audio* s_instance;
	};
}

#endif // NAZARA_AUDIO_HPP
