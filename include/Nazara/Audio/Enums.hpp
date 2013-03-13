// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_HPP
#define NAZARA_ENUMS_HPP

enum nzAudioFormat
{
	nzAudioFormat_Unknown = -1,

	// La valeur entière est le nombre de canaux possédés par ce format
	nzAudioFormat_Mono   = 1,
	nzAudioFormat_Stereo = 2,
	nzAudioFormat_Quad   = 4,
	nzAudioFormat_5_1    = 6,
	nzAudioFormat_6_1    = 7,
	nzAudioFormat_7_1    = 8,

	nzAudioFormat_Max = nzAudioFormat_7_1
};

enum nzSoundStatus
{
	nzSoundStatus_Playing,
	nzSoundStatus_Paused,
	nzSoundStatus_Stopped
};

#endif // NAZARA_ENUMS_HPP
