// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <optional>
#include <set>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::optional<AudioFormat> GuessFormat(UInt32 channelCount)
		{
			switch (channelCount)
			{
				case 1:
					return AudioFormat::I16_Mono;

				case 2:
					return AudioFormat::I16_Stereo;

				case 4:
					return AudioFormat::I16_Quad;

				case 6:
					return AudioFormat::I16_5_1;

				case 7:
					return AudioFormat::I16_6_1;

				case 8:
					return AudioFormat::I16_7_1;

				default:
					return std::nullopt;
			}
		}

		std::size_t ReadCallback(void* ptr, size_t size, size_t nmemb, void* datasource)
		{
			Stream* stream = static_cast<Stream*>(datasource);
			return static_cast<std::size_t>(stream->Read(ptr, size * nmemb));
		}

		int SeekCallback(void* datasource, ogg_int64_t offset, int whence)
		{
			Stream* stream = static_cast<Stream*>(datasource);
			switch (whence)
			{
				case SEEK_CUR:
					stream->Read(nullptr, static_cast<std::size_t>(offset));
					break;

				case SEEK_END:
					stream->SetCursorPos(stream->GetSize() + offset); // offset is negative here
					break;

				case SEEK_SET:
					stream->SetCursorPos(offset);
					break;

				default:
					NazaraInternalError("Seek mode not handled");
					return false;
			}

			return 0;
		}

		long TellCallback(void* datasource)
		{
			Stream* stream = static_cast<Stream*>(datasource);
			return static_cast<long>(stream->GetCursorPos());
		}

		static ov_callbacks s_callbacks = {
			&ReadCallback,
			&SeekCallback,
			nullptr,
			&TellCallback
		};


		std::string ErrToString(int errCode)
		{
			switch (errCode)
			{
				case 0: return "no error";
				case OV_EBADHEADER: return "invalid Vorbis bitstream header";
				case OV_EBADLINK: return "an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt";
				case OV_EFAULT: return "internal logic fault";
				case OV_EINVAL: return "an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt";
				case OV_ENOTVORBIS: return "bitstream does not contain any Vorbis data";
				case OV_EREAD: return "a read from media returned an error";
				case OV_EVERSION: return "Vorbis version mismatch";
				case OV_HOLE: return "there was an interruption in the data";
				default: return "unknown error";
			}
		}

		UInt64 ReadOgg(OggVorbis_File* file, void* buffer, UInt64 sampleCount)
		{
			constexpr int bigendian = (GetPlatformEndianness() == Endianness::LittleEndian) ? 0 : 1;

			char* ptr = reinterpret_cast<char*>(buffer);
			UInt64 remainingBytes = sampleCount * sizeof(Int16);
			do
			{
				long readBytes = ov_read(file, ptr, int(remainingBytes), bigendian, 2, 1, nullptr);
				if (readBytes == 0)
					break; //< End of file

				if (readBytes < 0)
				{
					NazaraError("an error occurred while reading file: " + ErrToString(readBytes));
					return 0;
				}

				assert(readBytes > 0 && readBytes <= remainingBytes);

				ptr += readBytes;
				remainingBytes -= readBytes;
			}
			while (remainingBytes > 0);

			return sampleCount - remainingBytes / sizeof(Int16);
		}

		bool IsSupported(const std::string_view& extension)
		{
			static std::set<std::string_view> supportedExtensions = {
				"oga", "ogg", "ogm", "ogv", "ogx", "opus", "spx"
			};

			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary CheckOgg(Stream& stream, const ResourceParameters& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeVorbisLoader", &skip) && skip)
				return Ternary::False;

			OggVorbis_File file;
			if (ov_test_callbacks(&stream, &file, nullptr, 0, s_callbacks) != 0)
				return Ternary::False;

			ov_clear(&file);
			return Ternary::True;
		}

		std::shared_ptr<SoundBuffer> LoadSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			OggVorbis_File file;
			int err = ov_open_callbacks(&stream, &file, nullptr, 0, s_callbacks);
			if (err != 0)
			{
				NazaraError(ErrToString(err));
				return {};
			}

			CallOnExit clearOnExit([&] { ov_clear(&file); });

			vorbis_info* info = ov_info(&file, -1);
			assert(info);

			std::optional<AudioFormat> formatOpt = GuessFormat(info->channels);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: " + std::to_string(info->channels));
				return {};
			}

			AudioFormat format = *formatOpt;

			UInt64 frameCount = UInt64(ov_pcm_total(&file, -1));
			UInt64 sampleCount = UInt64(frameCount * info->channels);
			std::unique_ptr<Int16[]> samples = std::make_unique<Int16[]>(sampleCount); //< std::vector would default-init to zero

			UInt64 readSample = ReadOgg(&file, samples.get(), sampleCount);
			if (readSample == 0)
				return {};

			if (readSample != sampleCount)
			{
				NazaraError("failed to read the whole file");
				return {};
			}

			if (parameters.forceMono && format != AudioFormat::I16_Mono)
			{
				MixToMono(samples.get(), samples.get(), static_cast<UInt32>(info->channels), frameCount);

				format = AudioFormat::I16_Mono;
				sampleCount = frameCount;
			}
			
			return std::make_shared<SoundBuffer>(format, sampleCount, info->rate, samples.get());
		}

		class libvorbisStream : public SoundStream
		{
			public:
				libvorbisStream()
				{
					m_decoder.datasource = nullptr;
				}

				~libvorbisStream()
				{
					if (m_decoder.datasource)
						ov_clear(&m_decoder);
				}

				UInt32 GetDuration() const override
				{
					return m_duration;
				}

				AudioFormat GetFormat() const override
				{
					if (m_mixToMono)
						return AudioFormat::I16_Mono;
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
					std::unique_ptr<File> file = std::make_unique<File>();
					if (!file->Open(filePath, OpenMode::ReadOnly))
					{
						NazaraError("failed to open stream from file: " + Error::GetLastError());
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
					int err = ov_open_callbacks(&stream, &m_decoder, nullptr, 0, s_callbacks);
					if (err != 0)
					{
						NazaraError(ErrToString(err));
						return {};
					}

					CallOnExit clearOnError([&]
					{
						ov_clear(&m_decoder);
						m_decoder.datasource = nullptr;
					});

					vorbis_info* info = ov_info(&m_decoder, -1);
					assert(info);

					std::optional<AudioFormat> formatOpt = GuessFormat(info->channels);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: " + std::to_string(info->channels));
						return {};
					}

					m_format = *formatOpt;

					UInt64 frameCount = UInt64(ov_pcm_total(&m_decoder, -1));

					m_channelCount = info->channels;
					m_duration = UInt32(1000ULL * frameCount / info->rate);
					m_sampleCount = UInt64(frameCount * info->channels);
					m_sampleRate = info->rate;

					// Mixing to mono will be done on the fly
					if (forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = frameCount;
					}
					else
						m_mixToMono = false;

					clearOnError.Reset();

					return true;
				}

				UInt64 Read(void* buffer, UInt64 sampleCount) override
				{
					// Convert to mono in the fly if necessary
					if (m_mixToMono)
					{
						// Keep a buffer to the side to prevent allocation
						m_mixBuffer.resize(sampleCount * m_channelCount);

						std::size_t readSample = ReadOgg(&m_decoder, m_mixBuffer.data(), sampleCount * m_channelCount);
						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), m_channelCount, sampleCount);

						return readSample / m_channelCount;
					}
					else
					{
						UInt64 readSample = ReadOgg(&m_decoder, buffer, sampleCount);
						return readSample;
					}
				}

				void Seek(UInt64 offset) override
				{
					if (!m_mixToMono)
						offset /= m_channelCount;

					ov_pcm_seek(&m_decoder, Int64(offset));
				}

				UInt64 Tell() override
				{
					UInt64 offset = UInt64(ov_pcm_tell(&m_decoder));
					if (!m_mixToMono)
						offset *= m_channelCount;

					return offset;
				}

			private:
				std::mutex m_mutex;
				std::unique_ptr<Stream> m_ownedStream;
				std::vector<Int16> m_mixBuffer;
				AudioFormat m_format;
				OggVorbis_File m_decoder;
				UInt32 m_channelCount;
				UInt32 m_duration;
				UInt32 m_sampleRate;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		std::shared_ptr<SoundStream> LoadSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			if (!soundStream->Open(filePath, parameters.forceMono))
			{
				NazaraError("failed to open sound stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			if (!soundStream->Open(data, size, parameters.forceMono))
			{
				NazaraError("failed to open music stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			if (!soundStream->Open(stream, parameters.forceMono))
			{
				NazaraError("failed to open music stream");
				return {};
			}

			return soundStream;
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_libvorbis()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundBufferParams& parameters) { return CheckOgg(stream, parameters); };
			loaderEntry.streamLoader = LoadSoundBuffer;

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_libvorbis()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundStreamParams& parameters) { return CheckOgg(stream, parameters); };
			loaderEntry.fileLoader = LoadSoundStreamFile;
			loaderEntry.memoryLoader = LoadSoundStreamMemory;
			loaderEntry.streamLoader = LoadSoundStreamStream;

			return loaderEntry;
		}
	}
}
