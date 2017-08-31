// Copyright (C) 2017 Jérôme Leclercq
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
	/*!
	* \ingroup audio
	* \class Nz::Audio
	* \brief Audio class that represents the module initializer of Audio
	*/

	/*!
	* \brief Gets the format of the audio
	* \return AudioFormat Enumeration type for the format
	*
	* \param channelCount Number of channels
	*
	* \remark Produces a NazaraError if the number of channels is erroneous (3 or 5) and AudioFormat_Unknown is returned
	*/

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

	/*!
	* \brief Gets the factor of the doppler effect
	* \return Global factor of the doppler effect
	*/

	float Audio::GetDopplerFactor()
	{
		return alGetFloat(AL_DOPPLER_FACTOR);
	}

	/*!
	* \brief Gets the global volume
	* \return Float between [0, inf) with 100.f being the default
	*/

	float Audio::GetGlobalVolume()
	{
		ALfloat gain = 0.f;
		alGetListenerf(AL_GAIN, &gain);

		return gain * 100.f;
	}

	/*!
	* \brief Gets the direction of the listener
	* \return Direction of the listener, in front of the listener
	*
	* \see GetListenerRotation
	*/

	Vector3f Audio::GetListenerDirection()
	{
		ALfloat orientation[6];
		alGetListenerfv(AL_ORIENTATION, orientation);

		return Vector3f(orientation[0], orientation[1], orientation[2]);
	}

	/*!
	* \brief Gets the position of the listener
	* \return Position of the listener
	*
	* \see GetListenerVelocity
	*/

	Vector3f Audio::GetListenerPosition()
	{
		Vector3f position;
		alGetListenerfv(AL_POSITION, position);

		return position;
	}

	/*!
	* \brief Gets the rotation of the listener
	* \return Rotation of the listener
	*/

	Quaternionf Audio::GetListenerRotation()
	{
		ALfloat orientation[6];
		alGetListenerfv(AL_ORIENTATION, orientation);

		Vector3f forward(orientation[0], orientation[1], orientation[2]);

		return Quaternionf::RotationBetween(Vector3f::Forward(), forward);
	}

	/*!
	* \brief Gets the velocity of the listener
	* \return Velocity of the listener
	*
	* \see GetListenerPosition
	*/

	Vector3f Audio::GetListenerVelocity()
	{
		Vector3f velocity;
		alGetListenerfv(AL_VELOCITY, velocity);

		return velocity;
	}

	/*!
	* \brief Gets the speed of sound
	* \return Speed of sound
	*/

	float Audio::GetSpeedOfSound()
	{
		return alGetFloat(AL_SPEED_OF_SOUND);
	}

	/*!
	* \brief Initializes the Audio module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraError if initialization of modules Core, OpenAL or SoundBuffer failed
	* \remark Produces a NazaraNotice
	*/

	bool Audio::Initialize()
	{
		if (IsInitialized())
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialisation of dependencies
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation of the module
		CallOnExit onExit(Audio::Uninitialize);

		// Initialisation of OpenAL
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

		// Definition of the orientation by default
		SetListenerDirection(Vector3f::Forward());

		// Loaders
		Loaders::Register_sndfile();

		onExit.Reset();

		NazaraNotice("Initialized: Audio module");
		return true;
	}

	/*!
	* \brief Checks whether the format is supported by the engine
	* \return true if it is the case
	*
	* \param format Format to check
	*/

	bool Audio::IsFormatSupported(AudioFormat format)
	{
		if (format == AudioFormat_Unknown)
			return false;

		return OpenAL::AudioFormat[format] != 0;
	}

	/*!
	* \brief Checks whether the module is initialized
	* \return true if module is initialized
	*/

	bool Audio::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	/*!
	* \brief Sets the factor of the doppler effect
	*
	* \param dopplerFactor Global factor of the doppler effect
	*/

	void Audio::SetDopplerFactor(float dopplerFactor)
	{
		alDopplerFactor(dopplerFactor);
	}

	/*!
	* \brief Sets the global volume
	*
	* \param volume Float between [0, inf) with 100.f being the default
	*/

	void Audio::SetGlobalVolume(float volume)
	{
		alListenerf(AL_GAIN, volume * 0.01f);
	}

	/*!
	* \brief Sets the direction of the listener
	*
	* \param direction Direction of the listener, in front of the listener
	*
	* \see SetListenerDirection, SetListenerRotation
	*/

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

	/*!
	* \brief Sets the direction of the listener
	*
	* \param (dirX, dirY, dirZ) Direction of the listener, in front of the listener
	*
	* \see SetListenerDirection, SetListenerRotation
	*/

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

	/*!
	* \brief Sets the position of the listener
	*
	* \param position Position of the listener
	*
	* \see SetListenerVelocity
	*/

	void Audio::SetListenerPosition(const Vector3f& position)
	{
		alListenerfv(AL_POSITION, position);
	}

	/*!
	* \brief Sets the position of the listener
	*
	* \param (x, y, z) Position of the listener
	*
	* \see SetListenerVelocity
	*/

	void Audio::SetListenerPosition(float x, float y, float z)
	{
		alListener3f(AL_POSITION, x, y, z);
	}

	/*!
	* \brief Sets the rotation of the listener
	*
	* \param rotation Rotation of the listener
	*/

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

	/*!
	* \brief Sets the velocity of the listener
	*
	* \param velocity Velocity of the listener
	*
	* \see SetListenerPosition
	*/

	void Audio::SetListenerVelocity(const Vector3f& velocity)
	{
		alListenerfv(AL_VELOCITY, velocity);
	}

	/*!
	* \brief Sets the velocity of the listener
	*
	* \param (velX, velY, velZ) Velocity of the listener
	*
	* \see SetListenerPosition
	*/

	void Audio::SetListenerVelocity(float velX, float velY, float velZ)
	{
		alListener3f(AL_VELOCITY, velX, velY, velZ);
	}

	/*!
	* \brief Sets the speed of sound
	*
	* \param speed Speed of sound
	*/

	void Audio::SetSpeedOfSound(float speed)
	{
		alSpeedOfSound(speed);
	}

	/*!
	* \brief Uninitializes the Audio module
	*
	* \remark Produces a NazaraNotice
	*/

	void Audio::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// The module is still in use, or can not be uninitialized
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Free of module
		s_moduleReferenceCounter = 0;

		// Loaders
		Loaders::Unregister_sndfile();

		SoundBuffer::Uninitialize();
		OpenAL::Uninitialize();

		NazaraNotice("Uninitialized: Audio module");

		// Free of dependencies
		Core::Uninitialize();
	}

	unsigned int Audio::s_moduleReferenceCounter = 0;
}
