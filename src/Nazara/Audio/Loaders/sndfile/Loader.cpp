// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <sndfile/sndfile.h>
#include <Nazara/Audio/Debug.hpp>

namespace
{
	const char* supportedFormats = "aiff,au,avr,caf,flac,htk,ircam,mat4,mat5,mpc2k,nist,ogg,paf,pvf,raw,rf64,sd2,sds,svx,voc,w64,wav,wve";

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

	class sndfileStream : public NzSoundStream
	{
		public:
			sndfileStream() = default;

			~sndfileStream()
			{
				if (m_file)
					sf_close(m_file);
			}

			nzUInt32 GetDuration() const
			{
				return m_duration;
			}

			nzAudioFormat GetFormat() const
			{
				return m_format;
			}

			unsigned int GetSampleCount() const
			{
				return m_sampleCount;
			}

			unsigned int GetSampleRate() const
			{
				return m_sampleRate;
			}

			bool Open(NzInputStream& stream)
			{
				SF_INFO infos;
				m_file = sf_open_virtual(&callbacks, SFM_READ, &infos, &stream);
				if (!m_file)
				{
					NazaraError("Failed to open sound: " + NzString(sf_strerror(m_file)));
					return false;
				}

				m_format = NzAudio::GetAudioFormat(infos.channels);
				if (m_format == nzAudioFormat_Unknown)
				{
					NazaraError("Channel count not handled");
					sf_close(m_file);
					m_file = nullptr;

					return false;
				}

				m_sampleCount = infos.channels*infos.frames;
				m_sampleRate = infos.samplerate;

				m_duration = 1000*m_sampleCount / (m_format*m_sampleRate);

				// https://github.com/LaurentGomila/SFML/issues/271
				// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
				///FIXME: Seulement le Vorbis ?
				/*if (infos.format & SF_FORMAT_VORBIS)
					sf_command(m_file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);*/

				return true;
			}

			unsigned int Read(void* buffer, unsigned int sampleCount)
			{
				return sf_read_short(m_file, reinterpret_cast<nzInt16*>(buffer), sampleCount);
			}

			void Seek(nzUInt32 offset)
			{
				sf_seek(m_file, offset*m_sampleRate / 1000, SEEK_SET);
			}

		private:
			nzAudioFormat m_format;
			SNDFILE* m_file = nullptr;
			unsigned int m_duration;
			unsigned int m_sampleCount;
			unsigned int m_sampleRate;
	};

	bool NzLoader_sndfile_Check_Music(NzInputStream& stream, const NzMusicParams& parameters)
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

	bool NzLoader_sndfile_Load_Music(NzMusic* music, NzInputStream& stream, const NzMusicParams& parameters)
	{
		NazaraUnused(parameters);

		sndfileStream* musicStream = new sndfileStream;
		if (!musicStream->Open(stream))
		{
			// L'erreur a déjà été signalée par la méthode
			delete musicStream;

			return false;
		}

		if (!music->Create(musicStream))
		{
			NazaraError("Failed to create music");
			delete musicStream;

			return false;
		}

		return true;
	}

	bool NzLoader_sndfile_Check_SoundBuffer(NzInputStream& stream, const NzSoundBufferParams& parameters)
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

	bool NzLoader_sndfile_Load_SoundBuffer(NzSoundBuffer* soundBuffer, NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		sndfileStream musicStream;
		if (!musicStream.Open(stream))
			return false; // L'erreur a déjà été envoyée par la méthode

		unsigned int sampleCount = musicStream.GetSampleCount();
		nzInt16* samples = new nzInt16[sampleCount];
		if (musicStream.Read(samples, sampleCount) != sampleCount)
		{
			NazaraError("Failed to read samples");
			return false;
		}

		if (!soundBuffer->Create(musicStream.GetFormat(), sampleCount, musicStream.GetSampleRate(), samples))
		{
			NazaraError("Failed to create sound buffer");
			delete[] samples;

			return false;
		}

		delete[] samples;

		return true;
	}
}

void NzLoaders_sndfile_Register()
{
	NzMusicLoader::RegisterLoader(supportedFormats, NzLoader_sndfile_Check_Music, NzLoader_sndfile_Load_Music);
	NzSoundBufferLoader::RegisterLoader(supportedFormats, NzLoader_sndfile_Check_SoundBuffer, NzLoader_sndfile_Load_SoundBuffer);
}

void NzLoaders_sndfile_Unregister()
{
	NzMusicLoader::UnregisterLoader(supportedFormats, NzLoader_sndfile_Check_Music, NzLoader_sndfile_Load_Music);
	NzSoundBufferLoader::UnregisterLoader(supportedFormats, NzLoader_sndfile_Check_SoundBuffer, NzLoader_sndfile_Load_SoundBuffer);
}
