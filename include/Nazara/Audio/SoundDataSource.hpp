// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDDATASOURCE_HPP
#define NAZARA_AUDIO_SOUNDDATASOURCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <NazaraUtils/Result.hpp>
#include <mutex>
#include <span>

namespace Nz
{
	class NAZARA_AUDIO_API SoundDataSource
	{
		public:
			struct ReadData;

			virtual ~SoundDataSource();

			virtual std::span<const AudioChannel> GetChannels() const = 0;
			virtual Time GetDuration() const = 0;
			virtual AudioFormat GetFormat() const = 0;
			virtual UInt64 GetFrameCount() const = 0;
			virtual std::mutex* GetMutex() = 0;
			virtual UInt32 GetSampleRate() const = 0;

			virtual Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) = 0;

			struct ReadData
			{
				UInt64 readFrame;
				UInt64 cursorPosition;
			};
	};
}

#endif // NAZARA_AUDIO_SOUNDDATASOURCE_HPP
