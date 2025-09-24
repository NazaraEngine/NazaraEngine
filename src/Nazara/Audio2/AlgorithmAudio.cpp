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

	UInt64 ConvertFrames(AudioFormat inputFormat, UInt32 inputChannelCount, UInt32 inputSampleRate, const void* input, AudioFormat outputFormat, UInt32 outputChannelCount, UInt32 outputSampleRate, void* output, UInt64 sampleCount)
	{
		return ma_convert_frames(output, sampleCount, ToMiniaudio(outputFormat), outputChannelCount, outputSampleRate, input, sampleCount, ToMiniaudio(inputFormat), inputChannelCount, inputSampleRate);
	}
}
