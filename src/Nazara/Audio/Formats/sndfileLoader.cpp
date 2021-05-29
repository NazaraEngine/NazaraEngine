// Copyright (C) 2020 Jérôme Leclercq
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
#include <sndfile.h>
#include <memory>
#include <optional>
#include <set>
#include <string_view>
#include <vector>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		std::optional<AudioFormat> GuessFormat(UInt32 channelCount)
		{
			switch (channelCount)
			{
				case 1:
					return AudioFormat::U16_Mono;

				case 2:
					return AudioFormat::U16_Stereo;

				case 4:
					return AudioFormat::U16_Quad;

				case 6:
					return AudioFormat::U16_5_1;

				case 7:
					return AudioFormat::U16_6_1;

				case 8:
					return AudioFormat::U16_7_1;

				default:
					return std::nullopt;
			}
		}

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
						return AudioFormat::U16_Mono;
					else
						return m_format;
				}

				std::mutex& GetMutex() override
				{
					return m_mutex;
				}

				UInt64 GetSampleCount() const override
				{
					return m_sampleCount;
				}

				UInt32 GetSampleRate() const override
				{
					return m_sampleRate;
				}

				bool Open(const std::filesystem::path& filePath, bool forceMono)
				{
					// Nous devons gérer nous-même le flux car il doit rester ouvert après le passage du loader
					// (les flux automatiquement ouverts par le ResourceLoader étant fermés après celui-ci)
					std::unique_ptr<File> file = std::make_unique<File>();
					if (!file->Open(filePath, OpenMode::ReadOnly))
					{
						NazaraError("Failed to open stream from file: " + Error::GetLastError());
						return false;
					}

					m_ownedStream = std::move(file);
					return Open(*m_ownedStream, forceMono);
				}

				bool Open(const void* data, std::size_t size, bool forceMono)
				{
					m_ownedStream = std::make_unique<MemoryView>(data, size);
					return Open(*m_ownedStream, forceMono);
				}

				bool Open(Stream& stream, bool forceMono)
				{
					SF_INFO info;
					info.format = 0; // Unknown format

					m_handle = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
					if (!m_handle)
					{
						NazaraError("Failed to open sound: " + std::string(sf_strerror(m_handle)));
						return false;
					}

					// Un peu de RRID
					CallOnExit onExit([this]
					{
						sf_close(m_handle);
						m_handle = nullptr;
					});

					std::optional<AudioFormat> formatOpt = GuessFormat(info.channels);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: " + std::to_string(info.channels));
						return false;
					}

					m_format = *formatOpt;

					m_duration = static_cast<UInt32>(1000ULL * info.frames / info.samplerate);
					m_sampleCount = info.channels * info.frames;
					m_sampleRate = info.samplerate;

					// https://github.com/LaurentGomila/SFML/issues/271
					// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
					///FIXME: Seulement le Vorbis ?
					if (info.format & SF_FORMAT_VORBIS)
						sf_command(m_handle, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

					// On mixera en mono lors de la lecture
					if (forceMono && m_format != AudioFormat::U16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = static_cast<UInt32>(info.frames);
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
						UInt32 channelCount = GetChannelCount(m_format);

						// On garde un buffer sur le côté pour éviter la réallocation
						m_mixBuffer.resize(channelCount * sampleCount);
						sf_count_t readSampleCount = sf_read_short(m_handle, m_mixBuffer.data(), channelCount * sampleCount);
						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), channelCount, sampleCount);

						return readSampleCount / channelCount;
					}
					else
						return sf_read_short(m_handle, static_cast<Int16*>(buffer), sampleCount);
				}

				void Seek(UInt64 offset) override
				{
					sf_seek(m_handle, offset*m_sampleRate / 1000, SEEK_SET);
				}

				UInt64 Tell() override
				{
					return sf_seek(m_handle, 0, SEEK_CUR) * 1000 / m_sampleRate;
				}

			private:
				std::vector<Int16> m_mixBuffer;
				std::unique_ptr<Stream> m_ownedStream;
				AudioFormat m_format;
				SNDFILE* m_handle;
				bool m_mixToMono;
				std::mutex m_mutex;
				UInt32 m_duration;
				UInt32 m_sampleRate;
				UInt64 m_sampleCount;
		};

		bool IsSupported(const std::string_view& extension)
		{
			static std::set<std::string_view> supportedExtensions = {
				"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k",
				"nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
			};

			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary CheckSoundStream(Stream& stream, const SoundStreamParams& parameters)
		{
			NazaraUnused(parameters);

			SF_INFO info;
			info.format = 0; // Format inconnu

			// Si on peut ouvrir le flux, c'est qu'il est dans un format compatible
			SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
			if (file)
			{
				sf_close(file);
				return Ternary::True;
			}
			else
				return Ternary::False;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<sndfileStream> soundStream = std::make_shared<sndfileStream>();
			if (!soundStream->Open(filePath, parameters.forceMono))
			{
				NazaraError("Failed to open sound stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<sndfileStream> soundStream = std::make_shared<sndfileStream>();
			if (!soundStream->Open(data, size, parameters.forceMono))
			{
				NazaraError("Failed to open music stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<sndfileStream> soundStream = std::make_shared<sndfileStream>();
			if (!soundStream->Open(stream, parameters.forceMono))
			{
				NazaraError("Failed to open music stream");
				return {};
			}

			return soundStream;
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
				return Ternary::True;
			}
			else
				return Ternary::False;
		}

		std::shared_ptr<SoundBuffer> LoadSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			SF_INFO info;
			info.format = 0;

			SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
			if (!file)
			{
				NazaraError("Failed to load sound file: " + std::string(sf_strerror(file)));
				return {};
			}
			
			CallOnExit onExit([file]
			{
				sf_close(file);
			});

			std::optional<AudioFormat> formatOpt = GuessFormat(info.channels);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: " + std::to_string(info.channels));
				return {};
			}

			AudioFormat format = *formatOpt;

			// https://github.com/LaurentGomila/SFML/issues/271
			// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
			///FIXME: Only Vorbis?
			if (info.format & SF_FORMAT_VORBIS)
				sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

			sf_count_t sampleCount = static_cast<sf_count_t>(info.frames * info.channels);
			std::unique_ptr<Int16[]> samples = std::make_unique<Int16[]>(sampleCount); //< std::vector would default-init to zero

			if (sf_read_short(file, samples.get(), sampleCount) != sampleCount)
			{
				NazaraError("Failed to read samples");
				return {};
			}

			// Convert to mono if required
			if (parameters.forceMono && format != AudioFormat::U16_Mono)
			{
				MixToMono(samples.get(), samples.get(), static_cast<UInt32>(info.channels), static_cast<UInt64>(info.frames));

				format = AudioFormat::U16_Mono;
				sampleCount = static_cast<unsigned int>(info.frames);
			}

			return std::make_shared<SoundBuffer>(format, sampleCount, info.samplerate, samples.get());
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_sndfile()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = Detail::IsSupported;
			loaderEntry.streamChecker = Detail::CheckSoundBuffer;
			loaderEntry.streamLoader = Detail::LoadSoundBuffer;

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_sndfile()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = Detail::IsSupported;
			loaderEntry.streamChecker = Detail::CheckSoundStream;
			loaderEntry.fileLoader = Detail::LoadSoundStreamFile;
			loaderEntry.memoryLoader = Detail::LoadSoundStreamMemory;
			loaderEntry.streamLoader = Detail::LoadSoundStreamStream;

			return loaderEntry;
		}
	}
}
