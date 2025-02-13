// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_SOUNDDATASOURCE_HPP
#define NAZARA_AUDIO2_SOUNDDATASOURCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <NazaraUtils/Result.hpp>
#include <mutex>
#include <span>

namespace Nz
{
	class NAZARA_AUDIO2_API SoundDataSource
	{
		public:
			struct ReadData;

			virtual ~SoundDataSource();

			virtual std::span<const AudioChannel> GetChannels() const = 0;
			virtual AudioFormat GetFormat() const = 0;
			virtual UInt64 GetFrameCount() const = 0;
			virtual UInt32 GetSampleRate() const = 0;
			virtual std::mutex* GetMutex() = 0;

			virtual Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) = 0;

			struct ReadData
			{
				UInt64 readFrame;
				UInt64 cursorPosition;
			};
	};
}

#endif // NAZARA_AUDIO2_SOUNDDATASOURCE_HPP
