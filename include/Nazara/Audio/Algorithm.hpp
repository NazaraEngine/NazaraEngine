// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_ALGORITHM_HPP
#define NAZARA_AUDIO_ALGORITHM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <span>

namespace Nz
{
	NAZARA_AUDIO_API void ConvertAudioFormat(AudioFormat sourceFormat, const void* input, AudioFormat destinationFormat, void* output, UInt64 sampleCount, AudioDitherMode ditherMode = AudioDitherMode::None);
	NAZARA_AUDIO_API UInt64 ConvertFrames(AudioFormat inputFormat, UInt32 inputChannelCount, UInt32 inputSampleRate, const void* input, AudioFormat outputFormat, UInt32 outputChannelCount, UInt32 outputSampleRate, void* output, UInt64 sampleCount);

	inline std::span<const AudioChannel> GetAudioChannelMap(UInt32 channelCount);
}

#include <Nazara/Audio/Algorithm.inl>

#endif // NAZARA_AUDIO_ALGORITHM_HPP
