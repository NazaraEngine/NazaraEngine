// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_AUDIO_HPP
#define NAZARA_ENUMS_AUDIO_HPP

namespace Nz
{
	enum class AudioFormat
	{
		Unknown = -1,

		I16_Mono,
		I16_Stereo,
		I16_Quad,
		I16_5_1,
		I16_6_1,
		I16_7_1,

		Max = I16_7_1
	};

	constexpr std::size_t AudioFormatCount = static_cast<std::size_t>(AudioFormat::Max) + 1;

	enum class SoundStatus
	{
		Playing,
		Paused,
		Stopped,

		Max = Stopped
	};

	constexpr std::size_t SoundStatusCount = static_cast<std::size_t>(SoundStatus::Max) + 1;
}

#endif // NAZARA_ENUMS_AUDIO_HPP
