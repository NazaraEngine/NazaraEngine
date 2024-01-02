// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline OpenALBuffer::OpenALBuffer(std::shared_ptr<AudioDevice> device, ALuint bufferId) :
	AudioBuffer(std::move(device)),
	m_bufferId(bufferId)
	{
	}

	inline ALuint OpenALBuffer::GetBufferId() const
	{
		return m_bufferId;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
