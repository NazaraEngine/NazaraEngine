// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_AUDIO_HPP
#define NAZARA_ALGORITHM_AUDIO_HPP

#include <Nazara/Prerequesites.hpp>

template<typename T> void NzMixToMono(T* input, T* output, unsigned int channelCount, unsigned int frameCount);

#include <Nazara/Audio/Algorithm.inl>

#endif // NAZARA_ALGORITHM_AUDIO_HPP
