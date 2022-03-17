// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENAL_HPP
#define NAZARA_AUDIO_OPENAL_HPP

// no include reordering

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

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

#define NAZARA_AUDIO_FOREACH_AL_FUNC(cb) \
	cb(alBuffer3f,             OpenALDetail::LPALBUFFER3F) \
	cb(alBuffer3i,             OpenALDetail::LPALBUFFER3I) \
	cb(alBufferData,           OpenALDetail::LPALBUFFERDATA) \
	cb(alBufferf,              OpenALDetail::LPALBUFFERF) \
	cb(alBufferfv,             OpenALDetail::LPALBUFFERFV) \
	cb(alBufferi,              OpenALDetail::LPALBUFFERI) \
	cb(alBufferiv,             OpenALDetail::LPALBUFFERIV) \
	cb(alDeleteBuffers,        OpenALDetail::LPALDELETEBUFFERS) \
	cb(alDeleteSources,        OpenALDetail::LPALDELETESOURCES) \
	cb(alDisable,              OpenALDetail::LPALDISABLE) \
	cb(alDistanceModel,        OpenALDetail::LPALDISTANCEMODEL) \
	cb(alDopplerFactor,        OpenALDetail::LPALDOPPLERFACTOR) \
	cb(alDopplerVelocity,      OpenALDetail::LPALDOPPLERVELOCITY) \
	cb(alEnable,               OpenALDetail::LPALENABLE) \
	cb(alGenBuffers,           OpenALDetail::LPALGENBUFFERS) \
	cb(alGenSources,           OpenALDetail::LPALGENSOURCES) \
	cb(alGetBoolean,           OpenALDetail::LPALGETBOOLEAN) \
	cb(alGetBooleanv,          OpenALDetail::LPALGETBOOLEANV) \
	cb(alGetBuffer3f,          OpenALDetail::LPALGETBUFFER3F) \
	cb(alGetBuffer3i,          OpenALDetail::LPALGETBUFFER3I) \
	cb(alGetBufferf,           OpenALDetail::LPALGETBUFFERF) \
	cb(alGetBufferfv,          OpenALDetail::LPALGETBUFFERFV) \
	cb(alGetBufferi,           OpenALDetail::LPALGETBUFFERI) \
	cb(alGetBufferiv,          OpenALDetail::LPALGETBUFFERIV) \
	cb(alGetDouble,            OpenALDetail::LPALGETDOUBLE) \
	cb(alGetDoublev,           OpenALDetail::LPALGETDOUBLEV) \
	cb(alGetEnumValue,         OpenALDetail::LPALGETENUMVALUE) \
	cb(alGetError,             OpenALDetail::LPALGETERROR) \
	cb(alGetFloat,             OpenALDetail::LPALGETFLOAT) \
	cb(alGetFloatv,            OpenALDetail::LPALGETFLOATV) \
	cb(alGetInteger,           OpenALDetail::LPALGETINTEGER) \
	cb(alGetIntegerv,          OpenALDetail::LPALGETINTEGERV) \
	cb(alGetListener3f,        OpenALDetail::LPALGETLISTENER3F) \
	cb(alGetListener3i,        OpenALDetail::LPALGETLISTENER3I) \
	cb(alGetListenerf,         OpenALDetail::LPALGETLISTENERF) \
	cb(alGetListenerfv,        OpenALDetail::LPALGETLISTENERFV) \
	cb(alGetListeneri,         OpenALDetail::LPALGETLISTENERI) \
	cb(alGetListeneriv,        OpenALDetail::LPALGETLISTENERIV) \
	cb(alGetProcAddress,       OpenALDetail::LPALGETPROCADDRESS) \
	cb(alGetSource3f,          OpenALDetail::LPALGETSOURCE3F) \
	cb(alGetSource3i,          OpenALDetail::LPALGETSOURCE3I) \
	cb(alGetSourcef,           OpenALDetail::LPALGETSOURCEF) \
	cb(alGetSourcefv,          OpenALDetail::LPALGETSOURCEFV) \
	cb(alGetSourcei,           OpenALDetail::LPALGETSOURCEI) \
	cb(alGetSourceiv,          OpenALDetail::LPALGETSOURCEIV) \
	cb(alGetString,            OpenALDetail::LPALGETSTRING) \
	cb(alIsBuffer,             OpenALDetail::LPALISBUFFER) \
	cb(alIsEnabled,            OpenALDetail::LPALISENABLED) \
	cb(alIsExtensionPresent,   OpenALDetail::LPALISEXTENSIONPRESENT) \
	cb(alIsSource,             OpenALDetail::LPALISSOURCE) \
	cb(alListener3f,           OpenALDetail::LPALLISTENER3F) \
	cb(alListener3i,           OpenALDetail::LPALLISTENER3I) \
	cb(alListenerf,            OpenALDetail::LPALLISTENERF) \
	cb(alListenerfv,           OpenALDetail::LPALLISTENERFV) \
	cb(alListeneri,            OpenALDetail::LPALLISTENERI) \
	cb(alListeneriv,           OpenALDetail::LPALLISTENERIV) \
	cb(alSource3f,             OpenALDetail::LPALSOURCE3F) \
	cb(alSource3i,             OpenALDetail::LPALSOURCE3I) \
	cb(alSourcef,              OpenALDetail::LPALSOURCEF) \
	cb(alSourcefv,             OpenALDetail::LPALSOURCEFV) \
	cb(alSourcei,              OpenALDetail::LPALSOURCEI) \
	cb(alSourceiv,             OpenALDetail::LPALSOURCEIV) \
	cb(alSourcePause,          OpenALDetail::LPALSOURCEPAUSE) \
	cb(alSourcePausev,         OpenALDetail::LPALSOURCEPAUSEV) \
	cb(alSourcePlay,           OpenALDetail::LPALSOURCEPLAY) \
	cb(alSourcePlayv,          OpenALDetail::LPALSOURCEPLAYV) \
	cb(alSourceQueueBuffers,   OpenALDetail::LPALSOURCEQUEUEBUFFERS) \
	cb(alSourceRewind,         OpenALDetail::LPALSOURCEREWIND) \
	cb(alSourceRewindv,        OpenALDetail::LPALSOURCEREWINDV) \
	cb(alSourceStop,           OpenALDetail::LPALSOURCESTOP) \
	cb(alSourceStopv,          OpenALDetail::LPALSOURCESTOPV) \
	cb(alSourceUnqueueBuffers, OpenALDetail::LPALSOURCEUNQUEUEBUFFERS) \
	cb(alSpeedOfSound,         OpenALDetail::LPALSPEEDOFSOUND)

#define NAZARA_AUDIO_FOREACH_ALC_FUNC(cb) \
	cb(alcCaptureCloseDevice, OpenALDetail::LPALCCAPTURECLOSEDEVICE) \
	cb(alcCaptureOpenDevice,  OpenALDetail::LPALCCAPTUREOPENDEVICE) \
	cb(alcCaptureSamples,     OpenALDetail::LPALCCAPTURESAMPLES) \
	cb(alcCaptureStart,       OpenALDetail::LPALCCAPTURESTART) \
	cb(alcCaptureStop,        OpenALDetail::LPALCCAPTURESTOP) \
	cb(alcCloseDevice,        OpenALDetail::LPALCCLOSEDEVICE) \
	cb(alcCreateContext,      OpenALDetail::LPALCCREATECONTEXT) \
	cb(alcDestroyContext,     OpenALDetail::LPALCDESTROYCONTEXT) \
	cb(alcGetContextsDevice,  OpenALDetail::LPALCGETCONTEXTSDEVICE) \
	cb(alcGetCurrentContext,  OpenALDetail::LPALCGETCURRENTCONTEXT) \
	cb(alcGetEnumValue,       OpenALDetail::LPALCGETENUMVALUE) \
	cb(alcGetError,           OpenALDetail::LPALCGETERROR) \
	cb(alcGetIntegerv,        OpenALDetail::LPALCGETINTEGERV) \
	cb(alcGetProcAddress,     OpenALDetail::LPALCGETPROCADDRESS) \
	cb(alcGetString,          OpenALDetail::LPALCGETSTRING) \
	cb(alcIsExtensionPresent, OpenALDetail::LPALCISEXTENSIONPRESENT) \
	cb(alcMakeContextCurrent, OpenALDetail::LPALCMAKECONTEXTCURRENT) \
	cb(alcOpenDevice,         OpenALDetail::LPALCOPENDEVICE) \
	cb(alcProcessContext,     OpenALDetail::LPALCPROCESSCONTEXT) \
	cb(alcSuspendContext,     OpenALDetail::LPALCSUSPENDCONTEXT) \

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENAL_HPP
