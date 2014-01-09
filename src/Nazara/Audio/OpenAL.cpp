// Copyright (C) 2013 Jérôme Leclercq
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

namespace
{
	NzDynLib s_library;
	NzString s_deviceName;
	NzString s_rendererName;
	NzString s_vendorName;
	ALCdevice* s_device = nullptr;
	ALCcontext* s_context = nullptr;
	unsigned int s_version;

	unsigned int ParseDevices(const char* deviceString, std::vector<NzString>& devices)
	{
		if (!deviceString)
			return 0;

		unsigned int startSize = devices.size();

		unsigned int length;
		while ((length = std::strlen(deviceString)) > 0)
		{
			devices.push_back(NzString(deviceString, length));
			deviceString += length + 1;
		}

		return devices.size() - startSize;
	}
}

NzOpenALFunc NzOpenAL::GetEntry(const NzString& entryPoint)
{
	return LoadEntry(entryPoint.GetConstBuffer(), false);
}

NzString NzOpenAL::GetRendererName()
{
	return s_rendererName;
}

NzString NzOpenAL::GetVendorName()
{
	return s_vendorName;
}

unsigned int NzOpenAL::GetVersion()
{
	return s_version;
}

bool NzOpenAL::Initialize(bool openDevice)
{
	if (s_library.IsLoaded())
		return true;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	// OpenAL Soft est une meilleure implémentation que les redistribuables de Creative
	///TODO: Détecter le driver software de Creative et basculer sur OpenAL Soft si possible
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
		NzString libPath(path);
		if (!s_library.Load(libPath, false))
			continue;

		try
		{
			// al
			alBuffer3f = reinterpret_cast<NzOpenALDetail::LPALBUFFER3F>(LoadEntry("alBuffer3f"));
			alBuffer3i = reinterpret_cast<NzOpenALDetail::LPALBUFFER3I>(LoadEntry("alBuffer3i"));
			alBufferData = reinterpret_cast<NzOpenALDetail::LPALBUFFERDATA>(LoadEntry("alBufferData"));
			alBufferf = reinterpret_cast<NzOpenALDetail::LPALBUFFERF>(LoadEntry("alBufferf"));
			alBufferfv = reinterpret_cast<NzOpenALDetail::LPALBUFFERFV>(LoadEntry("alBufferfv"));
			alBufferi = reinterpret_cast<NzOpenALDetail::LPALBUFFERI>(LoadEntry("alBufferi"));
			alBufferiv = reinterpret_cast<NzOpenALDetail::LPALBUFFERIV>(LoadEntry("alBufferiv"));
			alDeleteBuffers = reinterpret_cast<NzOpenALDetail::LPALDELETEBUFFERS>(LoadEntry("alDeleteBuffers"));
			alDeleteSources = reinterpret_cast<NzOpenALDetail::LPALDELETESOURCES>(LoadEntry("alDeleteSources"));
			alDisable = reinterpret_cast<NzOpenALDetail::LPALDISABLE>(LoadEntry("alDisable"));
			alDistanceModel = reinterpret_cast<NzOpenALDetail::LPALDISTANCEMODEL>(LoadEntry("alDistanceModel"));
			alDopplerFactor = reinterpret_cast<NzOpenALDetail::LPALDOPPLERFACTOR>(LoadEntry("alDopplerFactor"));
			alDopplerVelocity = reinterpret_cast<NzOpenALDetail::LPALDOPPLERVELOCITY>(LoadEntry("alDopplerVelocity"));
			alEnable = reinterpret_cast<NzOpenALDetail::LPALENABLE>(LoadEntry("alEnable"));
			alGenBuffers = reinterpret_cast<NzOpenALDetail::LPALGENBUFFERS>(LoadEntry("alGenBuffers"));
			alGenSources = reinterpret_cast<NzOpenALDetail::LPALGENSOURCES>(LoadEntry("alGenSources"));
			alGetBoolean = reinterpret_cast<NzOpenALDetail::LPALGETBOOLEAN>(LoadEntry("alGetBoolean"));
			alGetBooleanv = reinterpret_cast<NzOpenALDetail::LPALGETBOOLEANV>(LoadEntry("alGetBooleanv"));
			alGetBuffer3f = reinterpret_cast<NzOpenALDetail::LPALGETBUFFER3F>(LoadEntry("alGetBuffer3f"));
			alGetBuffer3i = reinterpret_cast<NzOpenALDetail::LPALGETBUFFER3I>(LoadEntry("alGetBuffer3i"));
			alGetBufferf = reinterpret_cast<NzOpenALDetail::LPALGETBUFFERF>(LoadEntry("alGetBufferf"));
			alGetBufferfv = reinterpret_cast<NzOpenALDetail::LPALGETBUFFERFV>(LoadEntry("alGetBufferfv"));
			alGetBufferi = reinterpret_cast<NzOpenALDetail::LPALGETBUFFERI>(LoadEntry("alGetBufferi"));
			alGetBufferiv = reinterpret_cast<NzOpenALDetail::LPALGETBUFFERIV>(LoadEntry("alGetBufferiv"));
			alGetDouble = reinterpret_cast<NzOpenALDetail::LPALGETDOUBLE>(LoadEntry("alGetDouble"));
			alGetDoublev = reinterpret_cast<NzOpenALDetail::LPALGETDOUBLEV>(LoadEntry("alGetDoublev"));
			alGetEnumValue = reinterpret_cast<NzOpenALDetail::LPALGETENUMVALUE>(LoadEntry("alGetEnumValue"));
			alGetError = reinterpret_cast<NzOpenALDetail::LPALGETERROR>(LoadEntry("alGetError"));
			alGetFloat = reinterpret_cast<NzOpenALDetail::LPALGETFLOAT>(LoadEntry("alGetFloat"));
			alGetFloatv = reinterpret_cast<NzOpenALDetail::LPALGETFLOATV>(LoadEntry("alGetFloatv"));
			alGetInteger = reinterpret_cast<NzOpenALDetail::LPALGETINTEGER>(LoadEntry("alGetInteger"));
			alGetIntegerv = reinterpret_cast<NzOpenALDetail::LPALGETINTEGERV>(LoadEntry("alGetIntegerv"));
			alGetListener3f = reinterpret_cast<NzOpenALDetail::LPALGETLISTENER3F>(LoadEntry("alGetListener3f"));
			alGetListener3i = reinterpret_cast<NzOpenALDetail::LPALGETLISTENER3I>(LoadEntry("alGetListener3i"));
			alGetListenerf = reinterpret_cast<NzOpenALDetail::LPALGETLISTENERF>(LoadEntry("alGetListenerf"));
			alGetListenerfv = reinterpret_cast<NzOpenALDetail::LPALGETLISTENERFV>(LoadEntry("alGetListenerfv"));
			alGetListeneri = reinterpret_cast<NzOpenALDetail::LPALGETLISTENERI>(LoadEntry("alGetListeneri"));
			alGetListeneriv = reinterpret_cast<NzOpenALDetail::LPALGETLISTENERIV>(LoadEntry("alGetListeneriv"));
			alGetProcAddress = reinterpret_cast<NzOpenALDetail::LPALGETPROCADDRESS>(LoadEntry("alGetProcAddress"));
			alGetSource3f = reinterpret_cast<NzOpenALDetail::LPALGETSOURCE3F>(LoadEntry("alGetSource3f"));
			alGetSource3i = reinterpret_cast<NzOpenALDetail::LPALGETSOURCE3I>(LoadEntry("alGetSource3i"));
			alGetSourcef = reinterpret_cast<NzOpenALDetail::LPALGETSOURCEF>(LoadEntry("alGetSourcef"));
			alGetSourcefv = reinterpret_cast<NzOpenALDetail::LPALGETSOURCEFV>(LoadEntry("alGetSourcefv"));
			alGetSourcei = reinterpret_cast<NzOpenALDetail::LPALGETSOURCEI>(LoadEntry("alGetSourcei"));
			alGetSourceiv = reinterpret_cast<NzOpenALDetail::LPALGETSOURCEIV>(LoadEntry("alGetSourceiv"));
			alGetString = reinterpret_cast<NzOpenALDetail::LPALGETSTRING>(LoadEntry("alGetString"));
			alIsBuffer = reinterpret_cast<NzOpenALDetail::LPALISBUFFER>(LoadEntry("alIsBuffer"));
			alIsEnabled = reinterpret_cast<NzOpenALDetail::LPALISENABLED>(LoadEntry("alIsEnabled"));
			alIsExtensionPresent = reinterpret_cast<NzOpenALDetail::LPALISEXTENSIONPRESENT>(LoadEntry("alIsExtensionPresent"));
			alIsSource = reinterpret_cast<NzOpenALDetail::LPALISSOURCE>(LoadEntry("alIsSource"));
			alListener3f = reinterpret_cast<NzOpenALDetail::LPALLISTENER3F>(LoadEntry("alListener3f"));
			alListener3i = reinterpret_cast<NzOpenALDetail::LPALLISTENER3I>(LoadEntry("alListener3i"));
			alListenerf = reinterpret_cast<NzOpenALDetail::LPALLISTENERF>(LoadEntry("alListenerf"));
			alListenerfv = reinterpret_cast<NzOpenALDetail::LPALLISTENERFV>(LoadEntry("alListenerfv"));
			alListeneri = reinterpret_cast<NzOpenALDetail::LPALLISTENERI>(LoadEntry("alListeneri"));
			alListeneriv = reinterpret_cast<NzOpenALDetail::LPALLISTENERIV>(LoadEntry("alListeneriv"));
			alSource3f = reinterpret_cast<NzOpenALDetail::LPALSOURCE3F>(LoadEntry("alSource3f"));
			alSource3i = reinterpret_cast<NzOpenALDetail::LPALSOURCE3I>(LoadEntry("alSource3i"));
			alSourcef = reinterpret_cast<NzOpenALDetail::LPALSOURCEF>(LoadEntry("alSourcef"));
			alSourcefv = reinterpret_cast<NzOpenALDetail::LPALSOURCEFV>(LoadEntry("alSourcefv"));
			alSourcei = reinterpret_cast<NzOpenALDetail::LPALSOURCEI>(LoadEntry("alSourcei"));
			alSourceiv = reinterpret_cast<NzOpenALDetail::LPALSOURCEIV>(LoadEntry("alSourceiv"));
			alSourcePause = reinterpret_cast<NzOpenALDetail::LPALSOURCEPAUSE>(LoadEntry("alSourcePause"));
			alSourcePausev = reinterpret_cast<NzOpenALDetail::LPALSOURCEPAUSEV>(LoadEntry("alSourcePausev"));
			alSourcePlay = reinterpret_cast<NzOpenALDetail::LPALSOURCEPLAY>(LoadEntry("alSourcePlay"));
			alSourcePlayv = reinterpret_cast<NzOpenALDetail::LPALSOURCEPLAYV>(LoadEntry("alSourcePlayv"));
			alSourceQueueBuffers = reinterpret_cast<NzOpenALDetail::LPALSOURCEQUEUEBUFFERS>(LoadEntry("alSourceQueueBuffers"));
			alSourceRewind = reinterpret_cast<NzOpenALDetail::LPALSOURCEREWIND>(LoadEntry("alSourceRewind"));
			alSourceRewindv = reinterpret_cast<NzOpenALDetail::LPALSOURCEREWINDV>(LoadEntry("alSourceRewindv"));
			alSourceStop = reinterpret_cast<NzOpenALDetail::LPALSOURCESTOP>(LoadEntry("alSourceStop"));
			alSourceStopv = reinterpret_cast<NzOpenALDetail::LPALSOURCESTOPV>(LoadEntry("alSourceStopv"));
			alSourceUnqueueBuffers = reinterpret_cast<NzOpenALDetail::LPALSOURCEUNQUEUEBUFFERS>(LoadEntry("alSourceUnqueueBuffers"));
			alSpeedOfSound = reinterpret_cast<NzOpenALDetail::LPALSPEEDOFSOUND>(LoadEntry("alSpeedOfSound"));

			// alc
			alcCaptureCloseDevice = reinterpret_cast<NzOpenALDetail::LPALCCAPTURECLOSEDEVICE>(LoadEntry("alcCaptureCloseDevice"));
			alcCaptureOpenDevice = reinterpret_cast<NzOpenALDetail::LPALCCAPTUREOPENDEVICE>(LoadEntry("alcCaptureOpenDevice"));
			alcCaptureSamples = reinterpret_cast<NzOpenALDetail::LPALCCAPTURESAMPLES>(LoadEntry("alcCaptureSamples"));
			alcCaptureStart = reinterpret_cast<NzOpenALDetail::LPALCCAPTURESTART>(LoadEntry("alcCaptureStart"));
			alcCaptureStop = reinterpret_cast<NzOpenALDetail::LPALCCAPTURESTOP>(LoadEntry("alcCaptureStop"));
			alcCloseDevice = reinterpret_cast<NzOpenALDetail::LPALCCLOSEDEVICE>(LoadEntry("alcCloseDevice"));
			alcCreateContext = reinterpret_cast<NzOpenALDetail::LPALCCREATECONTEXT>(LoadEntry("alcCreateContext"));
			alcDestroyContext = reinterpret_cast<NzOpenALDetail::LPALCDESTROYCONTEXT>(LoadEntry("alcDestroyContext"));
			alcGetContextsDevice = reinterpret_cast<NzOpenALDetail::LPALCGETCONTEXTSDEVICE>(LoadEntry("alcGetContextsDevice"));
			alcGetCurrentContext = reinterpret_cast<NzOpenALDetail::LPALCGETCURRENTCONTEXT>(LoadEntry("alcGetCurrentContext"));
			alcGetEnumValue = reinterpret_cast<NzOpenALDetail::LPALCGETENUMVALUE>(LoadEntry("alcGetEnumValue"));
			alcGetError = reinterpret_cast<NzOpenALDetail::LPALCGETERROR>(LoadEntry("alcGetError"));
			alcGetIntegerv = reinterpret_cast<NzOpenALDetail::LPALCGETINTEGERV>(LoadEntry("alcGetIntegerv"));
			alcGetProcAddress = reinterpret_cast<NzOpenALDetail::LPALCGETPROCADDRESS>(LoadEntry("alcGetProcAddress"));
			alcGetString = reinterpret_cast<NzOpenALDetail::LPALCGETSTRING>(LoadEntry("alcGetString"));
			alcIsExtensionPresent = reinterpret_cast<NzOpenALDetail::LPALCISEXTENSIONPRESENT>(LoadEntry("alcIsExtensionPresent"));
			alcMakeContextCurrent = reinterpret_cast<NzOpenALDetail::LPALCMAKECONTEXTCURRENT>(LoadEntry("alcMakeContextCurrent"));
			alcOpenDevice = reinterpret_cast<NzOpenALDetail::LPALCOPENDEVICE>(LoadEntry("alcOpenDevice"));
			alcProcessContext = reinterpret_cast<NzOpenALDetail::LPALCPROCESSCONTEXT>(LoadEntry("alcProcessContext"));
			alcSuspendContext = reinterpret_cast<NzOpenALDetail::LPALCSUSPENDCONTEXT>(LoadEntry("alcSuspendContext"));

			succeeded = true;
			break;
		}
		catch (const std::exception& e)
		{
			NazaraWarning(libPath + " loading failed: " + NzString(e.what()));
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

bool NzOpenAL::IsInitialized()
{
	return s_library.IsLoaded();
}

unsigned int NzOpenAL::QueryInputDevices(std::vector<NzString>& devices)
{
	const char* deviceString = reinterpret_cast<const char*>(alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
	if (!deviceString)
		return 0;

	return ParseDevices(deviceString, devices);
}

unsigned int NzOpenAL::QueryOutputDevices(std::vector<NzString>& devices)
{
	const char* deviceString = reinterpret_cast<const char*>(alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));
	if (!deviceString)
		return 0;

	return ParseDevices(deviceString, devices);
}

bool NzOpenAL::SetDevice(const NzString& deviceName)
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

void NzOpenAL::Uninitialize()
{
	CloseDevice();

	s_rendererName.Clear(false);
	s_vendorName.Clear(false);
	s_library.Unload();
}

///ATTENTION: La valeur entière est le nombre de canaux possédés par ce format
ALenum NzOpenAL::AudioFormat[nzAudioFormat_Max+1] = {0}; // Valeur ajoutées au chargement d'OpenAL

void NzOpenAL::CloseDevice()
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
			// Nous n'avons pas pu fermer le device, ce qui signifie qu'il est en cours d'utilisation
			NazaraWarning("Failed to close device");

		s_device = nullptr;
	}
}

bool NzOpenAL::OpenDevice()
{
	// Initialisation du module
	s_device = alcOpenDevice(s_deviceName.IsEmpty() ? nullptr : s_deviceName.GetConstBuffer()); // On choisit le device par défaut
	if (!s_device)
	{
		NazaraError("Failed to open default device");
		return false;
	}

	// Un seul contexte nous suffira
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

			NazaraDebug("OpenAL version: " + NzString::Number(major) + '.' + NzString::Number(minor));
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

	// On complète le tableau de formats
	AudioFormat[nzAudioFormat_Mono] = AL_FORMAT_MONO16;
	AudioFormat[nzAudioFormat_Stereo] = AL_FORMAT_STEREO16;

	// "The presence of an enum value does not guarantee the applicability of an extension to the current context."
	if (alIsExtensionPresent("AL_EXT_MCFORMATS"))
	{
		AudioFormat[nzAudioFormat_Quad] = alGetEnumValue("AL_FORMAT_QUAD16");
		AudioFormat[nzAudioFormat_5_1]  = alGetEnumValue("AL_FORMAT_51CHN16");
		AudioFormat[nzAudioFormat_6_1]  = alGetEnumValue("AL_FORMAT_61CHN16");
		AudioFormat[nzAudioFormat_7_1]  = alGetEnumValue("AL_FORMAT_71CHN16");
	}
	else if (alIsExtensionPresent("AL_LOKI_quadriphonic"))
		AudioFormat[nzAudioFormat_Quad] = alGetEnumValue("AL_FORMAT_QUAD16_LOKI");

	return true;
}

NzOpenALFunc NzOpenAL::LoadEntry(const char* name, bool throwException)
{
	NzOpenALFunc entry = reinterpret_cast<NzOpenALFunc>(s_library.GetSymbol(name));
	if (!entry && throwException)
	{
		std::ostringstream oss;
		oss << "failed to load \"" << name << '"';

		throw std::runtime_error(oss.str());
	}

	return entry;
}

// al
NzOpenALDetail::LPALBUFFER3F             alBuffer3f             = nullptr;
NzOpenALDetail::LPALBUFFER3I             alBuffer3i             = nullptr;
NzOpenALDetail::LPALBUFFERDATA           alBufferData           = nullptr;
NzOpenALDetail::LPALBUFFERF              alBufferf              = nullptr;
NzOpenALDetail::LPALBUFFERFV             alBufferfv             = nullptr;
NzOpenALDetail::LPALBUFFERI              alBufferi              = nullptr;
NzOpenALDetail::LPALBUFFERIV             alBufferiv             = nullptr;
NzOpenALDetail::LPALDELETEBUFFERS        alDeleteBuffers        = nullptr;
NzOpenALDetail::LPALDELETESOURCES        alDeleteSources        = nullptr;
NzOpenALDetail::LPALDISABLE              alDisable              = nullptr;
NzOpenALDetail::LPALDISTANCEMODEL        alDistanceModel        = nullptr;
NzOpenALDetail::LPALDOPPLERFACTOR        alDopplerFactor        = nullptr;
NzOpenALDetail::LPALDOPPLERVELOCITY      alDopplerVelocity      = nullptr;
NzOpenALDetail::LPALENABLE               alEnable               = nullptr;
NzOpenALDetail::LPALGENBUFFERS           alGenBuffers           = nullptr;
NzOpenALDetail::LPALGENSOURCES           alGenSources           = nullptr;
NzOpenALDetail::LPALGETBOOLEAN           alGetBoolean           = nullptr;
NzOpenALDetail::LPALGETBOOLEANV          alGetBooleanv          = nullptr;
NzOpenALDetail::LPALGETBUFFER3F          alGetBuffer3f          = nullptr;
NzOpenALDetail::LPALGETBUFFER3I          alGetBuffer3i          = nullptr;
NzOpenALDetail::LPALGETBUFFERF           alGetBufferf           = nullptr;
NzOpenALDetail::LPALGETBUFFERFV          alGetBufferfv          = nullptr;
NzOpenALDetail::LPALGETBUFFERI           alGetBufferi           = nullptr;
NzOpenALDetail::LPALGETBUFFERIV          alGetBufferiv          = nullptr;
NzOpenALDetail::LPALGETDOUBLE            alGetDouble            = nullptr;
NzOpenALDetail::LPALGETDOUBLEV           alGetDoublev           = nullptr;
NzOpenALDetail::LPALGETENUMVALUE         alGetEnumValue         = nullptr;
NzOpenALDetail::LPALGETERROR             alGetError             = nullptr;
NzOpenALDetail::LPALGETFLOAT             alGetFloat             = nullptr;
NzOpenALDetail::LPALGETFLOATV            alGetFloatv            = nullptr;
NzOpenALDetail::LPALGETINTEGER           alGetInteger           = nullptr;
NzOpenALDetail::LPALGETINTEGERV          alGetIntegerv          = nullptr;
NzOpenALDetail::LPALGETLISTENER3F        alGetListener3f        = nullptr;
NzOpenALDetail::LPALGETLISTENER3I        alGetListener3i        = nullptr;
NzOpenALDetail::LPALGETLISTENERF         alGetListenerf         = nullptr;
NzOpenALDetail::LPALGETLISTENERFV        alGetListenerfv        = nullptr;
NzOpenALDetail::LPALGETLISTENERI         alGetListeneri         = nullptr;
NzOpenALDetail::LPALGETLISTENERIV        alGetListeneriv        = nullptr;
NzOpenALDetail::LPALGETPROCADDRESS       alGetProcAddress       = nullptr;
NzOpenALDetail::LPALGETSOURCE3F          alGetSource3f          = nullptr;
NzOpenALDetail::LPALGETSOURCE3I          alGetSource3i          = nullptr;
NzOpenALDetail::LPALGETSOURCEF           alGetSourcef           = nullptr;
NzOpenALDetail::LPALGETSOURCEFV          alGetSourcefv          = nullptr;
NzOpenALDetail::LPALGETSOURCEI           alGetSourcei           = nullptr;
NzOpenALDetail::LPALGETSOURCEIV          alGetSourceiv          = nullptr;
NzOpenALDetail::LPALGETSTRING            alGetString            = nullptr;
NzOpenALDetail::LPALISBUFFER             alIsBuffer             = nullptr;
NzOpenALDetail::LPALISENABLED            alIsEnabled            = nullptr;
NzOpenALDetail::LPALISEXTENSIONPRESENT   alIsExtensionPresent   = nullptr;
NzOpenALDetail::LPALISSOURCE             alIsSource             = nullptr;
NzOpenALDetail::LPALLISTENER3F           alListener3f           = nullptr;
NzOpenALDetail::LPALLISTENER3I           alListener3i           = nullptr;
NzOpenALDetail::LPALLISTENERF            alListenerf            = nullptr;
NzOpenALDetail::LPALLISTENERFV           alListenerfv           = nullptr;
NzOpenALDetail::LPALLISTENERI            alListeneri            = nullptr;
NzOpenALDetail::LPALLISTENERIV           alListeneriv           = nullptr;
NzOpenALDetail::LPALSOURCE3F             alSource3f             = nullptr;
NzOpenALDetail::LPALSOURCE3I             alSource3i             = nullptr;
NzOpenALDetail::LPALSOURCEF              alSourcef              = nullptr;
NzOpenALDetail::LPALSOURCEFV             alSourcefv             = nullptr;
NzOpenALDetail::LPALSOURCEI              alSourcei              = nullptr;
NzOpenALDetail::LPALSOURCEIV             alSourceiv             = nullptr;
NzOpenALDetail::LPALSOURCEPAUSE          alSourcePause          = nullptr;
NzOpenALDetail::LPALSOURCEPAUSEV         alSourcePausev         = nullptr;
NzOpenALDetail::LPALSOURCEPLAY           alSourcePlay           = nullptr;
NzOpenALDetail::LPALSOURCEPLAYV          alSourcePlayv          = nullptr;
NzOpenALDetail::LPALSOURCEQUEUEBUFFERS   alSourceQueueBuffers   = nullptr;
NzOpenALDetail::LPALSOURCEREWIND         alSourceRewind         = nullptr;
NzOpenALDetail::LPALSOURCEREWINDV        alSourceRewindv        = nullptr;
NzOpenALDetail::LPALSOURCESTOP           alSourceStop           = nullptr;
NzOpenALDetail::LPALSOURCESTOPV          alSourceStopv          = nullptr;
NzOpenALDetail::LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers = nullptr;
NzOpenALDetail::LPALSPEEDOFSOUND         alSpeedOfSound         = nullptr;

// alc
NzOpenALDetail::LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice = nullptr;
NzOpenALDetail::LPALCCAPTUREOPENDEVICE  alcCaptureOpenDevice  = nullptr;
NzOpenALDetail::LPALCCAPTURESAMPLES     alcCaptureSamples     = nullptr;
NzOpenALDetail::LPALCCAPTURESTART       alcCaptureStart       = nullptr;
NzOpenALDetail::LPALCCAPTURESTOP        alcCaptureStop        = nullptr;
NzOpenALDetail::LPALCCLOSEDEVICE        alcCloseDevice        = nullptr;
NzOpenALDetail::LPALCCREATECONTEXT      alcCreateContext      = nullptr;
NzOpenALDetail::LPALCDESTROYCONTEXT     alcDestroyContext     = nullptr;
NzOpenALDetail::LPALCGETCONTEXTSDEVICE  alcGetContextsDevice  = nullptr;
NzOpenALDetail::LPALCGETCURRENTCONTEXT  alcGetCurrentContext  = nullptr;
NzOpenALDetail::LPALCGETENUMVALUE       alcGetEnumValue       = nullptr;
NzOpenALDetail::LPALCGETERROR           alcGetError           = nullptr;
NzOpenALDetail::LPALCGETINTEGERV        alcGetIntegerv        = nullptr;
NzOpenALDetail::LPALCGETPROCADDRESS     alcGetProcAddress     = nullptr;
NzOpenALDetail::LPALCGETSTRING          alcGetString          = nullptr;
NzOpenALDetail::LPALCISEXTENSIONPRESENT alcIsExtensionPresent = nullptr;
NzOpenALDetail::LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = nullptr;
NzOpenALDetail::LPALCOPENDEVICE         alcOpenDevice         = nullptr;
NzOpenALDetail::LPALCPROCESSCONTEXT     alcProcessContext     = nullptr;
NzOpenALDetail::LPALCSUSPENDCONTEXT     alcSuspendContext     = nullptr;
