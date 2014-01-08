// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Audio/Debug.hpp>

nzAudioFormat NzAudio::GetAudioFormat(unsigned int channelCount)
{
	switch (channelCount)
	{
		case 1:
		case 2:
		case 4:
		case 6:
		case 7:
		case 8:
			return static_cast<nzAudioFormat>(channelCount);

		default:
			NazaraError("Invalid channel count: " + NzString::Number(channelCount));
			return nzAudioFormat_Unknown;
	}
}

float NzAudio::GetDopplerFactor()
{
	return alGetFloat(AL_DOPPLER_FACTOR);
}

float NzAudio::GetGlobalVolume()
{
	ALfloat gain = 0.f;
	alGetListenerf(AL_GAIN, &gain);

	return gain*100.f;
}

NzVector3f NzAudio::GetListenerDirection()
{
	ALfloat orientation[6];
	alGetListenerfv(AL_ORIENTATION, orientation);

	return NzVector3f(orientation[0], orientation[1], orientation[2]);
}

NzVector3f NzAudio::GetListenerPosition()
{
	NzVector3f position;
	alGetListenerfv(AL_POSITION, position);

	return position;
}

NzQuaternionf NzAudio::GetListenerRotation()
{
	float orientation[6];
	alGetListenerfv(AL_ORIENTATION, orientation);

	NzVector3f forward(orientation[0], orientation[1], orientation[2]);

	return NzQuaternionf::RotationBetween(NzVector3f::Forward(), forward);
}

NzVector3f NzAudio::GetListenerVelocity()
{
	NzVector3f velocity;
	alGetListenerfv(AL_VELOCITY, velocity);

	return velocity;
}

float NzAudio::GetSpeedOfSound()
{
	return alGetFloat(AL_SPEED_OF_SOUND);
}

bool NzAudio::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		Uninitialize();

		return false;
	}

	// Initialisation d'OpenGL
	if (!NzOpenAL::Initialize())
	{
		NazaraError("Failed to initialize OpenAL");
		Uninitialize();

		return false;
	}

	// Définition de l'orientation par défaut
	SetListenerDirection(NzVector3f::Forward());

	// Loaders
	NzLoaders_sndfile_Register();

	NazaraNotice("Initialized: Audio module");

	return true;
}

bool NzAudio::IsFormatSupported(nzAudioFormat format)
{
	if (format == nzAudioFormat_Unknown)
		return false;

	return NzOpenAL::AudioFormat[format] != 0;
}

bool NzAudio::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzAudio::SetDopplerFactor(float dopplerFactor)
{
	alDopplerFactor(dopplerFactor);
}

void NzAudio::SetGlobalVolume(float volume)
{
	alListenerf(AL_GAIN, volume*0.01f);
}

void NzAudio::SetListenerDirection(const NzVector3f& direction)
{
	NzVector3f up = NzVector3f::Up();

	ALfloat orientation[6] =
	{
		direction.x, direction.y, direction.z,
		up.x, up.y, up.z
	};

	alListenerfv(AL_ORIENTATION, orientation);
}

void NzAudio::SetListenerDirection(float dirX, float dirY, float dirZ)
{
	NzVector3f up = NzVector3f::Up();

	ALfloat orientation[6] =
	{
		dirX, dirY, dirZ,
		up.x, up.y, up.z
	};

	alListenerfv(AL_ORIENTATION, orientation);
}

void NzAudio::SetListenerPosition(const NzVector3f& position)
{
	alListenerfv(AL_POSITION, position);
}

void NzAudio::SetListenerPosition(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
}

void NzAudio::SetListenerRotation(const NzQuaternionf& rotation)
{
	NzVector3f forward = rotation * NzVector3f::Forward();
	NzVector3f up = NzVector3f::Up();

	ALfloat orientation[6] =
	{
		forward.x, forward.y, forward.z,
		up.x, up.y, up.z
	};

	alListenerfv(AL_ORIENTATION, orientation);
}

void NzAudio::SetListenerVelocity(const NzVector3f& velocity)
{
	alListenerfv(AL_VELOCITY, velocity);
}

void NzAudio::SetListenerVelocity(float velX, float velY, float velZ)
{
	alListener3f(AL_VELOCITY, velX, velY, velZ);
}

void NzAudio::SetSpeedOfSound(float speed)
{
	alSpeedOfSound(speed);
}

void NzAudio::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	// Loaders
	NzLoaders_sndfile_Unregister();

	NzOpenAL::Uninitialize();

	NazaraNotice("Uninitialized: Audio module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzAudio::s_moduleReferenceCounter = 0;
