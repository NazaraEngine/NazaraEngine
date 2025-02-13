// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_ALGORITHM_HPP
#define NAZARA_AUDIO2_ALGORITHM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <span>

namespace Nz
{
	void ConvertAudioFormat(AudioFormat sourceFormat, const void* input, AudioFormat destinationFormat, void* output, UInt64 sampleCount, AudioDitherMode ditherMode = AudioDitherMode::None);

	inline std::span<const AudioChannel> GetAudioChannelMap(UInt32 channelCount);
}

#include <Nazara/Audio2/Algorithm.inl>

#endif // NAZARA_AUDIO2_ALGORITHM_HPP
