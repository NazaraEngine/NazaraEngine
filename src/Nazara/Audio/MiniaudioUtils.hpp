// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_MINIAUDIOUTILS_HPP
#define NAZARA_AUDIO_MINIAUDIOUTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <miniaudio.h>

namespace Nz
{
	constexpr AudioDitherMode FromMiniaudio(ma_dither_mode ditherMode);
	constexpr AudioChannel FromMiniaudio(ma_channel channel);
	constexpr AudioFormat FromMiniaudio(ma_format format);
	constexpr SoundAttenuationModel FromMiniaudio(ma_attenuation_model attenuation);
	constexpr SoundPanMode FromMiniaudio(ma_pan_mode panMode);
	constexpr SoundPositioning FromMiniaudio(ma_positioning positioning);
	constexpr Vector3f FromMiniaudio(const ma_vec3f& vec);

	constexpr ma_dither_mode ToMiniaudio(AudioDitherMode ditherMode);
	constexpr ma_channel ToMiniaudio(AudioChannel channel);
	constexpr ma_format ToMiniaudio(AudioFormat format);
	constexpr ma_attenuation_model ToMiniaudio(SoundAttenuationModel attenuation);
	constexpr ma_pan_mode ToMiniaudio(SoundPanMode panMode);
	constexpr ma_positioning ToMiniaudio(SoundPositioning positioning);
	constexpr ma_vec3f ToMiniaudio(const Vector3f& vec);
}

#include <Nazara/Audio/MiniaudioUtils.inl>

#endif // NAZARA_AUDIO_MINIAUDIOUTILS_HPP
