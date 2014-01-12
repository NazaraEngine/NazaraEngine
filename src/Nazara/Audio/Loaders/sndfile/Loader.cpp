// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <memory>
#include <set>
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
        return static_cast<sf_count_t>(stream->Read(ptr, static_cast<std::size_t>(count)));
    }

    sf_count_t Seek(sf_count_t offset, int whence, void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        switch (whence)
        {
			case SEEK_CUR:
				stream->Read(nullptr, static_cast<std::size_t>(offset));
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
			sndfileStream() :
			m_file(nullptr),
			m_handle(nullptr)
			{
			}

			~sndfileStream()
			{
				if (m_handle)
					sf_close(m_handle);

				if (m_file)
					delete m_file;
			}

			nzUInt32 GetDuration() const
			{
				return m_duration;
			}

			nzAudioFormat GetFormat() const
			{
				if (m_mixToMono)
					return nzAudioFormat_Mono;
				else
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

			bool Open(const NzString& filePath, bool forceMono)
			{
				m_file = new NzFile(filePath);
				if (!m_file->Open(NzFile::ReadOnly))
				{
					NazaraError("Failed to open file " + filePath);
					return false;
				}

				return Open(*m_file, forceMono);
			}

			bool Open(NzInputStream& stream, bool forceMono)
			{
				SF_INFO infos;
				infos.format = 0;

				m_handle = sf_open_virtual(&callbacks, SFM_READ, &infos, &stream);
				if (!m_handle)
				{
					NazaraError("Failed to open sound: " + NzString(sf_strerror(m_handle)));
					return false;
				}

				m_format = NzAudio::GetAudioFormat(infos.channels);
				if (m_format == nzAudioFormat_Unknown)
				{
					sf_close(m_handle);
					m_handle = nullptr;

					NazaraError("Channel count not handled");
					return false;
				}

				m_sampleCount = infos.channels*infos.frames;
				m_sampleRate = infos.samplerate;

				m_duration = 1000*m_sampleCount / (m_format*m_sampleRate);

				// https://github.com/LaurentGomila/SFML/issues/271
				// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
				///FIXME: Seulement le Vorbis ?
				if (infos.format & SF_FORMAT_VORBIS)
					sf_command(m_handle, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

				if (forceMono && m_format != nzAudioFormat_Mono)
				{
					m_mixToMono = true;
					m_sampleCount = infos.frames;
				}
				else
					m_mixToMono = false;

				return true;
			}

			unsigned int Read(void* buffer, unsigned int sampleCount)
			{
				if (m_mixToMono)
				{
					std::unique_ptr<nzInt16[]> samples(new nzInt16[m_format*sampleCount]);
					unsigned int readSampleCount = sf_read_short(m_handle, samples.get(), m_format*sampleCount);
					NzMixToMono(samples.get(), reinterpret_cast<nzInt16*>(buffer), m_format, sampleCount);

					return readSampleCount / m_format;
				}
				else
					return sf_read_short(m_handle, reinterpret_cast<nzInt16*>(buffer), sampleCount);
			}

			void Seek(nzUInt32 offset)
			{
				sf_seek(m_handle, offset*m_sampleRate / 1000, SEEK_SET);
			}

		private:
			nzAudioFormat m_format;
			NzFile* m_file;
			SNDFILE* m_handle;
			bool m_mixToMono;
			unsigned int m_duration;
			unsigned int m_sampleCount;
			unsigned int m_sampleRate;
	};

	bool IsSupported(const NzString& extension)
	{
		static std::set<NzString> supportedExtensions = {
			"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k",
			"nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
		};

		return supportedExtensions.find(extension) != supportedExtensions.end();
	}

	nzTernary CheckMusic(NzInputStream& stream, const NzMusicParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		info.format = 0;

		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return nzTernary_True;
		}
		else
			return nzTernary_False;
	}

	bool LoadMusicFile(NzMusic* music, const NzString& filePath, const NzMusicParams& parameters)
	{
		NazaraUnused(parameters);

		std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
		if (!musicStream->Open(filePath, parameters.forceMono))
		{
			NazaraError("Failed to open music stream");
			return false;
		}

		if (!music->Create(musicStream.get()))
		{
			NazaraError("Failed to create music");
			return false;
		}

		musicStream.release();

		return true;
	}

	bool LoadMusicStream(NzMusic* music, NzInputStream& stream, const NzMusicParams& parameters)
	{
		NazaraUnused(parameters);

		std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
		if (!musicStream->Open(stream, parameters.forceMono))
		{
			NazaraError("Failed to open music stream");
			return false;
		}

		if (!music->Create(musicStream.get()))
		{
			NazaraError("Failed to create music");
			return false;
		}

		musicStream.release();

		return true;
	}

	nzTernary CheckSoundBuffer(NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		info.format = 0;

		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return nzTernary_True;
		}
		else
			return nzTernary_False;
	}

	bool LoadSoundBuffer(NzSoundBuffer* soundBuffer, NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		info.format = 0;

		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (!file)
		{
			NazaraError("Failed to load sound file: " + NzString(sf_strerror(file)));
			return false;
		}

		nzAudioFormat format = NzAudio::GetAudioFormat(info.channels);
		if (format == nzAudioFormat_Unknown)
		{
			NazaraError("Channel count not handled");
			sf_close(file);

			return false;
		}

		// https://github.com/LaurentGomila/SFML/issues/271
		// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
		///FIXME: Seulement le Vorbis ?
		if (info.format & SF_FORMAT_VORBIS)
			sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

		sf_count_t sampleCount = info.frames * info.channels;
		std::unique_ptr<nzInt16[]> samples(new nzInt16[sampleCount]);

		if (sf_read_short(file, samples.get(), sampleCount) != sampleCount)
		{
			sf_close(file);

			NazaraError("Failed to read samples");
			return false;
		}

		if (parameters.forceMono && format != nzAudioFormat_Mono)
		{
			std::unique_ptr<nzInt16[]> monoSamples(new nzInt16[info.frames]);
			NzMixToMono(samples.get(), monoSamples.get(), info.channels, info.frames);

			format = nzAudioFormat_Mono;
			samples = std::move(monoSamples);
			sampleCount = info.frames;
		}

		if (!soundBuffer->Create(format, static_cast<unsigned int>(sampleCount), info.samplerate, samples.get()))
		{
			sf_close(file);

			NazaraError("Failed to create sound buffer");
			return false;
		}

		return true;
	}
}

void NzLoaders_sndfile_Register()
{
	NzMusicLoader::RegisterLoader(IsSupported, CheckMusic, LoadMusicStream, LoadMusicFile);
	NzSoundBufferLoader::RegisterLoader(IsSupported, CheckSoundBuffer, LoadSoundBuffer);
}

void NzLoaders_sndfile_Unregister()
{
	NzMusicLoader::UnregisterLoader(IsSupported, CheckMusic, LoadMusicStream, LoadMusicFile);
	NzSoundBufferLoader::UnregisterLoader(IsSupported, CheckSoundBuffer, LoadSoundBuffer);
}
