// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenAL.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace
	{
		DynLib s_library;
		String s_deviceName;
		String s_rendererName;
		String s_vendorName;
		ALCdevice* s_device = nullptr;
		ALCcontext* s_context = nullptr;
		unsigned int s_version;

		/*!
		* \brief Parses the devices
		* \return Number of devices
		*
		* \param deviceString String for the device (input / output)
		* \param devices List of names of the devices
		*/

		std::size_t ParseDevices(const char* deviceString, std::vector<String>& devices)
		{
			if (!deviceString)
				return 0;

			std::size_t startSize = devices.size();

			std::size_t length;
			while ((length = std::strlen(deviceString)) > 0)
			{
				devices.push_back(String(deviceString, length));
				deviceString += length + 1;
			}

			return devices.size() - startSize;
		}
	}

	/*!
	* \ingroup audio
	* \class Nz::OpenAL
	* \brief Audio class that represents the link with OpenAL 
	*
	* \remark This class is meant to be used by Module Audio
	*/

	/*!
	* \brief Gets the entry for the function name
	* \return Pointer to the function
	*
	* \param entryPoint Name of the entry
	*
	* \remark This does not produces a NazaraError if entry does not exist
	*/

	OpenALFunc OpenAL::GetEntry(const String& entryPoint)
	{
		return LoadEntry(entryPoint.GetConstBuffer(), false);
	}

	/*!
	* \brief Gets the name of the renderer
	* \return Name of the renderer
	*/

	String OpenAL::GetRendererName()
	{
		return s_rendererName;
	}

	/*!
	* \brief Gets the name of the vendor
	* \return Name of the vendor
	*/

	String OpenAL::GetVendorName()
	{
		return s_vendorName;
	}

	/*!
	* \brief Gets the version of OpenAL
	* \return Version of OpenAL
	*/

	unsigned int OpenAL::GetVersion()
	{
		return s_version;
	}

	/*!
	* \brief Initializes the module OpenAL
	* \return true if initialization is successful
	*
	* \param openDevice True to get information from the device
	*
	* \remark Produces a NazaraError if one of the entry failed
	* \remark Produces a NazaraError if opening device failed with openDevice parameter set to true
	*/

	bool OpenAL::Initialize(bool openDevice)
	{
		if (IsInitialized())
			return true;

		#if defined(NAZARA_PLATFORM_WINDOWS)
		///FIXME: Is OpenAL Soft a better implementation than Creative ?
		/// If we could use OpenAL Soft everytime, this would allow us to use sonorous extensions
		/// and give us more technical possibilities  with audio
		const char* libs[] = {
			"soft_oal.dll",
			"wrap_oal.dll",
			"openal32.dll"
		};
		#elif defined(NAZARA_PLATFORM_LINUX)
		const char* libs[] = {
			"libopenal.so.1",
			"libopenal.so.0",
			"libopenal.so"
		};
		//#elif defined(NAZARA_PLATFORM_MACOSX)
		#else
		NazaraError("Unknown OS");
		return false;
		#endif

		bool succeeded = false;
		for (const char* path : libs)
		{
			String libPath(path);
			if (!s_library.Load(libPath))
				continue;

			try
			{
				// al
				alBuffer3f = reinterpret_cast<OpenALDetail::LPALBUFFER3F>(LoadEntry("alBuffer3f"));
				alBuffer3i = reinterpret_cast<OpenALDetail::LPALBUFFER3I>(LoadEntry("alBuffer3i"));
				alBufferData = reinterpret_cast<OpenALDetail::LPALBUFFERDATA>(LoadEntry("alBufferData"));
				alBufferf = reinterpret_cast<OpenALDetail::LPALBUFFERF>(LoadEntry("alBufferf"));
				alBufferfv = reinterpret_cast<OpenALDetail::LPALBUFFERFV>(LoadEntry("alBufferfv"));
				alBufferi = reinterpret_cast<OpenALDetail::LPALBUFFERI>(LoadEntry("alBufferi"));
				alBufferiv = reinterpret_cast<OpenALDetail::LPALBUFFERIV>(LoadEntry("alBufferiv"));
				alDeleteBuffers = reinterpret_cast<OpenALDetail::LPALDELETEBUFFERS>(LoadEntry("alDeleteBuffers"));
				alDeleteSources = reinterpret_cast<OpenALDetail::LPALDELETESOURCES>(LoadEntry("alDeleteSources"));
				alDisable = reinterpret_cast<OpenALDetail::LPALDISABLE>(LoadEntry("alDisable"));
				alDistanceModel = reinterpret_cast<OpenALDetail::LPALDISTANCEMODEL>(LoadEntry("alDistanceModel"));
				alDopplerFactor = reinterpret_cast<OpenALDetail::LPALDOPPLERFACTOR>(LoadEntry("alDopplerFactor"));
				alDopplerVelocity = reinterpret_cast<OpenALDetail::LPALDOPPLERVELOCITY>(LoadEntry("alDopplerVelocity"));
				alEnable = reinterpret_cast<OpenALDetail::LPALENABLE>(LoadEntry("alEnable"));
				alGenBuffers = reinterpret_cast<OpenALDetail::LPALGENBUFFERS>(LoadEntry("alGenBuffers"));
				alGenSources = reinterpret_cast<OpenALDetail::LPALGENSOURCES>(LoadEntry("alGenSources"));
				alGetBoolean = reinterpret_cast<OpenALDetail::LPALGETBOOLEAN>(LoadEntry("alGetBoolean"));
				alGetBooleanv = reinterpret_cast<OpenALDetail::LPALGETBOOLEANV>(LoadEntry("alGetBooleanv"));
				alGetBuffer3f = reinterpret_cast<OpenALDetail::LPALGETBUFFER3F>(LoadEntry("alGetBuffer3f"));
				alGetBuffer3i = reinterpret_cast<OpenALDetail::LPALGETBUFFER3I>(LoadEntry("alGetBuffer3i"));
				alGetBufferf = reinterpret_cast<OpenALDetail::LPALGETBUFFERF>(LoadEntry("alGetBufferf"));
				alGetBufferfv = reinterpret_cast<OpenALDetail::LPALGETBUFFERFV>(LoadEntry("alGetBufferfv"));
				alGetBufferi = reinterpret_cast<OpenALDetail::LPALGETBUFFERI>(LoadEntry("alGetBufferi"));
				alGetBufferiv = reinterpret_cast<OpenALDetail::LPALGETBUFFERIV>(LoadEntry("alGetBufferiv"));
				alGetDouble = reinterpret_cast<OpenALDetail::LPALGETDOUBLE>(LoadEntry("alGetDouble"));
				alGetDoublev = reinterpret_cast<OpenALDetail::LPALGETDOUBLEV>(LoadEntry("alGetDoublev"));
				alGetEnumValue = reinterpret_cast<OpenALDetail::LPALGETENUMVALUE>(LoadEntry("alGetEnumValue"));
				alGetError = reinterpret_cast<OpenALDetail::LPALGETERROR>(LoadEntry("alGetError"));
				alGetFloat = reinterpret_cast<OpenALDetail::LPALGETFLOAT>(LoadEntry("alGetFloat"));
				alGetFloatv = reinterpret_cast<OpenALDetail::LPALGETFLOATV>(LoadEntry("alGetFloatv"));
				alGetInteger = reinterpret_cast<OpenALDetail::LPALGETINTEGER>(LoadEntry("alGetInteger"));
				alGetIntegerv = reinterpret_cast<OpenALDetail::LPALGETINTEGERV>(LoadEntry("alGetIntegerv"));
				alGetListener3f = reinterpret_cast<OpenALDetail::LPALGETLISTENER3F>(LoadEntry("alGetListener3f"));
				alGetListener3i = reinterpret_cast<OpenALDetail::LPALGETLISTENER3I>(LoadEntry("alGetListener3i"));
				alGetListenerf = reinterpret_cast<OpenALDetail::LPALGETLISTENERF>(LoadEntry("alGetListenerf"));
				alGetListenerfv = reinterpret_cast<OpenALDetail::LPALGETLISTENERFV>(LoadEntry("alGetListenerfv"));
				alGetListeneri = reinterpret_cast<OpenALDetail::LPALGETLISTENERI>(LoadEntry("alGetListeneri"));
				alGetListeneriv = reinterpret_cast<OpenALDetail::LPALGETLISTENERIV>(LoadEntry("alGetListeneriv"));
				alGetProcAddress = reinterpret_cast<OpenALDetail::LPALGETPROCADDRESS>(LoadEntry("alGetProcAddress"));
				alGetSource3f = reinterpret_cast<OpenALDetail::LPALGETSOURCE3F>(LoadEntry("alGetSource3f"));
				alGetSource3i = reinterpret_cast<OpenALDetail::LPALGETSOURCE3I>(LoadEntry("alGetSource3i"));
				alGetSourcef = reinterpret_cast<OpenALDetail::LPALGETSOURCEF>(LoadEntry("alGetSourcef"));
				alGetSourcefv = reinterpret_cast<OpenALDetail::LPALGETSOURCEFV>(LoadEntry("alGetSourcefv"));
				alGetSourcei = reinterpret_cast<OpenALDetail::LPALGETSOURCEI>(LoadEntry("alGetSourcei"));
				alGetSourceiv = reinterpret_cast<OpenALDetail::LPALGETSOURCEIV>(LoadEntry("alGetSourceiv"));
				alGetString = reinterpret_cast<OpenALDetail::LPALGETSTRING>(LoadEntry("alGetString"));
				alIsBuffer = reinterpret_cast<OpenALDetail::LPALISBUFFER>(LoadEntry("alIsBuffer"));
				alIsEnabled = reinterpret_cast<OpenALDetail::LPALISENABLED>(LoadEntry("alIsEnabled"));
				alIsExtensionPresent = reinterpret_cast<OpenALDetail::LPALISEXTENSIONPRESENT>(LoadEntry("alIsExtensionPresent"));
				alIsSource = reinterpret_cast<OpenALDetail::LPALISSOURCE>(LoadEntry("alIsSource"));
				alListener3f = reinterpret_cast<OpenALDetail::LPALLISTENER3F>(LoadEntry("alListener3f"));
				alListener3i = reinterpret_cast<OpenALDetail::LPALLISTENER3I>(LoadEntry("alListener3i"));
				alListenerf = reinterpret_cast<OpenALDetail::LPALLISTENERF>(LoadEntry("alListenerf"));
				alListenerfv = reinterpret_cast<OpenALDetail::LPALLISTENERFV>(LoadEntry("alListenerfv"));
				alListeneri = reinterpret_cast<OpenALDetail::LPALLISTENERI>(LoadEntry("alListeneri"));
				alListeneriv = reinterpret_cast<OpenALDetail::LPALLISTENERIV>(LoadEntry("alListeneriv"));
				alSource3f = reinterpret_cast<OpenALDetail::LPALSOURCE3F>(LoadEntry("alSource3f"));
				alSource3i = reinterpret_cast<OpenALDetail::LPALSOURCE3I>(LoadEntry("alSource3i"));
				alSourcef = reinterpret_cast<OpenALDetail::LPALSOURCEF>(LoadEntry("alSourcef"));
				alSourcefv = reinterpret_cast<OpenALDetail::LPALSOURCEFV>(LoadEntry("alSourcefv"));
				alSourcei = reinterpret_cast<OpenALDetail::LPALSOURCEI>(LoadEntry("alSourcei"));
				alSourceiv = reinterpret_cast<OpenALDetail::LPALSOURCEIV>(LoadEntry("alSourceiv"));
				alSourcePause = reinterpret_cast<OpenALDetail::LPALSOURCEPAUSE>(LoadEntry("alSourcePause"));
				alSourcePausev = reinterpret_cast<OpenALDetail::LPALSOURCEPAUSEV>(LoadEntry("alSourcePausev"));
				alSourcePlay = reinterpret_cast<OpenALDetail::LPALSOURCEPLAY>(LoadEntry("alSourcePlay"));
				alSourcePlayv = reinterpret_cast<OpenALDetail::LPALSOURCEPLAYV>(LoadEntry("alSourcePlayv"));
				alSourceQueueBuffers = reinterpret_cast<OpenALDetail::LPALSOURCEQUEUEBUFFERS>(LoadEntry("alSourceQueueBuffers"));
				alSourceRewind = reinterpret_cast<OpenALDetail::LPALSOURCEREWIND>(LoadEntry("alSourceRewind"));
				alSourceRewindv = reinterpret_cast<OpenALDetail::LPALSOURCEREWINDV>(LoadEntry("alSourceRewindv"));
				alSourceStop = reinterpret_cast<OpenALDetail::LPALSOURCESTOP>(LoadEntry("alSourceStop"));
				alSourceStopv = reinterpret_cast<OpenALDetail::LPALSOURCESTOPV>(LoadEntry("alSourceStopv"));
				alSourceUnqueueBuffers = reinterpret_cast<OpenALDetail::LPALSOURCEUNQUEUEBUFFERS>(LoadEntry("alSourceUnqueueBuffers"));
				alSpeedOfSound = reinterpret_cast<OpenALDetail::LPALSPEEDOFSOUND>(LoadEntry("alSpeedOfSound"));

				// alc
				alcCaptureCloseDevice = reinterpret_cast<OpenALDetail::LPALCCAPTURECLOSEDEVICE>(LoadEntry("alcCaptureCloseDevice"));
				alcCaptureOpenDevice = reinterpret_cast<OpenALDetail::LPALCCAPTUREOPENDEVICE>(LoadEntry("alcCaptureOpenDevice"));
				alcCaptureSamples = reinterpret_cast<OpenALDetail::LPALCCAPTURESAMPLES>(LoadEntry("alcCaptureSamples"));
				alcCaptureStart = reinterpret_cast<OpenALDetail::LPALCCAPTURESTART>(LoadEntry("alcCaptureStart"));
				alcCaptureStop = reinterpret_cast<OpenALDetail::LPALCCAPTURESTOP>(LoadEntry("alcCaptureStop"));
				alcCloseDevice = reinterpret_cast<OpenALDetail::LPALCCLOSEDEVICE>(LoadEntry("alcCloseDevice"));
				alcCreateContext = reinterpret_cast<OpenALDetail::LPALCCREATECONTEXT>(LoadEntry("alcCreateContext"));
				alcDestroyContext = reinterpret_cast<OpenALDetail::LPALCDESTROYCONTEXT>(LoadEntry("alcDestroyContext"));
				alcGetContextsDevice = reinterpret_cast<OpenALDetail::LPALCGETCONTEXTSDEVICE>(LoadEntry("alcGetContextsDevice"));
				alcGetCurrentContext = reinterpret_cast<OpenALDetail::LPALCGETCURRENTCONTEXT>(LoadEntry("alcGetCurrentContext"));
				alcGetEnumValue = reinterpret_cast<OpenALDetail::LPALCGETENUMVALUE>(LoadEntry("alcGetEnumValue"));
				alcGetError = reinterpret_cast<OpenALDetail::LPALCGETERROR>(LoadEntry("alcGetError"));
				alcGetIntegerv = reinterpret_cast<OpenALDetail::LPALCGETINTEGERV>(LoadEntry("alcGetIntegerv"));
				alcGetProcAddress = reinterpret_cast<OpenALDetail::LPALCGETPROCADDRESS>(LoadEntry("alcGetProcAddress"));
				alcGetString = reinterpret_cast<OpenALDetail::LPALCGETSTRING>(LoadEntry("alcGetString"));
				alcIsExtensionPresent = reinterpret_cast<OpenALDetail::LPALCISEXTENSIONPRESENT>(LoadEntry("alcIsExtensionPresent"));
				alcMakeContextCurrent = reinterpret_cast<OpenALDetail::LPALCMAKECONTEXTCURRENT>(LoadEntry("alcMakeContextCurrent"));
				alcOpenDevice = reinterpret_cast<OpenALDetail::LPALCOPENDEVICE>(LoadEntry("alcOpenDevice"));
				alcProcessContext = reinterpret_cast<OpenALDetail::LPALCPROCESSCONTEXT>(LoadEntry("alcProcessContext"));
				alcSuspendContext = reinterpret_cast<OpenALDetail::LPALCSUSPENDCONTEXT>(LoadEntry("alcSuspendContext"));

				succeeded = true;
				break;
			}
			catch (const std::exception& e)
			{
				NazaraWarning(libPath + " loading failed: " + String(e.what()));
				continue;
			}
		}

		if (!succeeded)
		{
			NazaraError("Failed to load OpenAL");
			Uninitialize();

			return false;
		}

		if (openDevice)
			OpenDevice();

		return true;
	}

	/*!
	* \brief Checks whether the module is initialized
	* \return true if it is the case
	*/

	bool OpenAL::IsInitialized()
	{
		return s_library.IsLoaded();
	}

	/*!
	* \brief Queries the input devices
	* \return Number of devices
	*
	* \param devices List of names of the input devices
	*/

	std::size_t OpenAL::QueryInputDevices(std::vector<String>& devices)
	{
		const char* deviceString = reinterpret_cast<const char*>(alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
		if (!deviceString)
			return 0;

		return ParseDevices(deviceString, devices);
	}

	/*!
	* \brief Queries the output devices
	* \return Number of devices
	*
	* \param devices List of names of the output devices
	*/

	std::size_t OpenAL::QueryOutputDevices(std::vector<String>& devices)
	{
		const char* deviceString = reinterpret_cast<const char*>(alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));
		if (!deviceString)
			return 0;

		return ParseDevices(deviceString, devices);
	}

	/*!
	* \brief Sets the active device
	* \return true if device is successfully opened
	*
	* \param deviceName Name of the device
	*/

	bool OpenAL::SetDevice(const String& deviceName)
	{
		s_deviceName = deviceName;
		if (IsInitialized())
		{
			CloseDevice();

			return OpenDevice();
		}
		else
			return true;
	}

	/*!
	* \brief Uninitializes the module
	*/

	void OpenAL::Uninitialize()
	{
		CloseDevice();

		s_rendererName.Clear(false);
		s_vendorName.Clear(false);
		s_library.Unload();
	}

	///WARNING: The integer value is the number of canals owned by the format
	ALenum OpenAL::AudioFormat[AudioFormat_Max+1] = {0}; // Added values with loading of OpenAL

	/*!
	* \brief Closes the device
	*
	* \remark Produces a NazaraWarning if you try to close an active device
	*/

	void OpenAL::CloseDevice()
	{
		if (s_device)
		{
			if (s_context)
			{
				alcMakeContextCurrent(nullptr);
				alcDestroyContext(s_context);
				s_context = nullptr;
			}

			if (!alcCloseDevice(s_device))
				// We could not close the close, this means that it's still in use
				NazaraWarning("Failed to close device");

			s_device = nullptr;
		}
	}

	/*!
	* \brief Opens the device
	* \return true if open is successful
	*
	* \remark Produces a NazaraError if it could not create the context
	*/

	bool OpenAL::OpenDevice()
	{
		// Initialisation of the module
		s_device = alcOpenDevice(s_deviceName.IsEmpty() ? nullptr : s_deviceName.GetConstBuffer()); // We choose the default device
		if (!s_device)
		{
			NazaraError("Failed to open default device");
			return false;
		}

		// One context is enough
		s_context = alcCreateContext(s_device, nullptr);
		if (!s_context)
		{
			NazaraError("Failed to create context");
			return false;
		}

		if (!alcMakeContextCurrent(s_context))
		{
			NazaraError("Failed to activate context");
			return false;
		}

		s_rendererName = reinterpret_cast<const char*>(alGetString(AL_RENDERER));
		s_vendorName = reinterpret_cast<const char*>(alGetString(AL_VENDOR));

		const ALchar* version = alGetString(AL_VERSION);
		if (version)
		{
			unsigned int major = version[0] - '0';
			unsigned int minor = version[2] - '0';

			if (major != 0 && major <= 9)
			{
				if (minor > 9)
				{
					NazaraWarning("Unable to retrieve OpenAL minor version (using 0)");
					minor = 0;
				}

				s_version = major*100 + minor*10;

				NazaraDebug("OpenAL version: " + String::Number(major) + '.' + String::Number(minor));
			}
			else
			{
				NazaraDebug("Unable to retrieve OpenAL major version");
				s_version = 0;
			}
		}
		else
		{
			NazaraDebug("Unable to retrieve OpenAL version");
			s_version = 0;
		}

		// We complete the formats table
		AudioFormat[AudioFormat_Mono] = AL_FORMAT_MONO16;
		AudioFormat[AudioFormat_Stereo] = AL_FORMAT_STEREO16;

		// "The presence of an enum value does not guarantee the applicability of an extension to the current context."
		if (alIsExtensionPresent("AL_EXT_MCFORMATS"))
		{
			AudioFormat[AudioFormat_Quad] = alGetEnumValue("AL_FORMAT_QUAD16");
			AudioFormat[AudioFormat_5_1]  = alGetEnumValue("AL_FORMAT_51CHN16");
			AudioFormat[AudioFormat_6_1]  = alGetEnumValue("AL_FORMAT_61CHN16");
			AudioFormat[AudioFormat_7_1]  = alGetEnumValue("AL_FORMAT_71CHN16");
		}
		else if (alIsExtensionPresent("AL_LOKI_quadriphonic"))
			AudioFormat[AudioFormat_Quad] = alGetEnumValue("AL_FORMAT_QUAD16_LOKI");

		return true;
	}

	/*!
	* \brief Loads the entry for the function name
	* \return Pointer to the function
	*
	* \param name Name of the entry
	* \param throwException Should throw exception if failed ?
	*
	* \remark Produces a std::runtime_error if entry does not exist and throwException is set to true
	*/

	OpenALFunc OpenAL::LoadEntry(const char* name, bool throwException)
	{
		OpenALFunc entry = reinterpret_cast<OpenALFunc>(s_library.GetSymbol(name));
		if (!entry && throwException)
		{
			std::ostringstream oss;
			oss << "failed to load \"" << name << '"';

			throw std::runtime_error(oss.str());
		}

		return entry;
	}

// al
OpenALDetail::LPALBUFFER3F             alBuffer3f             = nullptr;
OpenALDetail::LPALBUFFER3I             alBuffer3i             = nullptr;
OpenALDetail::LPALBUFFERDATA           alBufferData           = nullptr;
OpenALDetail::LPALBUFFERF              alBufferf              = nullptr;
OpenALDetail::LPALBUFFERFV             alBufferfv             = nullptr;
OpenALDetail::LPALBUFFERI              alBufferi              = nullptr;
OpenALDetail::LPALBUFFERIV             alBufferiv             = nullptr;
OpenALDetail::LPALDELETEBUFFERS        alDeleteBuffers        = nullptr;
OpenALDetail::LPALDELETESOURCES        alDeleteSources        = nullptr;
OpenALDetail::LPALDISABLE              alDisable              = nullptr;
OpenALDetail::LPALDISTANCEMODEL        alDistanceModel        = nullptr;
OpenALDetail::LPALDOPPLERFACTOR        alDopplerFactor        = nullptr;
OpenALDetail::LPALDOPPLERVELOCITY      alDopplerVelocity      = nullptr;
OpenALDetail::LPALENABLE               alEnable               = nullptr;
OpenALDetail::LPALGENBUFFERS           alGenBuffers           = nullptr;
OpenALDetail::LPALGENSOURCES           alGenSources           = nullptr;
OpenALDetail::LPALGETBOOLEAN           alGetBoolean           = nullptr;
OpenALDetail::LPALGETBOOLEANV          alGetBooleanv          = nullptr;
OpenALDetail::LPALGETBUFFER3F          alGetBuffer3f          = nullptr;
OpenALDetail::LPALGETBUFFER3I          alGetBuffer3i          = nullptr;
OpenALDetail::LPALGETBUFFERF           alGetBufferf           = nullptr;
OpenALDetail::LPALGETBUFFERFV          alGetBufferfv          = nullptr;
OpenALDetail::LPALGETBUFFERI           alGetBufferi           = nullptr;
OpenALDetail::LPALGETBUFFERIV          alGetBufferiv          = nullptr;
OpenALDetail::LPALGETDOUBLE            alGetDouble            = nullptr;
OpenALDetail::LPALGETDOUBLEV           alGetDoublev           = nullptr;
OpenALDetail::LPALGETENUMVALUE         alGetEnumValue         = nullptr;
OpenALDetail::LPALGETERROR             alGetError             = nullptr;
OpenALDetail::LPALGETFLOAT             alGetFloat             = nullptr;
OpenALDetail::LPALGETFLOATV            alGetFloatv            = nullptr;
OpenALDetail::LPALGETINTEGER           alGetInteger           = nullptr;
OpenALDetail::LPALGETINTEGERV          alGetIntegerv          = nullptr;
OpenALDetail::LPALGETLISTENER3F        alGetListener3f        = nullptr;
OpenALDetail::LPALGETLISTENER3I        alGetListener3i        = nullptr;
OpenALDetail::LPALGETLISTENERF         alGetListenerf         = nullptr;
OpenALDetail::LPALGETLISTENERFV        alGetListenerfv        = nullptr;
OpenALDetail::LPALGETLISTENERI         alGetListeneri         = nullptr;
OpenALDetail::LPALGETLISTENERIV        alGetListeneriv        = nullptr;
OpenALDetail::LPALGETPROCADDRESS       alGetProcAddress       = nullptr;
OpenALDetail::LPALGETSOURCE3F          alGetSource3f          = nullptr;
OpenALDetail::LPALGETSOURCE3I          alGetSource3i          = nullptr;
OpenALDetail::LPALGETSOURCEF           alGetSourcef           = nullptr;
OpenALDetail::LPALGETSOURCEFV          alGetSourcefv          = nullptr;
OpenALDetail::LPALGETSOURCEI           alGetSourcei           = nullptr;
OpenALDetail::LPALGETSOURCEIV          alGetSourceiv          = nullptr;
OpenALDetail::LPALGETSTRING            alGetString            = nullptr;
OpenALDetail::LPALISBUFFER             alIsBuffer             = nullptr;
OpenALDetail::LPALISENABLED            alIsEnabled            = nullptr;
OpenALDetail::LPALISEXTENSIONPRESENT   alIsExtensionPresent   = nullptr;
OpenALDetail::LPALISSOURCE             alIsSource             = nullptr;
OpenALDetail::LPALLISTENER3F           alListener3f           = nullptr;
OpenALDetail::LPALLISTENER3I           alListener3i           = nullptr;
OpenALDetail::LPALLISTENERF            alListenerf            = nullptr;
OpenALDetail::LPALLISTENERFV           alListenerfv           = nullptr;
OpenALDetail::LPALLISTENERI            alListeneri            = nullptr;
OpenALDetail::LPALLISTENERIV           alListeneriv           = nullptr;
OpenALDetail::LPALSOURCE3F             alSource3f             = nullptr;
OpenALDetail::LPALSOURCE3I             alSource3i             = nullptr;
OpenALDetail::LPALSOURCEF              alSourcef              = nullptr;
OpenALDetail::LPALSOURCEFV             alSourcefv             = nullptr;
OpenALDetail::LPALSOURCEI              alSourcei              = nullptr;
OpenALDetail::LPALSOURCEIV             alSourceiv             = nullptr;
OpenALDetail::LPALSOURCEPAUSE          alSourcePause          = nullptr;
OpenALDetail::LPALSOURCEPAUSEV         alSourcePausev         = nullptr;
OpenALDetail::LPALSOURCEPLAY           alSourcePlay           = nullptr;
OpenALDetail::LPALSOURCEPLAYV          alSourcePlayv          = nullptr;
OpenALDetail::LPALSOURCEQUEUEBUFFERS   alSourceQueueBuffers   = nullptr;
OpenALDetail::LPALSOURCEREWIND         alSourceRewind         = nullptr;
OpenALDetail::LPALSOURCEREWINDV        alSourceRewindv        = nullptr;
OpenALDetail::LPALSOURCESTOP           alSourceStop           = nullptr;
OpenALDetail::LPALSOURCESTOPV          alSourceStopv          = nullptr;
OpenALDetail::LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers = nullptr;
OpenALDetail::LPALSPEEDOFSOUND         alSpeedOfSound         = nullptr;

// alc
OpenALDetail::LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice = nullptr;
OpenALDetail::LPALCCAPTUREOPENDEVICE  alcCaptureOpenDevice  = nullptr;
OpenALDetail::LPALCCAPTURESAMPLES     alcCaptureSamples     = nullptr;
OpenALDetail::LPALCCAPTURESTART       alcCaptureStart       = nullptr;
OpenALDetail::LPALCCAPTURESTOP        alcCaptureStop        = nullptr;
OpenALDetail::LPALCCLOSEDEVICE        alcCloseDevice        = nullptr;
OpenALDetail::LPALCCREATECONTEXT      alcCreateContext      = nullptr;
OpenALDetail::LPALCDESTROYCONTEXT     alcDestroyContext     = nullptr;
OpenALDetail::LPALCGETCONTEXTSDEVICE  alcGetContextsDevice  = nullptr;
OpenALDetail::LPALCGETCURRENTCONTEXT  alcGetCurrentContext  = nullptr;
OpenALDetail::LPALCGETENUMVALUE       alcGetEnumValue       = nullptr;
OpenALDetail::LPALCGETERROR           alcGetError           = nullptr;
OpenALDetail::LPALCGETINTEGERV        alcGetIntegerv        = nullptr;
OpenALDetail::LPALCGETPROCADDRESS     alcGetProcAddress     = nullptr;
OpenALDetail::LPALCGETSTRING          alcGetString          = nullptr;
OpenALDetail::LPALCISEXTENSIONPRESENT alcIsExtensionPresent = nullptr;
OpenALDetail::LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = nullptr;
OpenALDetail::LPALCOPENDEVICE         alcOpenDevice         = nullptr;
OpenALDetail::LPALCPROCESSCONTEXT     alcProcessContext     = nullptr;
OpenALDetail::LPALCSUSPENDCONTEXT     alcSuspendContext     = nullptr;

}
