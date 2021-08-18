// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Checks whether the sound is playing
	* \return true if it is the case
	*/
	bool SoundEmitter::IsPlaying() const
	{
		return GetStatus() == SoundStatus::Playing;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
