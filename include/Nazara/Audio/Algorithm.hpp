// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_ALGORITHM_HPP
#define NAZARA_AUDIO_ALGORITHM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <optional>

namespace Nz
{
	inline UInt32 GetChannelCount(AudioFormat format);
	inline std::optional<AudioFormat> GuessAudioFormat(UInt32 channelCount);
	template<typename T> void MixToMono(T* input, T* output, UInt32 channelCount, UInt64 frameCount);
}

#include <Nazara/Audio/Algorithm.inl>

#endif // NAZARA_AUDIO_ALGORITHM_HPP
