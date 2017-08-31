// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDSTREAM_HPP
#define NAZARA_SOUNDSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>

namespace Nz
{
	class NAZARA_AUDIO_API SoundStream
	{
		public:
			SoundStream() = default;
			virtual ~SoundStream();

			virtual UInt32 GetDuration() const = 0;
			virtual AudioFormat GetFormat() const = 0;
			virtual UInt64 GetSampleCount() const = 0;
			virtual UInt32 GetSampleRate() const = 0;

			virtual UInt64 Read(void* buffer, UInt64 sampleCount) = 0;
			virtual void Seek(UInt64 offset) = 0;
	};
}

#endif // NAZARA_SOUNDSTREAM_HPP
