// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline OpenALSource::OpenALSource(std::shared_ptr<AudioDevice> device, ALuint sourceId) :
	AudioSource(std::move(device)),
	m_sourceId(sourceId)
	{
	}
}

#include <Nazara/Audio/DebugOff.hpp>
