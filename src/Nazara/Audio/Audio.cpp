// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/Formats/sndfileLoader.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	AudioFormat Audio::GetAudioFormat(unsigned int channelCount)
	{
		switch (channelCount)
		{
			case 1:
			case 2:
			case 4:
			case 6:
			case 7:
			case 8:
				return static_cast<AudioFormat>(channelCount);

			default:
				NazaraError("Invalid channel count: " + String::Number(channelCount));
				return AudioFormat_Unknown;
		}
	}

	float Audio::GetDopplerFactor()
	{
		return alGetFloat(AL_DOPPLER_FACTOR);
	}

	float Audio::GetGlobalVolume()
	{
		ALfloat gain = 0.f;
		alGetListenerf(AL_GAIN, &gain);

		return gain*100.f;
	}

	Vector3f Audio::GetListenerDirection()
	{
		ALfloat orientation[6];
		alGetListenerfv(AL_ORIENTATION, orientation);

		return Vector3f(orientation[0], orientation[1], orientation[2]);
	}

	Vector3f Audio::GetListenerPosition()
	{
		Vector3f position;
		alGetListenerfv(AL_POSITION, position);

		return position;
	}

	Quaternionf Audio::GetListenerRotation()
	{
		ALfloat orientation[6];
		alGetListenerfv(AL_ORIENTATION, orientation);

		Vector3f forward(orientation[0], orientation[1], orientation[2]);

		return Quaternionf::RotationBetween(Vector3f::Forward(), forward);
	}

	Vector3f Audio::GetListenerVelocity()
	{
		Vector3f velocity;
		alGetListenerfv(AL_VELOCITY, velocity);

		return velocity;
	}

	float Audio::GetSpeedOfSound()
	{
		return alGetFloat(AL_SPEED_OF_SOUND);
	}

	bool Audio::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Déjà initialisé
		}

		// Initialisation des dépendances
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation du module
		CallOnExit onExit(Audio::Uninitialize);

		// Initialisation d'OpenAL
		if (!OpenAL::Initialize())
		{
			NazaraError("Failed to initialize OpenAL");
			return false;
		}

		if (!SoundBuffer::Initialize())
		{
			NazaraError("Failed to initialize sound buffers");
			return false;
		}

		// Définition de l'orientation par défaut
		SetListenerDirection(Vector3f::Forward());

		// Loaders
		Loaders::Register_sndfile();

		onExit.Reset();

		NazaraNotice("Initialized: Audio module");
		return true;
	}

	bool Audio::IsFormatSupported(AudioFormat format)
	{
		if (format == AudioFormat_Unknown)
			return false;

		return OpenAL::AudioFormat[format] != 0;
	}

	bool Audio::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Audio::SetDopplerFactor(float dopplerFactor)
	{
		alDopplerFactor(dopplerFactor);
	}

	void Audio::SetGlobalVolume(float volume)
	{
		alListenerf(AL_GAIN, volume*0.01f);
	}

	void Audio::SetListenerDirection(const Vector3f& direction)
	{
		Vector3f up = Vector3f::Up();

		ALfloat orientation[6] =
		{
			direction.x, direction.y, direction.z,
			up.x, up.y, up.z
		};

		alListenerfv(AL_ORIENTATION, orientation);
	}

	void Audio::SetListenerDirection(float dirX, float dirY, float dirZ)
	{
		Vector3f up = Vector3f::Up();

		ALfloat orientation[6] =
		{
			dirX, dirY, dirZ,
			up.x, up.y, up.z
		};

		alListenerfv(AL_ORIENTATION, orientation);
	}

	void Audio::SetListenerPosition(const Vector3f& position)
	{
		alListenerfv(AL_POSITION, position);
	}

	void Audio::SetListenerPosition(float x, float y, float z)
	{
		alListener3f(AL_POSITION, x, y, z);
	}

	void Audio::SetListenerRotation(const Quaternionf& rotation)
	{
		Vector3f forward = rotation * Vector3f::Forward();
		Vector3f up = Vector3f::Up();

		ALfloat orientation[6] =
		{
			forward.x, forward.y, forward.z,
			up.x, up.y, up.z
		};

		alListenerfv(AL_ORIENTATION, orientation);
	}

	void Audio::SetListenerVelocity(const Vector3f& velocity)
	{
		alListenerfv(AL_VELOCITY, velocity);
	}

	void Audio::SetListenerVelocity(float velX, float velY, float velZ)
	{
		alListener3f(AL_VELOCITY, velX, velY, velZ);
	}

	void Audio::SetSpeedOfSound(float speed)
	{
		alSpeedOfSound(speed);
	}

	void Audio::Uninitialize()
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
		Loaders::Unregister_sndfile();

		SoundBuffer::Uninitialize();
		OpenAL::Uninitialize();

		NazaraNotice("Uninitialized: Audio module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Audio::s_moduleReferenceCounter = 0;
}
