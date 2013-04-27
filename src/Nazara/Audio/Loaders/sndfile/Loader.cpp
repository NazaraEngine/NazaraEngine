// Copyright (C) 2013 Jérôme Leclercq
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
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
		return stream->GetSize();
	}

    sf_count_t Read(void* ptr, sf_count_t count, void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        return stream->Read(ptr, count);
    }

    sf_count_t Seek(sf_count_t offset, int whence, void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        switch (whence)
        {
			case SEEK_CUR:
				stream->Read(nullptr, offset);
				break;

			case SEEK_END:
				stream->SetCursorPos(stream->GetSize() + offset); // L'offset est négatif ici
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

	bool IsSupported(const NzString& extension)
	{
		static std::set<NzString> supportedExtensions = {
			"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k",
			"nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
		};

		return supportedExtensions.find(extension) != supportedExtensions.end();
	}

	nzTernary Check(NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return nzTernary_True;
		}
		else
			return nzTernary_False;
	}

	bool Load(NzSoundBuffer* soundBuffer, NzInputStream& stream, const NzSoundBufferParams& parameters)
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

		// https://github.com/LaurentGomila/SFML/issues/271
		// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
		///FIXME: Seulement le Vorbis ?
		if (infos.format & SF_FORMAT_VORBIS)
			sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

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

			delete[] samples;
			sf_close(file);

			return false;
		}

		delete[] samples;

		return true;
	}
}

void NzLoaders_sndfile_Register()
{
	NzSoundBufferLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_sndfile_Unregister()
{
	NzSoundBufferLoader::UnregisterLoader(IsSupported, Check, Load);
}
