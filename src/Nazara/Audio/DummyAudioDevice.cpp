// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/DummyAudioDevice.hpp>
#include <Nazara/Audio/DummyAudioBuffer.hpp>
#include <Nazara/Audio/DummyAudioSource.hpp>
#include <cstring>
#include <stdexcept>

namespace Nz
{
	DummyAudioDevice::DummyAudioDevice() :
	m_listenerRotation(Quaternionf::Identity()),
	m_listenerPosition(Vector3f::Zero()),
	m_dopplerFactor(1.f),
	m_globalVolume(1.f),
	m_speedOfSound(343.3f)
	{
	}

	std::shared_ptr<AudioBuffer> DummyAudioDevice::CreateBuffer()
	{
		return std::make_shared<DummyAudioBuffer>(shared_from_this());
	}

	std::shared_ptr<AudioSource> DummyAudioDevice::CreateSource()
	{
		return std::make_shared<DummyAudioSource>(shared_from_this());
	}

	void DummyAudioDevice::DetachThread() const
	{
		// nothing to do
	}

	float DummyAudioDevice::GetDopplerFactor() const
	{
		return m_dopplerFactor;
	}

	float DummyAudioDevice::GetGlobalVolume() const
	{
		return m_globalVolume;
	}

	Vector3f DummyAudioDevice::GetListenerDirection(Vector3f* up) const
	{
		if (up)
			*up = m_listenerRotation * Vector3f::Up();

		return m_listenerRotation * Vector3f::Forward();
	}

	Vector3f DummyAudioDevice::GetListenerPosition() const
	{
		return m_listenerPosition;
	}

	Quaternionf DummyAudioDevice::GetListenerRotation() const
	{
		return m_listenerRotation;
	}

	Vector3f DummyAudioDevice::GetListenerVelocity() const
	{
		return m_listenerVelocity;
	}

	float DummyAudioDevice::GetSpeedOfSound() const
	{
		return m_speedOfSound;
	}

	const void* DummyAudioDevice::GetSubSystemIdentifier() const
	{
		return this;
	}

	bool DummyAudioDevice::IsFormatSupported(AudioFormat /*format*/) const
	{
		return true;
	}

	void DummyAudioDevice::SetDopplerFactor(float dopplerFactor)
	{
		m_dopplerFactor = dopplerFactor;
	}

	void DummyAudioDevice::SetGlobalVolume(float volume)
	{
		m_globalVolume = volume;
	}

	void DummyAudioDevice::SetListenerDirection(const Vector3f& direction, const Vector3f& up)
	{
		m_listenerRotation = Quaternionf::LookAt(direction, up);
	}

	void DummyAudioDevice::SetListenerPosition(const Vector3f& position)
	{
		m_listenerPosition = position;
	}

	void DummyAudioDevice::SetListenerVelocity(const Vector3f& velocity)
	{
		m_listenerVelocity = velocity;
	}

	void DummyAudioDevice::SetSpeedOfSound(float speed)
	{
		m_speedOfSound = speed;
	}
}
