// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/Algorithm.hpp>
#include <Nazara/Audio2/MiniaudioUtils.hpp>
#include <miniaudio.h>

namespace Nz
{
	void ConvertAudioFormat(AudioFormat sourceFormat, const void* input, AudioFormat destinationFormat, void* output, UInt64 sampleCount, AudioDitherMode ditherMode)
	{
		ma_pcm_convert(output, ToMiniaudio(destinationFormat), input, ToMiniaudio(sourceFormat), sampleCount, ToMiniaudio(ditherMode));
	}
}
