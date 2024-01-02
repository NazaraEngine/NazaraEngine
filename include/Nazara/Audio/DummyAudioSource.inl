// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline DummyAudioSource::DummyAudioSource(std::shared_ptr<AudioDevice> device) :
	AudioSource(std::move(device)),
	m_playClock(Time::Zero(), true),
	m_status(SoundStatus::Stopped),
	m_position(Vector3f::Zero()),
	m_velocity(Vector3f::Zero()),
	m_isLooping(false),
	m_isSpatialized(true),
	m_attenuation(1.f),
	m_minDistance(1.f),
	m_pitch(1.f),
	m_volume(1.f)
	{
	}
}

#include <Nazara/Audio/DebugOff.hpp>
