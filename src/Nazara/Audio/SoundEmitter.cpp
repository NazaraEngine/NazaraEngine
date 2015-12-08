// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://connect.creativelabs.com/openal/Documentation/OpenAL_Programmers_Guide.pdf

#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	SoundEmitter::SoundEmitter()
	{
		alGenSources(1, &m_source);
	}

	SoundEmitter::SoundEmitter(const SoundEmitter& emitter)
	{
		alGenSources(1, &m_source);

		SetAttenuation(emitter.GetAttenuation());
		SetMinDistance(emitter.GetMinDistance());
		SetPitch(emitter.GetPitch());
		// Pas de copie de position ou de vitesse
		SetVolume(emitter.GetVolume());
	}

	SoundEmitter::~SoundEmitter()
	{
		alDeleteSources(1, &m_source);
	}

	void SoundEmitter::EnableSpatialization(bool spatialization)
	{
		alSourcei(m_source, AL_SOURCE_RELATIVE, !spatialization);
	}

	float SoundEmitter::GetAttenuation() const
	{
		ALfloat attenuation;
		alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &attenuation);

		return attenuation;
	}

	float SoundEmitter::GetMinDistance() const
	{
		ALfloat distance;
		alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &distance);

		return distance;
	}

	float SoundEmitter::GetPitch() const
	{
		ALfloat pitch;
		alGetSourcef(m_source, AL_PITCH, &pitch);

		return pitch;
	}

	Vector3f SoundEmitter::GetPosition() const
	{
		Vector3f position;
		alGetSourcefv(m_source, AL_POSITION, position);

		return position;
	}

	Vector3f SoundEmitter::GetVelocity() const
	{
		Vector3f velocity;
		alGetSourcefv(m_source, AL_VELOCITY, velocity);

		return velocity;
	}

	float SoundEmitter::GetVolume() const
	{
		ALfloat gain;
		alGetSourcef(m_source, AL_GAIN, &gain);

		return gain * 100.f;
	}

	bool SoundEmitter::IsSpatialized() const
	{
		ALint relative;
		alGetSourcei(m_source, AL_SOURCE_RELATIVE, &relative);

		return relative == AL_FALSE;
	}

	void SoundEmitter::SetAttenuation(float attenuation)
	{
		alSourcef(m_source, AL_ROLLOFF_FACTOR, attenuation);
	}

	void SoundEmitter::SetMinDistance(float minDistance)
	{
		alSourcef(m_source, AL_REFERENCE_DISTANCE, minDistance);
	}

	void SoundEmitter::SetPitch(float pitch)
	{
		alSourcef(m_source, AL_PITCH, pitch);
	}

	void SoundEmitter::SetPosition(const Vector3f& position)
	{
		alSourcefv(m_source, AL_POSITION, position);
	}

	void SoundEmitter::SetPosition(float x, float y, float z)
	{
		alSource3f(m_source, AL_POSITION, x, y, z);
	}

	void SoundEmitter::SetVelocity(const Vector3f& velocity)
	{
		alSourcefv(m_source, AL_VELOCITY, velocity);
	}

	void SoundEmitter::SetVelocity(float velX, float velY, float velZ)
	{
		alSource3f(m_source, AL_VELOCITY, velX, velY, velZ);
	}

	void SoundEmitter::SetVolume(float volume)
	{
		alSourcef(m_source, AL_GAIN, volume*0.01f);
	}

	SoundStatus SoundEmitter::GetInternalStatus() const
	{
		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);

		switch (state)
		{
			case AL_INITIAL:
			case AL_STOPPED:
				return SoundStatus_Stopped;

			case AL_PAUSED:
				return SoundStatus_Paused;

			case AL_PLAYING:
				return SoundStatus_Playing;

			default:
				NazaraInternalError("Source state unrecognized");
		}

		return SoundStatus_Stopped;
	}
}
