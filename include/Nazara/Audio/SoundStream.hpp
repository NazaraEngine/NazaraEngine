// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDSTREAM_HPP
#define NAZARA_SOUNDSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>

class NAZARA_AUDIO_API NzSoundStream
{
	public:
		NzSoundStream() = default;
		virtual ~NzSoundStream();

		virtual nzUInt32 GetDuration() const = 0;
		virtual nzAudioFormat GetFormat() const = 0;
		virtual nzUInt32 GetSampleCount() const = 0;
		virtual nzUInt32 GetSampleRate() const = 0;

		virtual unsigned int Read(void* buffer, unsigned int sampleCount) = 0;
		virtual void Seek(nzUInt32 offset) = 0;
};

#endif // NAZARA_SOUNDSTREAM_HPP
