// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://connect.creativelabs.com/openal/Documentation/OpenAL_Programmers_Guide.pdf

#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Core/Error.hpp>
#include <AL/al.h>
#include <Nazara/Audio/Debug.hpp>

NzSoundEmitter::NzSoundEmitter()
{
	alGenSources(1, &m_source);
}

NzSoundEmitter::NzSoundEmitter(const NzSoundEmitter& emitter)
{
	alGenSources(1, &m_source);

	SetAttenuation(emitter.GetAttenuation());
	SetMinDistance(emitter.GetMinDistance());
	SetPitch(emitter.GetPitch());
	// Pas de copie de position ou de vitesse
	SetVolume(emitter.GetVolume());
}

NzSoundEmitter::~NzSoundEmitter()
{
	alDeleteSources(1, &m_source);
}

void NzSoundEmitter::EnableSpatialization(bool spatialization)
{
	alSourcei(m_source, AL_SOURCE_RELATIVE, spatialization);
}

float NzSoundEmitter::GetAttenuation() const
{
	ALfloat attenuation;
	alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &attenuation);

	return attenuation;
}

float NzSoundEmitter::GetMinDistance() const
{
	ALfloat distance;
	alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &distance);

	return distance;
}

float NzSoundEmitter::GetPitch() const
{
	ALfloat pitch;
	alGetSourcef(m_source, AL_PITCH, &pitch);

	return pitch;
}

NzVector3f NzSoundEmitter::GetPosition() const
{
	NzVector3f position;
	alGetSourcefv(m_source, AL_POSITION, position);

	return position;
}

NzVector3f NzSoundEmitter::GetVelocity() const
{
	NzVector3f velocity;
	alGetSourcefv(m_source, AL_VELOCITY, velocity);

	return velocity;
}

float NzSoundEmitter::GetVolume() const
{
	ALfloat gain;
	alGetSourcef(m_source, AL_GAIN, &gain);

	return gain * 100.f;
}

bool NzSoundEmitter::IsSpatialized() const
{
	ALint relative;
	alGetSourcei(m_source, AL_SOURCE_RELATIVE, &relative);

	return relative != AL_FALSE;
}

void NzSoundEmitter::SetAttenuation(float attenuation)
{
	alSourcef(m_source, AL_ROLLOFF_FACTOR, attenuation);
}

void NzSoundEmitter::SetMinDistance(float minDistance)
{
	alSourcef(m_source, AL_REFERENCE_DISTANCE, minDistance);
}

void NzSoundEmitter::SetPitch(float pitch)
{
	alSourcef(m_source, AL_PITCH, pitch);
}

void NzSoundEmitter::SetPosition(const NzVector3f& position)
{
	alSourcefv(m_source, AL_POSITION, position);
}

void NzSoundEmitter::SetPosition(float x, float y, float z)
{
	alSource3f(m_source, AL_POSITION, x, y, z);
}

void NzSoundEmitter::SetVelocity(const NzVector3f& velocity)
{
	alSourcefv(m_source, AL_VELOCITY, velocity);
}

void NzSoundEmitter::SetVelocity(float velX, float velY, float velZ)
{
	alSource3f(m_source, AL_VELOCITY, velX, velY, velZ);
}

void NzSoundEmitter::SetVolume(float volume)
{
	alSourcef(m_source, AL_GAIN, volume*0.01f);
}

nzSoundStatus NzSoundEmitter::GetInternalStatus() const
{
	ALint state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);

	switch (state)
	{
		case AL_INITIAL:
		case AL_STOPPED:
			return nzSoundStatus_Stopped;

		case AL_PAUSED:
			return nzSoundStatus_Paused;

		case AL_PLAYING:
			return nzSoundStatus_Playing;

		default:
			NazaraInternalError("Source state unrecognized");
	}

	return nzSoundStatus_Stopped;
}
