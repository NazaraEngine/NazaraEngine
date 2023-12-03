// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Audio/OpenALBuffer.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Audio/OpenALSource.hpp>
#include <Nazara/Audio/OpenALUtils.hpp>
#include <Nazara/Core/Log.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <array>
#include <cstring>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr std::array s_functionNames = {
#define NAZARA_AUDIO_AL_ALC_FUNCTION(name) #name,
#include <Nazara/Audio/OpenALFunctions.hpp>
		};

		thread_local const OpenALDevice* s_currentALDevice;

		template<typename FuncType, std::size_t FuncIndex, typename>
		struct ALWrapper;

		template<typename FuncType, std::size_t FuncIndex, typename Ret, typename... Args>
		struct ALWrapper<FuncType, FuncIndex, Ret(AL_APIENTRY*)(Args...)>
		{
			static auto WrapErrorHandling()
			{
				return [](Args... args) -> Ret
				{
					const OpenALDevice* device = s_currentALDevice; //< pay TLS cost once
					assert(device);

					FuncType funcPtr = reinterpret_cast<FuncType>(device->GetFunctionByIndex(FuncIndex));

					if constexpr (std::is_same_v<Ret, void>)
					{
						funcPtr(args...);

						if (device->ProcessErrorFlag())
							device->PrintFunctionCall(FuncIndex, args...);
					}
					else
					{
						Ret r = funcPtr(args...);

						if (device->ProcessErrorFlag())
							device->PrintFunctionCall(FuncIndex, args...);

						return r;
					}
				};
			}
		};
	}

	struct OpenALDevice::SymbolLoader
	{
		SymbolLoader(OpenALDevice& parent) :
		device(parent)
		{
		}

		template<typename FuncType, std::size_t FuncIndex, bool ContextFunction, typename Func>
		bool Load(Func& func, const char* funcName, FuncType libraryPtr)
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			ALFunction originalFuncPtr;
			if constexpr (ContextFunction)
				originalFuncPtr = BitCast<ALFunction>(device.m_library.alcGetProcAddress(device.m_device, funcName));
			else
				originalFuncPtr = BitCast<ALFunction>(device.m_library.alGetProcAddress(funcName));

			// Fallback in case of faulty OpenAL implementations not returning core functions through alGetProcAddress/alcGetProcAddress
			if (!originalFuncPtr)
				originalFuncPtr = reinterpret_cast<ALFunction>(libraryPtr);

			func = reinterpret_cast<FuncType>(originalFuncPtr);

			if (func && wrapErrorHandling)
			{
				if constexpr (
					FuncIndex != UnderlyingCast(FunctionIndex::alGetError) &&      //< Prevent infinite recursion
					FuncIndex != UnderlyingCast(FunctionIndex::alcCloseDevice) &&  //< alcDestroyContext is called with no context
					FuncIndex != UnderlyingCast(FunctionIndex::alcDestroyContext)) //< alcDestroyContext is called with no context
				{
					using Wrapper = ALWrapper<FuncType, FuncIndex, FuncType>;
					func = Wrapper::WrapErrorHandling();
				}
			}

			device.m_originalFunctionPointer[FuncIndex] = originalFuncPtr;

			return func != nullptr;
		}

		OpenALDevice& device;
		bool wrapErrorHandling = false;
	};


	OpenALDevice::OpenALDevice(OpenALLibrary& library, ALCdevice* device) :
	m_library(library),
	m_device(device)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		m_context = m_library.alcCreateContext(device, nullptr);
		if (!m_context)
			throw std::runtime_error("failed to create OpenAL context");

		// Don't use MakeContextCurrent as device pointers are not loaded yet
		if (m_library.alcMakeContextCurrent(m_context) != AL_TRUE)
			throw std::runtime_error("failed to activate OpenAL context");

		s_currentALDevice = this;

		SymbolLoader loader(*this);
#ifdef NAZARA_DEBUG
		loader.wrapErrorHandling = true;
#endif

#define NAZARA_AUDIO_AL_FUNCTION(name) loader.Load<decltype(&::name), UnderlyingCast(FunctionIndex:: name), false>(name, #name, library.name);
#define NAZARA_AUDIO_ALC_FUNCTION(name) loader.Load<decltype(&::name), UnderlyingCast(FunctionIndex:: name), true>(name, #name, library.name);
#define NAZARA_AUDIO_AL_EXT_FUNCTION(name) loader.Load<decltype(&::name), UnderlyingCast(FunctionIndex:: name), false>(name, #name, nullptr);
#include <Nazara/Audio/OpenALFunctions.hpp>

		m_renderer = reinterpret_cast<const char*>(alGetString(AL_RENDERER));
		m_vendor = reinterpret_cast<const char*>(alGetString(AL_VENDOR));

		// We complete the formats table
		m_audioFormatValues.fill(0);

		m_audioFormatValues[AudioFormat::I16_Mono] = AL_FORMAT_MONO16;
		m_audioFormatValues[AudioFormat::I16_Stereo] = AL_FORMAT_STEREO16;

		// "The presence of an enum value does not guarantee the applicability of an extension to the current context."
		if (library.alIsExtensionPresent("AL_EXT_MCFORMATS"))
		{
			m_audioFormatValues[AudioFormat::I16_Quad] = alGetEnumValue("AL_FORMAT_QUAD16");
			m_audioFormatValues[AudioFormat::I16_5_1] = alGetEnumValue("AL_FORMAT_51CHN16");
			m_audioFormatValues[AudioFormat::I16_6_1] = alGetEnumValue("AL_FORMAT_61CHN16");
			m_audioFormatValues[AudioFormat::I16_7_1] = alGetEnumValue("AL_FORMAT_71CHN16");
		}
		else if (library.alIsExtensionPresent("AL_LOKI_quadriphonic"))
			m_audioFormatValues[AudioFormat::I16_Quad] = alGetEnumValue("AL_FORMAT_QUAD16_LOKI");

		m_extensionStatus.fill(false);
		if (library.alIsExtensionPresent("AL_SOFT_source_latency"))
			m_extensionStatus[OpenALExtension::SourceLatency] = true;

		SetListenerDirection(Vector3f::Forward());
	}

	OpenALDevice::~OpenALDevice()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		MakeContextCurrent();

		alcDestroyContext(m_context);
		alcCloseDevice(m_device);

		if (s_currentALDevice == this)
			s_currentALDevice = nullptr;
	}

	bool OpenALDevice::ClearErrorFlag() const
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		assert(s_currentALDevice == this);

		alGetError();

		m_didCollectErrors = false;
		m_hadAnyError = false;

		return true;
	}

	std::shared_ptr<AudioBuffer> OpenALDevice::CreateBuffer()
	{
		MakeContextCurrent();

		ClearErrorFlag();

		ALuint bufferId = 0;
		alGenBuffers(1, &bufferId);

		if (!DidLastCallSucceed())
		{
			NazaraError("failed to create OpenAL buffer");
			return {};
		}

		return std::make_shared<OpenALBuffer>(shared_from_this(), bufferId);
	}

	std::shared_ptr<AudioSource> OpenALDevice::CreateSource()
	{
		MakeContextCurrent();

		ClearErrorFlag();

		ALuint sourceId = 0;
		alGenSources(1, &sourceId);

		if (!DidLastCallSucceed())
		{
			NazaraError("failed to create OpenAL buffer");
			return {};
		}

		return std::make_shared<OpenALSource>(shared_from_this(), sourceId);
	}

	/*!
	* \brief Gets the factor of the Doppler effect
	* \return Global factor of the Doppler effect
	*/
	float OpenALDevice::GetDopplerFactor() const
	{
		MakeContextCurrent();

		return alGetFloat(AL_DOPPLER_FACTOR);
	}

	/*!
	* \brief Gets the global volume
	* \return Float between [0, inf) with 1.f being the default
	*/
	float OpenALDevice::GetGlobalVolume() const
	{
		MakeContextCurrent();

		ALfloat gain = 0.f;
		alGetListenerf(AL_GAIN, &gain);

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
		alGetListenerfv(AL_ORIENTATION, orientation);

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
		alGetListenerfv(AL_POSITION, &position.x);

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
		alGetListenerfv(AL_ORIENTATION, orientation);

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
		alGetListenerfv(AL_VELOCITY, &velocity.x);

		return velocity;
	}

	void OpenALDevice::MakeContextCurrent() const
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (s_currentALDevice != this)
		{
			m_library.alcMakeContextCurrent(m_context);
			s_currentALDevice = this;
		}
	}

	template<typename... Args>
	void OpenALDevice::PrintFunctionCall(std::size_t funcIndex, Args... args) const
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::stringstream ss;
		ss << s_functionNames[funcIndex] << "(";
		if constexpr (sizeof...(args) > 0)
		{
			bool first = true;
			auto PrintParam = [&](auto value)
			{
				if (!first)
					ss << ", ";

				ss << +value;
				first = false;
			};

			(PrintParam(args), ...);
		}
		ss << ")";
		NazaraDebug(ss.str());
	}

	bool OpenALDevice::ProcessErrorFlag() const
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		assert(s_currentALDevice == this);

		bool hasAnyError = false;

		if (ALuint lastError = alGetError(); lastError != AL_NO_ERROR)
		{
			hasAnyError = true;

			NazaraErrorFmt("OpenAL error: {0}", TranslateOpenALError(lastError));
		}

		m_didCollectErrors = true;
		m_hadAnyError = hasAnyError;

		return hasAnyError;
	}

	/*!
	* \brief Gets the speed of sound
	* \return Speed of sound
	*/
	float OpenALDevice::GetSpeedOfSound() const
	{
		MakeContextCurrent();

		return alGetFloat(AL_SPEED_OF_SOUND);
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

		alDopplerFactor(dopplerFactor);
	}

	/*!
	* \brief Sets the global volume
	*
	* \param volume Float between [0, inf) with 1.f being the default
	*/
	void OpenALDevice::SetGlobalVolume(float volume)
	{
		MakeContextCurrent();

		alListenerf(AL_GAIN, volume);
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

		alListenerfv(AL_ORIENTATION, orientation);
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

		alListenerfv(AL_POSITION, &position.x);
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

		alListenerfv(AL_VELOCITY, &velocity.x);
	}

	/*!
	* \brief Sets the speed of sound
	*
	* \param speed Speed of sound
	*/
	void OpenALDevice::SetSpeedOfSound(float speed)
	{
		MakeContextCurrent();

		alSpeedOfSound(speed);
	}
}
