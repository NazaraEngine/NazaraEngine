// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <Nazara/Audio/Debug.hpp>

namespace
{
	ALenum formats[nzAudioFormat_Max+1] = {0};
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
}

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
/*
NzQuaternionf NzAudio::GetListenerRotation()
{
	// http://stackoverflow.com/questions/1171849/finding-quaternion-representing-the-rotation-from-one-vector-to-another
	float orientation[6];
	alGetListenerfv(AL_ORIENTATION, orientation);

	NzVector3f forward(orientation[0], orientation[1], orientation[2]);
	NzVector3f up(orientation[3], orientation[4], orientation[5]);

	NzQuaternionf rotation;
	NzVector3f a = NzVector3f::CrossProduct(forward, up);
	rotation.x = a.x;
	rotation.y = a.y;
	rotation.z = a.z;
	rotation.w = std::sqrt(forward.SquaredLength() * up.SquaredLength()) + NzVector3f::DotProduct(forward, up);

	return rotation;
}
*/
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
	if (s_moduleReferenceCouter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	// Initialisation du module
	device = alcOpenDevice(nullptr); // On choisit le device par défaut
	if (!device)
	{
		NazaraError("Failed to open default device");
		return false;
	}

	// Un seul contexte nous suffira
	context = alcCreateContext(device, nullptr);
	if (!context)
	{
		NazaraError("Failed to create context");

		alcCloseDevice(device);
		return false;
	}

	if (!alcMakeContextCurrent(context))
	{
		NazaraError("Failed to activate context");

		alcDestroyContext(context);
		alcCloseDevice(device);
		return false;
	}

	// Définition de l'orientation
	/*{
		NzVector3f forward = NzVector3f::Forward();
		NzVector3f up = NzVector3f::Up();

		ALfloat orientation[6] =
		{
			forward.x, forward.y, forward.z,
			up.x, up.y, up.z
		};

		alListenerfv(AL_ORIENTATION, orientation);
	}*/

	formats[nzAudioFormat_Mono]   = AL_FORMAT_MONO16;
	formats[nzAudioFormat_Stereo] = AL_FORMAT_STEREO16;
	formats[nzAudioFormat_Quad]   = alGetEnumValue("AL_FORMAT_QUAD16");
	formats[nzAudioFormat_5_1]    = alGetEnumValue("AL_FORMAT_51CHN16");
	formats[nzAudioFormat_6_1]    = alGetEnumValue("AL_FORMAT_61CHN16");
	formats[nzAudioFormat_7_1]    = alGetEnumValue("AL_FORMAT_71CHN16");

	NzLoaders_sndfile_Register();

	NazaraNotice("Initialized: Audio module");

	return true;
}

bool NzAudio::IsFormatSupported(nzAudioFormat format)
{
	if (format == nzAudioFormat_Unknown)
		return false;

	return formats[format] != 0;
}

bool NzAudio::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
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
	ALfloat orientation[6] =
	{
		direction.x, direction.y, direction.z,
		0.f, 1.f, 0.f
	};

	alListenerfv(AL_ORIENTATION, orientation);
}

void NzAudio::SetListenerDirection(float dirX, float dirY, float dirZ)
{
	ALfloat orientation[6] =
	{
		dirX, dirY, dirZ,
		0.f, 1.f, 0.f
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
/*
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
*/
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
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilisé

	// Libération du module
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);

	if (!alcCloseDevice(device))
		// Nous n'avons pas pu fermer le device, ce qui signifie qu'il est en cours d'utilisation
		NazaraWarning("Failed to close device");

	NazaraNotice("Uninitialized: Audio module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzAudio::GetOpenALFormat(nzAudioFormat format)
{
	#ifdef NAZARA_DEBUG
	if (format == nzAudioFormat_Unknown)
	{
		NazaraInternalError("Invalid audio format");
		return 0;
	}
	#endif

	return formats[format];
}

unsigned int NzAudio::s_moduleReferenceCouter = 0;
