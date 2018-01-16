// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_AUDIO_HPP
#define NAZARA_ALGORITHM_AUDIO_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	template<typename T> void MixToMono(T* input, T* output, UInt32 channelCount, UInt64 frameCount);
}

#include <Nazara/Audio/Algorithm.inl>

#endif // NAZARA_ALGORITHM_AUDIO_HPP
