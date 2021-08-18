// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://connect.creativelabs.com/openal/Documentation/OpenAL_Programmers_Guide.pdf

#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \class Nz::SoundEmitter
	* \brief Audio class that represents a sound source, that emits sound
	*
	* \remark Module Audio needs to be initialized to use this class
	* \remark This class is abstract
	*/

	/*!
	* \brief Constructs a SoundEmitter object
	*/
	SoundEmitter::SoundEmitter()
	{
		alGenSources(1, &m_source);
	}

	/*!
	* \brief Constructs a SoundEmitter object which is a copy of another
	*
	* \param emitter SoundEmitter to copy
	*
	* \remark Position and velocity are not copied
	*/

	SoundEmitter::SoundEmitter(const SoundEmitter& emitter)
	{
		if (emitter.m_source != InvalidSource)
		{
			alGenSources(1, &m_source);

			SetAttenuation(emitter.GetAttenuation());
			SetMinDistance(emitter.GetMinDistance());
			SetPitch(emitter.GetPitch());
			// No copy for position or velocity
			SetVolume(emitter.GetVolume());
		}
		else
			m_source = InvalidSource;
	}

	/*!
	* \brief Constructs a SoundEmitter object by moving another
	*
	* \param emitter SoundEmitter to move
	*
	* \remark The moved sound emitter cannot be used after being moved
	*/
	SoundEmitter::SoundEmitter(SoundEmitter&& emitter) noexcept :
	m_source(emitter.m_source)
	{
		emitter.m_source = InvalidSource;
	}

	/*!
	* \brief Destructs the object
	*/
	SoundEmitter::~SoundEmitter()
	{
		if (m_source != InvalidSource)
			alDeleteSources(1, &m_source);
	}

	/*!
	* \brief Enables spatialization
	*
	* \param spatialization True if spatialization is enabled
	*/

	void SoundEmitter::EnableSpatialization(bool spatialization)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcei(m_source, AL_SOURCE_RELATIVE, !spatialization);
	}

	/*!
	* \brief Gets the attenuation
	* \return Amount that your sound will drop off as by the inverse square law 
	*/

	float SoundEmitter::GetAttenuation() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALfloat attenuation;
		alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &attenuation);

		return attenuation;
	}

	/*!
	* \brief Gets the minimum distance to hear
	* \return Distance to begin to hear
	*/

	float SoundEmitter::GetMinDistance() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALfloat distance;
		alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &distance);

		return distance;
	}

	/*!
	* \brief Gets the pitch
	* \return Pitch of the sound
	*/

	float SoundEmitter::GetPitch() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALfloat pitch;
		alGetSourcef(m_source, AL_PITCH, &pitch);

		return pitch;
	}

	/*!
	* \brief Gets the position of the emitter
	* \return Position of the sound
	*/

	Vector3f SoundEmitter::GetPosition() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		Vector3f position;
		alGetSourcefv(m_source, AL_POSITION, &position.x);

		return position;
	}

	/*!
	* \brief Gets the velocity of the emitter
	* \return Velocity of the sound
	*/

	Vector3f SoundEmitter::GetVelocity() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		Vector3f velocity;
		alGetSourcefv(m_source, AL_VELOCITY, &velocity.x);

		return velocity;
	}

	/*!
	* \brief Gets the volume of the emitter
	* \param volume Float between [0, inf) with 100.f being the default
	*/

	float SoundEmitter::GetVolume() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALfloat gain;
		alGetSourcef(m_source, AL_GAIN, &gain);

		return gain * 100.f;
	}

	/*!
	* \brief Checks whether the sound emitter has spatialization enabled
	* \return true if it the case
	*/

	bool SoundEmitter::IsSpatialized() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALint relative;
		alGetSourcei(m_source, AL_SOURCE_RELATIVE, &relative);

		return relative == AL_FALSE;
	}

	/*!
	* \brief Sets the attenuation
	*
	* \param attenuation Amount that your sound will drop off as by the inverse square law 
	*/

	void SoundEmitter::SetAttenuation(float attenuation)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcef(m_source, AL_ROLLOFF_FACTOR, attenuation);
	}

	/*!
	* \brief Sets the minimum distance to hear
	*
	* \param minDistance to begin to hear
	*/

	void SoundEmitter::SetMinDistance(float minDistance)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcef(m_source, AL_REFERENCE_DISTANCE, minDistance);
	}

	/*!
	* \brief Sets the pitch
	*
	* \param pitch of the sound
	*/

	void SoundEmitter::SetPitch(float pitch)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcef(m_source, AL_PITCH, pitch);
	}

	/*!
	* \brief Sets the position of the emitter
	*
	* \param position Position of the sound
	*/

	void SoundEmitter::SetPosition(const Vector3f& position)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcefv(m_source, AL_POSITION, &position.x);
	}

	/*!
	* \brief Sets the position of the emitter
	*
	* \param position Position of the sound with (x, y, z)
	*/

	void SoundEmitter::SetPosition(float x, float y, float z)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSource3f(m_source, AL_POSITION, x, y, z);
	}

	/*!
	* \brief Sets the velocity of the emitter
	*
	* \param velocity Velocity of the sound
	*/

	void SoundEmitter::SetVelocity(const Vector3f& velocity)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcefv(m_source, AL_VELOCITY, &velocity.x);
	}

	/*!
	* \brief Sets the velocity of the emitter
	*
	* \param velocity Velocity with (velX, velY, velZ)
	*/

	void SoundEmitter::SetVelocity(float velX, float velY, float velZ)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSource3f(m_source, AL_VELOCITY, velX, velY, velZ);
	}

	/*!
	* \brief Sets the volume of the emitter
	*
	* \param volume Float between [0, inf) with 100.f being the default
	*/

	void SoundEmitter::SetVolume(float volume)
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		alSourcef(m_source, AL_GAIN, volume * 0.01f);
	}

	/*!
	* \brief Assign a sound emitter by moving it
	*
	* \param emitter SoundEmitter to move
	*
	* \return *this
	*/
	SoundEmitter& SoundEmitter::operator=(SoundEmitter&& emitter) noexcept
	{
		std::swap(m_source, emitter.m_source);

		return *this;
	}

	/*!
	* \brief Gets the status of the sound emitter
	* \return Enumeration of type SoundStatus (Playing, Stopped, ...)
	*/

	SoundStatus SoundEmitter::GetInternalStatus() const
	{
		NazaraAssert(m_source != InvalidSource, "Invalid sound emitter");

		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);

		switch (state)
		{
			case AL_INITIAL:
			case AL_STOPPED:
				return SoundStatus::Stopped;

			case AL_PAUSED:
				return SoundStatus::Paused;

			case AL_PLAYING:
				return SoundStatus::Playing;

			default:
				NazaraInternalError("Source state unrecognized");
		}

		return SoundStatus::Stopped;
	}
}
