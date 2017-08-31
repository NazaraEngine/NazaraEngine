// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENAL_HPP
#define NAZARA_OPENAL_HPP

#ifdef NAZARA_AUDIO_OPENAL

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/String.hpp>
#include <vector>

// Inclusion of OpenAL headers

// OpenAL headers does not allow us to only get the signatures without the pointers to the functions
// And I do no want to modify them, I'm obliged to put them in a different namespace
// to put only interesting things back in the global namespace (specially typedef)
namespace OpenALDetail
{
	#include <AL/al.h>
	#include <AL/alc.h>
}

// If someone has a better idea ...
using OpenALDetail::ALboolean;
using OpenALDetail::ALbyte;
using OpenALDetail::ALchar;
using OpenALDetail::ALdouble;
using OpenALDetail::ALenum;
using OpenALDetail::ALfloat;
using OpenALDetail::ALint;
using OpenALDetail::ALshort;
using OpenALDetail::ALsizei;
using OpenALDetail::ALubyte;
using OpenALDetail::ALuint;
using OpenALDetail::ALushort;
using OpenALDetail::ALvoid;

using OpenALDetail::ALCboolean;
using OpenALDetail::ALCbyte;
using OpenALDetail::ALCchar;
using OpenALDetail::ALCcontext;
using OpenALDetail::ALCdevice;
using OpenALDetail::ALCdouble;
using OpenALDetail::ALCenum;
using OpenALDetail::ALCfloat;
using OpenALDetail::ALCint;
using OpenALDetail::ALCshort;
using OpenALDetail::ALCsizei;
using OpenALDetail::ALCubyte;
using OpenALDetail::ALCuint;
using OpenALDetail::ALCushort;
using OpenALDetail::ALCvoid;

namespace Nz
{
	using OpenALFunc = void(*)();

	class NAZARA_AUDIO_API OpenAL
	{
		public:
			static OpenALFunc GetEntry(const String& entryPoint);
			static String GetRendererName();
			static String GetVendorName();
			static unsigned int GetVersion();

			static bool Initialize(bool openDevice = true);

			static bool IsInitialized();

			static std::size_t QueryInputDevices(std::vector<String>& devices);
			static std::size_t QueryOutputDevices(std::vector<String>& devices);

			static bool SetDevice(const String& deviceName);

			static void Uninitialize();

			static ALenum AudioFormat[AudioFormat_Max + 1];

		private:
			static void CloseDevice();
			static bool OpenDevice();
			static OpenALFunc LoadEntry(const char* name, bool throwException = false);
	};

// al
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFER3F             alBuffer3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFER3I             alBuffer3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFERDATA           alBufferData;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFERF              alBufferf;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFERFV             alBufferfv;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFERI              alBufferi;
NAZARA_AUDIO_API extern OpenALDetail::LPALBUFFERIV             alBufferiv;
NAZARA_AUDIO_API extern OpenALDetail::LPALDELETEBUFFERS        alDeleteBuffers;
NAZARA_AUDIO_API extern OpenALDetail::LPALDELETESOURCES        alDeleteSources;
NAZARA_AUDIO_API extern OpenALDetail::LPALDISABLE              alDisable;
NAZARA_AUDIO_API extern OpenALDetail::LPALDISTANCEMODEL        alDistanceModel;
NAZARA_AUDIO_API extern OpenALDetail::LPALDOPPLERFACTOR        alDopplerFactor;
NAZARA_AUDIO_API extern OpenALDetail::LPALDOPPLERVELOCITY      alDopplerVelocity;
NAZARA_AUDIO_API extern OpenALDetail::LPALENABLE               alEnable;
NAZARA_AUDIO_API extern OpenALDetail::LPALGENBUFFERS           alGenBuffers;
NAZARA_AUDIO_API extern OpenALDetail::LPALGENSOURCES           alGenSources;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBOOLEAN           alGetBoolean;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBOOLEANV          alGetBooleanv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFER3F          alGetBuffer3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFER3I          alGetBuffer3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFERF           alGetBufferf;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFERFV          alGetBufferfv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFERI           alGetBufferi;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETBUFFERIV          alGetBufferiv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETDOUBLE            alGetDouble;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETDOUBLEV           alGetDoublev;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETENUMVALUE         alGetEnumValue;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETERROR             alGetError;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETFLOAT             alGetFloat;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETFLOATV            alGetFloatv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETINTEGER           alGetInteger;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETINTEGERV          alGetIntegerv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENER3F        alGetListener3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENER3I        alGetListener3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENERF         alGetListenerf;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENERFV        alGetListenerfv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENERI         alGetListeneri;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETLISTENERIV        alGetListeneriv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETPROCADDRESS       alGetProcAddress;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCE3F          alGetSource3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCE3I          alGetSource3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCEF           alGetSourcef;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCEFV          alGetSourcefv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCEI           alGetSourcei;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSOURCEIV          alGetSourceiv;
NAZARA_AUDIO_API extern OpenALDetail::LPALGETSTRING            alGetString;
NAZARA_AUDIO_API extern OpenALDetail::LPALISBUFFER             alIsBuffer;
NAZARA_AUDIO_API extern OpenALDetail::LPALISENABLED            alIsEnabled;
NAZARA_AUDIO_API extern OpenALDetail::LPALISEXTENSIONPRESENT   alIsExtensionPresent;
NAZARA_AUDIO_API extern OpenALDetail::LPALISSOURCE             alIsSource;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENER3F           alListener3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENER3I           alListener3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENERF            alListenerf;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENERFV           alListenerfv;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENERI            alListeneri;
NAZARA_AUDIO_API extern OpenALDetail::LPALLISTENERIV           alListeneriv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCE3F             alSource3f;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCE3I             alSource3i;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEF              alSourcef;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEFV             alSourcefv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEI              alSourcei;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEIV             alSourceiv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEPAUSE          alSourcePause;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEPAUSEV         alSourcePausev;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEPLAY           alSourcePlay;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEPLAYV          alSourcePlayv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEQUEUEBUFFERS   alSourceQueueBuffers;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEREWIND         alSourceRewind;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEREWINDV        alSourceRewindv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCESTOP           alSourceStop;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCESTOPV          alSourceStopv;
NAZARA_AUDIO_API extern OpenALDetail::LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers;
NAZARA_AUDIO_API extern OpenALDetail::LPALSPEEDOFSOUND         alSpeedOfSound;

// alc
NAZARA_AUDIO_API extern OpenALDetail::LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCAPTUREOPENDEVICE  alcCaptureOpenDevice;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCAPTURESAMPLES     alcCaptureSamples;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCAPTURESTART       alcCaptureStart;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCAPTURESTOP        alcCaptureStop;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCLOSEDEVICE        alcCloseDevice;
NAZARA_AUDIO_API extern OpenALDetail::LPALCCREATECONTEXT      alcCreateContext;
NAZARA_AUDIO_API extern OpenALDetail::LPALCDESTROYCONTEXT     alcDestroyContext;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETCONTEXTSDEVICE  alcGetContextsDevice;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETCURRENTCONTEXT  alcGetCurrentContext;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETENUMVALUE       alcGetEnumValue;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETERROR           alcGetError;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETINTEGERV        alcGetIntegerv;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETPROCADDRESS     alcGetProcAddress;
NAZARA_AUDIO_API extern OpenALDetail::LPALCGETSTRING          alcGetString;
NAZARA_AUDIO_API extern OpenALDetail::LPALCISEXTENSIONPRESENT alcIsExtensionPresent;
NAZARA_AUDIO_API extern OpenALDetail::LPALCMAKECONTEXTCURRENT alcMakeContextCurrent;
NAZARA_AUDIO_API extern OpenALDetail::LPALCOPENDEVICE         alcOpenDevice;
NAZARA_AUDIO_API extern OpenALDetail::LPALCPROCESSCONTEXT     alcProcessContext;
NAZARA_AUDIO_API extern OpenALDetail::LPALCSUSPENDCONTEXT     alcSuspendContext;

}

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_OPENAL_HPP
