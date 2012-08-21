// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <sndfile/sndfile.h>
#include <Nazara/Audio/Debug.hpp>

namespace
{
	sf_count_t GetSize(void* user_data)
	{
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(user_data);
		return stream->GetSize();
	}

    sf_count_t Read(void* ptr, sf_count_t count, void* user_data)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(user_data);
        return stream->Read(ptr, count);
    }

    sf_count_t Seek(sf_count_t offset, int whence, void* user_data)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(user_data);
        switch (whence)
        {
			case SEEK_CUR:
				stream->Read(nullptr, offset);
				break;

			case SEEK_END:
				stream->SetCursorPos(stream->GetSize()+offset);
				break;

			case SEEK_SET:
				stream->SetCursorPos(offset);
				break;

			default:
				NazaraInternalError("Seek mode not handled");
        }

        return stream->GetCursorPos();
    }

    sf_count_t Tell(void* user_data)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(user_data);
        return stream->GetCursorPos();
    }

	static SF_VIRTUAL_IO callbacks = {GetSize, Seek, Read, nullptr, Tell};

	bool NzLoader_sndfile_Check(NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return true;
		}
		else
			return false;
	}

	bool NzLoader_sndfile_Load(NzSoundBuffer* soundBuffer, NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO infos;
		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &infos, &stream);
		if (!file)
		{
			NazaraError("Failed to load sound file: " + NzString(sf_strerror(file)));
			return false;
		}

		nzAudioFormat format = NzAudio::GetAudioFormat(infos.channels);
		if (format == nzAudioFormat_Unknown)
		{
			NazaraError("Channel count not handled");
			sf_close(file);

			return false;
		}

		unsigned int sampleCount = infos.frames*infos.channels;
		nzInt16* samples = new nzInt16[sampleCount];
		if (sf_read_short(file, samples, sampleCount) != sampleCount)
		{
			NazaraError("Failed to read samples");
			delete[] samples;
			sf_close(file);

			return false;
		}

		if (!soundBuffer->Create(format, infos.frames*infos.channels, infos.samplerate, samples))
		{
			NazaraError("Failed to create sound buffer");
			sf_close(file);

			return false;
		}

		delete[] samples;

		return true;
	}
}

void NzLoaders_sndfile_Register()
{
	NzSoundBufferLoader::RegisterLoader("aiff,au,avr,caf,flac,htk,ircam,mat4,mat5,mpc2k,nist,ogg,paf,pvf,raw,rf64,sd2,sds,svx,voc,w64,wav,wve",
										NzLoader_sndfile_Check,
										NzLoader_sndfile_Load);
}

void NzLoaders_sndfile_Unregister()
{
	NzSoundBufferLoader::UnregisterLoader("aiff,au,avr,caf,flac,htk,ircam,mat4,mat5,mpc2k,nist,ogg,paf,pvf,raw,rf64,sd2,sds,svx,voc,w64,wav,wve",
										NzLoader_sndfile_Check,
										NzLoader_sndfile_Load);
}
