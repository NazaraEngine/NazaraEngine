// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Formats/sndfileLoader.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <memory>
#include <set>
#include <vector>
#include <sndfile/sndfile.h>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		sf_count_t GetSize(void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return stream->GetSize();
		}

		sf_count_t Read(void* ptr, sf_count_t count, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return static_cast<sf_count_t>(stream->Read(ptr, static_cast<std::size_t>(count)));
		}

		sf_count_t Seek(sf_count_t offset, int whence, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
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
			Stream* stream = static_cast<Stream*>(user_data);
			return stream->GetCursorPos();
		}

		static SF_VIRTUAL_IO callbacks = {GetSize, Seek, Read, nullptr, Tell};

		class sndfileStream : public SoundStream
		{
			public:
				sndfileStream() :
				m_handle(nullptr)
				{
				}

				~sndfileStream()
				{
					if (m_handle)
						sf_close(m_handle);
				}

				UInt32 GetDuration() const override
				{
					return m_duration;
				}

				AudioFormat GetFormat() const override
				{
					// Nous avons besoin du nombre de canaux d'origine pour convertir en mono, nous trichons donc un peu...
					if (m_mixToMono)
						return AudioFormat_Mono;
					else
						return m_format;
				}

				UInt64 GetSampleCount() const override
				{
					return m_sampleCount;
				}

				UInt32 GetSampleRate() const override
				{
					return m_sampleRate;
				}

				bool Open(const String& filePath, bool forceMono)
				{
					// Nous devons gérer nous-même le flux car il doit rester ouvert après le passage du loader
					// (les flux automatiquement ouverts par le ResourceLoader étant fermés après celui-ci)
					std::unique_ptr<File> file(new File);
					if (!file->Open(filePath, OpenMode_ReadOnly))
					{
						NazaraError("Failed to open stream from file: " + Error::GetLastError());
						return false;
					}

					m_ownedStream = std::move(file);
					return Open(*m_ownedStream, forceMono);
				}

				bool Open(const void* data, std::size_t size, bool forceMono)
				{
					m_ownedStream.reset(new MemoryView(data, size));
					return Open(*m_ownedStream, forceMono);
				}

				bool Open(Stream& stream, bool forceMono)
				{
					SF_INFO infos;
					infos.format = 0; // Unknown format

					m_handle = sf_open_virtual(&callbacks, SFM_READ, &infos, &stream);
					if (!m_handle)
					{
						NazaraError("Failed to open sound: " + String(sf_strerror(m_handle)));
						return false;
					}

					// Un peu de RRID
					CallOnExit onExit([this]
					{
						sf_close(m_handle);
						m_handle = nullptr;
					});

					m_format = Audio::GetAudioFormat(infos.channels);
					if (m_format == AudioFormat_Unknown)
					{
						NazaraError("Channel count not handled");
						return false;
					}

					m_sampleCount = infos.channels*infos.frames;
					m_sampleRate = infos.samplerate;

					// Durée de la musique (s) = samples / channels*rate
					m_duration = static_cast<UInt32>(1000ULL*m_sampleCount / (m_format*m_sampleRate));

					// https://github.com/LaurentGomila/SFML/issues/271
					// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
					///FIXME: Seulement le Vorbis ?
					if (infos.format & SF_FORMAT_VORBIS)
						sf_command(m_handle, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

					// On mixera en mono lors de la lecture
					if (forceMono && m_format != AudioFormat_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = static_cast<UInt32>(infos.frames);
					}
					else
						m_mixToMono = false;

					onExit.Reset();

					return true;
				}

				UInt64 Read(void* buffer, UInt64 sampleCount) override
				{
					// Si la musique a été demandée en mono, nous devons la convertir à la volée lors de la lecture
					if (m_mixToMono)
					{
						// On garde un buffer sur le côté pour éviter la réallocation
						m_mixBuffer.resize(m_format * sampleCount);
						sf_count_t readSampleCount = sf_read_short(m_handle, m_mixBuffer.data(), m_format * sampleCount);
						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), m_format, sampleCount);

						return readSampleCount / m_format;
					}
					else
						return sf_read_short(m_handle, static_cast<Int16*>(buffer), sampleCount);
				}

				void Seek(UInt64 offset) override
				{
					sf_seek(m_handle, offset*m_sampleRate / 1000, SEEK_SET);
				}

			private:
				std::vector<Int16> m_mixBuffer;
				std::unique_ptr<Stream> m_ownedStream;
				AudioFormat m_format;
				SNDFILE* m_handle;
				bool m_mixToMono;
				UInt32 m_duration;
				UInt32 m_sampleRate;
				UInt64 m_sampleCount;
		};

		bool IsSupported(const String& extension)
		{
			static std::set<String> supportedExtensions = {
				"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k",
				"nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
			};

			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary CheckMusic(Stream& stream, const MusicParams& parameters)
		{
			NazaraUnused(parameters);

			SF_INFO info;
			info.format = 0; // Format inconnu

			// Si on peut ouvrir le flux, c'est qu'il est dans un format compatible
			SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
			if (file)
			{
				sf_close(file);
				return Ternary_True;
			}
			else
				return Ternary_False;
		}

		bool LoadMusicFile(Music* music, const String& filePath, const MusicParams& parameters)
		{
			std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
			if (!musicStream->Open(filePath, parameters.forceMono))
			{
				NazaraError("Failed to open music stream");
				return false;
			}

			if (!music->Create(musicStream.get())) // Transfert de propriété
			{
				NazaraError("Failed to create music");
				return false;
			}

			// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
			musicStream.release();

			return true;
		}

		bool LoadMusicMemory(Music* music, const void* data, std::size_t size, const MusicParams& parameters)
		{
			std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
			if (!musicStream->Open(data, size, parameters.forceMono))
			{
				NazaraError("Failed to open music stream");
				return false;
			}

			if (!music->Create(musicStream.get())) // Transfert de propriété
			{
				NazaraError("Failed to create music");
				return false;
			}

			// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
			musicStream.release();

			return true;
		}

		bool LoadMusicStream(Music* music, Stream& stream, const MusicParams& parameters)
		{
			std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
			if (!musicStream->Open(stream, parameters.forceMono))
			{
				NazaraError("Failed to open music stream");
				return false;
			}

			if (!music->Create(musicStream.get())) // Transfert de propriété
			{
				NazaraError("Failed to create music");
				return false;
			}

			// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
			musicStream.release();

			return true;
		}

		Ternary CheckSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			NazaraUnused(parameters);

			SF_INFO info;
			info.format = 0;

			SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
			if (file)
			{
				sf_close(file);
				return Ternary_True;
			}
			else
				return Ternary_False;
		}

		bool LoadSoundBuffer(SoundBuffer* soundBuffer, Stream& stream, const SoundBufferParams& parameters)
		{
			SF_INFO info;
			info.format = 0;

			SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
			if (!file)
			{
				NazaraError("Failed to load sound file: " + String(sf_strerror(file)));
				return false;
			}

			// Lynix utilise RAII...
			// C'est très efficace !
			// MemoryLeak est confus...
			CallOnExit onExit([file]
			{
				sf_close(file);
			});

			AudioFormat format = Audio::GetAudioFormat(info.channels);
			if (format == AudioFormat_Unknown)
			{
				NazaraError("Channel count not handled");
				return false;
			}

			// https://github.com/LaurentGomila/SFML/issues/271
			// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
			///FIXME: Seulement le Vorbis ?
			if (info.format & SF_FORMAT_VORBIS)
				sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

			unsigned int sampleCount = static_cast<unsigned int>(info.frames * info.channels);
			std::unique_ptr<Int16[]> samples(new Int16[sampleCount]);

			if (sf_read_short(file, samples.get(), sampleCount) != sampleCount)
			{
				NazaraError("Failed to read samples");
				return false;
			}

			// Une conversion en mono est-elle nécessaire ?
			if (parameters.forceMono && format != AudioFormat_Mono)
			{
				// Nous effectuons la conversion en mono dans le même buffer (il va de toute façon être copié)
				MixToMono(samples.get(), samples.get(), static_cast<unsigned int>(info.channels), static_cast<unsigned int>(info.frames));

				format = AudioFormat_Mono;
				sampleCount = static_cast<unsigned int>(info.frames);
			}

			if (!soundBuffer->Create(format, sampleCount, info.samplerate, samples.get()))
			{
				NazaraError("Failed to create sound buffer");
				return false;
			}

			return true;
		}
	}

	namespace Loaders
	{
		void Register_sndfile()
		{
			MusicLoader::RegisterLoader(Detail::IsSupported, Detail::CheckMusic, Detail::LoadMusicStream, Detail::LoadMusicFile, Detail::LoadMusicMemory);
			SoundBufferLoader::RegisterLoader(Detail::IsSupported, Detail::CheckSoundBuffer, Detail::LoadSoundBuffer);
		}

		void Unregister_sndfile()
		{
			MusicLoader::UnregisterLoader(Detail::IsSupported, Detail::CheckMusic, Detail::LoadMusicStream, Detail::LoadMusicFile, Detail::LoadMusicMemory);
			SoundBufferLoader::UnregisterLoader(Detail::IsSupported, Detail::CheckSoundBuffer, Detail::LoadSoundBuffer);
		}
	}
}
