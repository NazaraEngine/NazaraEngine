// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_HPP
#define NAZARA_AUDIO_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Core.hpp>

namespace Nz
{
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

			const std::shared_ptr<AudioDevice>& GetDefaultDevice() const;

			SoundBufferLoader& GetSoundBufferLoader();
			const SoundBufferLoader& GetSoundBufferLoader() const;
			SoundStreamLoader& GetSoundStreamLoader();
			const SoundStreamLoader& GetSoundStreamLoader() const;

			std::shared_ptr<AudioDevice> OpenOutputDevice(const std::string& deviceName);

			std::vector<std::string> QueryInputDevices() const;
			std::vector<std::string> QueryOutputDevices() const;

			Audio& operator=(const Audio&) = delete;
			Audio& operator=(Audio&&) = delete;

			struct Config
			{
				bool allowDummyDevice = true;
				bool noAudio = false;
			};

		private:
			std::shared_ptr<AudioDevice> m_defaultDevice;
			SoundBufferLoader m_soundBufferLoader;
			SoundStreamLoader m_soundStreamLoader;
			bool m_hasDummyDevice;

			static Audio* s_instance;
	};
}

#endif // NAZARA_AUDIO_HPP
