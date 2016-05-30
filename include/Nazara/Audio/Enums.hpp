// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_AUDIO_HPP
#define NAZARA_ENUMS_AUDIO_HPP

namespace Nz
{
	enum AudioFormat
	{
		AudioFormat_Unknown = -1,

		// The integer value is the number of channels used by the format
		AudioFormat_Mono   = 1,
		AudioFormat_Stereo = 2,
		AudioFormat_Quad   = 4,
		AudioFormat_5_1    = 6,
		AudioFormat_6_1    = 7,
		AudioFormat_7_1    = 8,

		AudioFormat_Max = AudioFormat_7_1
	};

	enum SoundStatus
	{
		SoundStatus_Playing,
		SoundStatus_Paused,
		SoundStatus_Stopped
	};
}

#endif // NAZARA_ENUMS_AUDIO_HPP
