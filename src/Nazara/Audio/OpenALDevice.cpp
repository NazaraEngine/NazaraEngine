// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Audio/OpenALBuffer.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Audio/OpenALSource.hpp>
#include <Nazara/Audio/OpenALUtils.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace
	{
		thread_local ALCcontext* s_currentContext;
	}

	OpenALDevice::OpenALDevice(OpenALLibrary& library, ALCdevice* device) :
	m_library(library),
	m_device(device)
	{
		m_context = m_library.alcCreateContext(device, nullptr);
		if (!m_context)
			throw std::runtime_error("failed to create OpenAL context");

		MakeContextCurrent();

		m_renderer = reinterpret_cast<const char*>(m_library.alGetString(AL_RENDERER));
		m_vendor = reinterpret_cast<const char*>(m_library.alGetString(AL_VENDOR));

		// We complete the formats table
		m_audioFormatValues.fill(0);

		m_audioFormatValues[AudioFormat::I16_Mono] = AL_FORMAT_MONO16;
		m_audioFormatValues[AudioFormat::I16_Stereo] = AL_FORMAT_STEREO16;

		// "The presence of an enum value does not guarantee the applicability of an extension to the current context."
		if (library.alIsExtensionPresent("AL_EXT_MCFORMATS"))
		{
			m_audioFormatValues[AudioFormat::I16_Quad] = m_library.alGetEnumValue("AL_FORMAT_QUAD16");
			m_audioFormatValues[AudioFormat::I16_5_1] = m_library.alGetEnumValue("AL_FORMAT_51CHN16");
			m_audioFormatValues[AudioFormat::I16_6_1] = m_library.alGetEnumValue("AL_FORMAT_61CHN16");
			m_audioFormatValues[AudioFormat::I16_7_1] = m_library.alGetEnumValue("AL_FORMAT_71CHN16");
		}
		else if (library.alIsExtensionPresent("AL_LOKI_quadriphonic"))
			m_audioFormatValues[AudioFormat::I16_Quad] = m_library.alGetEnumValue("AL_FORMAT_QUAD16_LOKI");

		m_extensionStatus.fill(false);
		if (library.alIsExtensionPresent("AL_SOFT_source_latency"))
			m_extensionStatus[OpenALExtension::SourceLatency] = true;

		SetListenerDirection(Vector3f::Forward());
	}

	OpenALDevice::~OpenALDevice()
	{
		MakeContextCurrent();

		m_library.alcDestroyContext(m_context);
		m_library.alcCloseDevice(m_device);

		if (s_currentContext == m_context)
			s_currentContext = nullptr;
	}

	std::shared_ptr<AudioBuffer> OpenALDevice::CreateBuffer()
	{
		MakeContextCurrent();

		m_library.alGetError(); // Clear error flags

		ALuint bufferId = 0;
		m_library.alGenBuffers(1, &bufferId);

		if (ALenum lastError = m_library.alGetError(); lastError != AL_NO_ERROR)
		{
			NazaraErrorFmt("failed to create OpenAL buffer: {0}", TranslateOpenALError(lastError));
			return {};
		}

		return std::make_shared<OpenALBuffer>(shared_from_this(), m_library, bufferId);
	}

	std::shared_ptr<AudioSource> OpenALDevice::CreateSource()
	{
		MakeContextCurrent();

		m_library.alGetError(); // Clear error flags

		ALuint sourceId = 0;
		m_library.alGenSources(1, &sourceId);

		if (ALenum lastError = m_library.alGetError(); lastError != AL_NO_ERROR)
		{
			NazaraErrorFmt("failed to create OpenAL source: {0}", TranslateOpenALError(lastError));
			return {};
		}

		return std::make_shared<OpenALSource>(shared_from_this(), m_library, sourceId);
	}

	/*!
	* \brief Gets the factor of the Doppler effect
	* \return Global factor of the Doppler effect
	*/
	float OpenALDevice::GetDopplerFactor() const
	{
		MakeContextCurrent();

		return m_library.alGetFloat(AL_DOPPLER_FACTOR);
	}

	/*!
	* \brief Gets the global volume
	* \return Float between [0, inf) with 1.f being the default
	*/
	float OpenALDevice::GetGlobalVolume() const
	{
		MakeContextCurrent();

		ALfloat gain = 0.f;
		m_library.alGetListenerf(AL_GAIN, &gain);

		return gain;
	}

	/*!
	* \brief Gets the direction of the listener
	* \return Direction of the listener, in front of the listener
	*
	* \param up Current up direction
	*
	* \see GetListenerRotation
	*/
	Vector3f OpenALDevice::GetListenerDirection(Vector3f* up) const
	{
		MakeContextCurrent();

		ALfloat orientation[6];
		m_library.alGetListenerfv(AL_ORIENTATION, orientation);

		if (up)
			(*up) = Vector3f(orientation[3], orientation[4], orientation[5]);

		return Vector3f(orientation[0], orientation[1], orientation[2]);
	}

	/*!
	* \brief Gets the position of the listener
	* \return Position of the listener
	*
	* \see GetListenerVelocity
	*/
	Vector3f OpenALDevice::GetListenerPosition() const
	{
		MakeContextCurrent();

		Vector3f position;
		m_library.alGetListenerfv(AL_POSITION, &position.x);

		return position;
	}

	/*!
	* \brief Gets the rotation of the listener
	* \return Rotation of the listener
	*
	* \param up Current up direction
	*
	* \see GetListenerDirection
	*/
	Quaternionf OpenALDevice::GetListenerRotation() const
	{
		MakeContextCurrent();

		ALfloat orientation[6];
		m_library.alGetListenerfv(AL_ORIENTATION, orientation);

		Vector3f forward(orientation[0], orientation[1], orientation[2]);
		Vector3f up(orientation[3], orientation[4], orientation[5]);

		return Quaternionf::LookAt(forward, up);
	}

	/*!
	* \brief Gets the velocity of the listener
	* \return Velocity of the listener
	*
	* \see GetListenerPosition
	*/
	Vector3f OpenALDevice::GetListenerVelocity() const
	{
		MakeContextCurrent();

		Vector3f velocity;
		m_library.alGetListenerfv(AL_VELOCITY, &velocity.x);

		return velocity;
	}

	void OpenALDevice::MakeContextCurrent() const
	{
		if (s_currentContext != m_context)
		{
			m_library.alcMakeContextCurrent(m_context);
			s_currentContext = m_context;
		}
	}

	/*!
	* \brief Gets the speed of sound
	* \return Speed of sound
	*/
	float OpenALDevice::GetSpeedOfSound() const
	{
		MakeContextCurrent();

		return m_library.alGetFloat(AL_SPEED_OF_SOUND);
	}

	const void* OpenALDevice::GetSubSystemIdentifier() const
	{
		return &m_library;
	}

	/*!
	* \brief Checks whether the format is supported by the engine
	* \return true if it is the case
	*
	* \param format Format to check
	*/
	bool OpenALDevice::IsFormatSupported(AudioFormat format) const
	{
		if (format == AudioFormat::Unknown)
			return false;

		return m_audioFormatValues[format] != 0;
	}

	/*!
	* \brief Sets the factor of the doppler effect
	*
	* \param dopplerFactor Global factor of the doppler effect
	*/
	void OpenALDevice::SetDopplerFactor(float dopplerFactor)
	{
		MakeContextCurrent();

		m_library.alDopplerFactor(dopplerFactor);
	}

	/*!
	* \brief Sets the global volume
	*
	* \param volume Float between [0, inf) with 1.f being the default
	*/
	void OpenALDevice::SetGlobalVolume(float volume)
	{
		MakeContextCurrent();

		m_library.alListenerf(AL_GAIN, volume);
	}

	/*!
	* \brief Sets the direction of the listener
	*
	* \param direction Direction of the listener, in front of the listener
	* \param up Up vector
	*
	* \see SetListenerDirection, SetListenerRotation
	*/
	void OpenALDevice::SetListenerDirection(const Vector3f& direction, const Vector3f& up)
	{
		MakeContextCurrent();

		ALfloat orientation[6] =
		{
			direction.x, direction.y, direction.z,
			up.x, up.y, up.z
		};

		m_library.alListenerfv(AL_ORIENTATION, orientation);
	}

	/*!
	* \brief Sets the position of the listener
	*
	* \param position Position of the listener
	*
	* \see SetListenerVelocity
	*/
	void OpenALDevice::SetListenerPosition(const Vector3f& position)
	{
		MakeContextCurrent();

		m_library.alListenerfv(AL_POSITION, &position.x);
	}

	/*!
	* \brief Sets the velocity of the listener
	*
	* \param velocity Velocity of the listener
	*
	* \see SetListenerPosition
	*/
	void OpenALDevice::SetListenerVelocity(const Vector3f& velocity)
	{
		MakeContextCurrent();

		m_library.alListenerfv(AL_VELOCITY, &velocity.x);
	}

	/*!
	* \brief Sets the speed of sound
	*
	* \param speed Speed of sound
	*/
	void OpenALDevice::SetSpeedOfSound(float speed)
	{
		MakeContextCurrent();

		m_library.alSpeedOfSound(speed);
	}
}
