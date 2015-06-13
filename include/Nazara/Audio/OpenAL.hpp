// Copyright (C) 2015 Jérôme Leclercq
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

// Inclusion des headers OpenAL

// Étant donné que les headers OpenAL ne nous permettent pas de n'avoir que les signatures sans les pointeurs de fonctions
// Et que je ne souhaite pas les modifier, je suis contraint de les placer dans un espace de nom différent pour ensuite
// remettre dans l'espace global les choses intéressantes (les typedef notamment)
namespace NzOpenALDetail
{
	#include <AL/al.h>
	#include <AL/alc.h>
}

// Si quelqu'un a une meilleure idée ...
using NzOpenALDetail::ALboolean;
using NzOpenALDetail::ALbyte;
using NzOpenALDetail::ALchar;
using NzOpenALDetail::ALdouble;
using NzOpenALDetail::ALenum;
using NzOpenALDetail::ALfloat;
using NzOpenALDetail::ALint;
using NzOpenALDetail::ALshort;
using NzOpenALDetail::ALsizei;
using NzOpenALDetail::ALubyte;
using NzOpenALDetail::ALuint;
using NzOpenALDetail::ALushort;
using NzOpenALDetail::ALvoid;

using NzOpenALDetail::ALCboolean;
using NzOpenALDetail::ALCbyte;
using NzOpenALDetail::ALCchar;
using NzOpenALDetail::ALCcontext;
using NzOpenALDetail::ALCdevice;
using NzOpenALDetail::ALCdouble;
using NzOpenALDetail::ALCenum;
using NzOpenALDetail::ALCfloat;
using NzOpenALDetail::ALCint;
using NzOpenALDetail::ALCshort;
using NzOpenALDetail::ALCsizei;
using NzOpenALDetail::ALCubyte;
using NzOpenALDetail::ALCuint;
using NzOpenALDetail::ALCushort;
using NzOpenALDetail::ALCvoid;

using NzOpenALFunc = void (*)();

class NAZARA_AUDIO_API NzOpenAL
{
	public:
		static NzOpenALFunc GetEntry(const NzString& entryPoint);
		static NzString GetRendererName();
		static NzString GetVendorName();
		static unsigned int GetVersion();

		static bool Initialize(bool openDevice = true);

		static bool IsInitialized();

		static unsigned int QueryInputDevices(std::vector<NzString>& devices);
		static unsigned int QueryOutputDevices(std::vector<NzString>& devices);

		static bool SetDevice(const NzString& deviceName);

		static void Uninitialize();

		static ALenum AudioFormat[nzAudioFormat_Max+1];

	private:
		static void CloseDevice();
		static bool OpenDevice();
		static NzOpenALFunc LoadEntry(const char* name, bool throwException = false);
};

// al
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFER3F             alBuffer3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFER3I             alBuffer3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFERDATA           alBufferData;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFERF              alBufferf;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFERFV             alBufferfv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFERI              alBufferi;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALBUFFERIV             alBufferiv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDELETEBUFFERS        alDeleteBuffers;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDELETESOURCES        alDeleteSources;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDISABLE              alDisable;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDISTANCEMODEL        alDistanceModel;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDOPPLERFACTOR        alDopplerFactor;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALDOPPLERVELOCITY      alDopplerVelocity;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALENABLE               alEnable;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGENBUFFERS           alGenBuffers;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGENSOURCES           alGenSources;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBOOLEAN           alGetBoolean;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBOOLEANV          alGetBooleanv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFER3F          alGetBuffer3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFER3I          alGetBuffer3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFERF           alGetBufferf;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFERFV          alGetBufferfv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFERI           alGetBufferi;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETBUFFERIV          alGetBufferiv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETDOUBLE            alGetDouble;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETDOUBLEV           alGetDoublev;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETENUMVALUE         alGetEnumValue;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETERROR             alGetError;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETFLOAT             alGetFloat;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETFLOATV            alGetFloatv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETINTEGER           alGetInteger;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETINTEGERV          alGetIntegerv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENER3F        alGetListener3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENER3I        alGetListener3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENERF         alGetListenerf;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENERFV        alGetListenerfv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENERI         alGetListeneri;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETLISTENERIV        alGetListeneriv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETPROCADDRESS       alGetProcAddress;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCE3F          alGetSource3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCE3I          alGetSource3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCEF           alGetSourcef;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCEFV          alGetSourcefv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCEI           alGetSourcei;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSOURCEIV          alGetSourceiv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALGETSTRING            alGetString;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALISBUFFER             alIsBuffer;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALISENABLED            alIsEnabled;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALISEXTENSIONPRESENT   alIsExtensionPresent;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALISSOURCE             alIsSource;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENER3F           alListener3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENER3I           alListener3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENERF            alListenerf;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENERFV           alListenerfv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENERI            alListeneri;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALLISTENERIV           alListeneriv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCE3F             alSource3f;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCE3I             alSource3i;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEF              alSourcef;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEFV             alSourcefv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEI              alSourcei;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEIV             alSourceiv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEPAUSE          alSourcePause;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEPAUSEV         alSourcePausev;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEPLAY           alSourcePlay;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEPLAYV          alSourcePlayv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEQUEUEBUFFERS   alSourceQueueBuffers;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEREWIND         alSourceRewind;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEREWINDV        alSourceRewindv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCESTOP           alSourceStop;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCESTOPV          alSourceStopv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALSPEEDOFSOUND         alSpeedOfSound;

// alc
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCAPTUREOPENDEVICE  alcCaptureOpenDevice;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCAPTURESAMPLES     alcCaptureSamples;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCAPTURESTART       alcCaptureStart;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCAPTURESTOP        alcCaptureStop;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCLOSEDEVICE        alcCloseDevice;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCCREATECONTEXT      alcCreateContext;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCDESTROYCONTEXT     alcDestroyContext;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETCONTEXTSDEVICE  alcGetContextsDevice;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETCURRENTCONTEXT  alcGetCurrentContext;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETENUMVALUE       alcGetEnumValue;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETERROR           alcGetError;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETINTEGERV        alcGetIntegerv;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETPROCADDRESS     alcGetProcAddress;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCGETSTRING          alcGetString;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCISEXTENSIONPRESENT alcIsExtensionPresent;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCMAKECONTEXTCURRENT alcMakeContextCurrent;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCOPENDEVICE         alcOpenDevice;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCPROCESSCONTEXT     alcProcessContext;
NAZARA_AUDIO_API extern NzOpenALDetail::LPALCSUSPENDCONTEXT     alcSuspendContext;

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_OPENAL_HPP
