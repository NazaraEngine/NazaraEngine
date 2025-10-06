// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_ENUMS_HPP
#define NAZARA_AUDIO_ENUMS_HPP

#include <NazaraUtils/EnumArray.hpp>
#include <cstddef>

namespace Nz
{
	enum class AudioChannel
	{
		None,

		Mono,
		FrontLeft,
		FrontRight,
		FrontCenter,
		LFE,
		BackLeft,
		BackRight,
		FrontLeftCenter,
		FrontRightCenter,
		BackCenter,
		SideLeft,
		SideRight,
		TopCenter,
		TopFrontLeft,
		TopFrontCenter,
		TopFrontRight,
		TopBackLeft,
		TopBackCenter,
		TopBackRight,
		Aux0,
		Aux1,
		Aux2,
		Aux3,
		Aux4,
		Aux5,
		Aux6,
		Aux7,
		Aux8,
		Aux9,
		Aux10,
		Aux11,
		Aux12,
		Aux13,
		Aux14,
		Aux15,
		Aux16,
		Aux17,
		Aux18,
		Aux19,
		Aux20,
		Aux21,
		Aux22,
		Aux23,
		Aux24,
		Aux25,
		Aux26,
		Aux27,
		Aux28,
		Aux29,
		Aux30,
		Aux31,

		Max = Aux31
	};

	enum class AudioDeviceType
	{
		Capture,
		Playback
	};

	enum class AudioDitherMode
	{
		None,
		Rectangle,
		Triangle,

		Max = Triangle
	};

	enum class AudioFormat
	{
		Unknown,

		Floating32, // f32
		Signed16,   // int16
		Signed24,   // 3*int8
		Signed32,   // int32
		Unsigned8 , // uint8

		Max = Unsigned8
	};

	constexpr Nz::EnumArray<AudioFormat, std::size_t> s_AudioFormatSize = {
		0,
		sizeof(float),
		sizeof(std::int16_t),
		3 * sizeof(std::int8_t),
		sizeof(std::int32_t),
		sizeof(std::uint8_t),
	};

	enum class SoundAttenuationModel
	{
		Exponential,
		Inverse,
		Linear,
		None,

		Max = None
	};

	enum class SoundPanMode
	{
		Balance,
		Pan,

		Max = Pan
	};

	enum class SoundPositioning
	{
		Absolute,
		Relative,

		Max = Relative
	};
}

#endif // NAZARA_AUDIO_ENUMS_HPP
