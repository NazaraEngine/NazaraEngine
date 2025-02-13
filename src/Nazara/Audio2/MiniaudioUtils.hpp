// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_MINIAUDIOUTILS_HPP
#define NAZARA_AUDIO2_MINIAUDIOUTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <miniaudio.h>

namespace Nz
{
	constexpr ma_dither_mode ToMiniaudio(AudioDitherMode ditherMode);
	constexpr ma_channel ToMiniaudio(AudioChannel channel);
	constexpr ma_format ToMiniaudio(AudioFormat format);
}

#include <Nazara/Audio2/MiniaudioUtils.inl>

#endif // NAZARA_AUDIO2_MINIAUDIOUTILS_HPP
