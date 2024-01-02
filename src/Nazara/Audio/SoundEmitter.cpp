// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/AudioSource.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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
	SoundEmitter::SoundEmitter(AudioDevice& audioDevice) :
	m_source(audioDevice.CreateSource())
	{
		if (!m_source)
		{
			ErrorFlags errFlags(ErrorMode::ThrowException);
			NazaraError("failed to create audio source");
		}
	}

	/*!
	* \brief Destructs the object
	*/
	SoundEmitter::~SoundEmitter() = default;

	/*!
	* \brief Enables spatialization
	*
	* \param spatialization True if spatialization is enabled
	*/
	void SoundEmitter::EnableSpatialization(bool spatialization)
	{
		m_source->EnableSpatialization(spatialization);
	}

	/*!
	* \brief Gets the attenuation
	* \return Amount that your sound will drop off as by the inverse square law 
	*/
	float SoundEmitter::GetAttenuation() const
	{
		return m_source->GetAttenuation();
	}

	/*!
	* \brief Gets the minimum distance to hear
	* \return Distance to begin to hear
	*/
	float SoundEmitter::GetMinDistance() const
	{
		return m_source->GetMinDistance();
	}

	/*!
	* \brief Gets the pitch
	* \return Pitch of the sound
	*/
	float SoundEmitter::GetPitch() const
	{
		return m_source->GetPitch();
	}

	/*!
	* \brief Gets the position of the emitter
	* \return Position of the sound
	*/
	Vector3f SoundEmitter::GetPosition() const
	{
		return m_source->GetPosition();
	}

	/*!
	* \brief Gets the velocity of the emitter
	* \return Velocity of the sound
	*/
	Vector3f SoundEmitter::GetVelocity() const
	{
		return m_source->GetVelocity();
	}

	/*!
	* \brief Gets the volume of the emitter
	* \param volume Float between [0, inf) with 100.f being the default
	*/
	float SoundEmitter::GetVolume() const
	{
		return m_source->GetVolume();
	}

	/*!
	* \brief Checks whether the sound emitter has spatialization enabled
	* \return true if it the case
	*/
	bool SoundEmitter::IsSpatializationEnabled() const
	{
		return m_source->IsSpatializationEnabled();
	}

	/*!
	* \brief Seek the sound to a point in time
	*
	* \param offset Time offset to seek
	*/
	void SoundEmitter::SeekToPlayingOffset(Time offset)
	{
		UInt64 microseconds = static_cast<UInt64>(std::max(offset.AsMicroseconds(), Int64(0)));
		SeekToSampleOffset(SafeCast<UInt32>(microseconds * GetSampleRate() / 1'000'000));
	}

	/*!
	* \brief Sets the attenuation
	*
	* \param attenuation Amount that your sound will drop off as by the inverse square law 
	*/
	void SoundEmitter::SetAttenuation(float attenuation)
	{
		m_source->SetAttenuation(attenuation);
	}

	/*!
	* \brief Sets the minimum distance to hear
	*
	* \param minDistance to begin to hear
	*/
	void SoundEmitter::SetMinDistance(float minDistance)
	{
		m_source->SetMinDistance(minDistance);
	}

	/*!
	* \brief Sets the pitch
	*
	* \param pitch of the sound
	*/
	void SoundEmitter::SetPitch(float pitch)
	{
		m_source->SetPitch(pitch);
	}

	/*!
	* \brief Sets the position of the emitter
	*
	* \param position Position of the sound
	*/
	void SoundEmitter::SetPosition(const Vector3f& position)
	{
		m_source->SetPosition(position);
	}

	/*!
	* \brief Sets the velocity of the emitter
	*
	* \param velocity Velocity of the sound
	*/
	void SoundEmitter::SetVelocity(const Vector3f& velocity)
	{
		m_source->SetVelocity(velocity);
	}

	/*!
	* \brief Sets the volume of the emitter
	*
	* \param volume Float between [0, inf) with 100.f being the default
	*/
	void SoundEmitter::SetVolume(float volume)
	{
		m_source->SetVolume(volume);
	}
}
