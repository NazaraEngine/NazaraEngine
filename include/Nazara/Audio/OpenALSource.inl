// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline OpenALSource::OpenALSource(std::shared_ptr<AudioDevice> device, OpenALLibrary& library, ALuint sourceId) :
	AudioSource(std::move(device)),
	m_sourceId(sourceId),
	m_library(library)
	{
	}
}

#include <Nazara/Audio/DebugOff.hpp>
