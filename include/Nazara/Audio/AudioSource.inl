// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline AudioSource::AudioSource(std::shared_ptr<AudioDevice> device) :
	m_device(std::move(device))
	{
	}

	inline const std::shared_ptr<AudioDevice>& AudioSource::GetAudioDevice() const
	{
		return m_device;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
