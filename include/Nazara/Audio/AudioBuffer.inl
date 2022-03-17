// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/AudioBuffer.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline AudioBuffer::AudioBuffer(std::shared_ptr<AudioDevice> device) :
	m_device(std::move(device))
	{
	}

	inline const std::shared_ptr<AudioDevice>& AudioBuffer::GetAudioDevice() const
	{
		return m_device;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
