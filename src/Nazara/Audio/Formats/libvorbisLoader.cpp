// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <frozen/string.h>
#include <frozen/unordered_set.h>
#include <optional>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::size_t VorbisReadCallback(void* ptr, size_t size, size_t nmemb, void* datasource)
		{
			Stream* stream = static_cast<Stream*>(datasource);
			return static_cast<std::size_t>(stream->Read(ptr, size * nmemb));
		}

		int VorbisSeekCallback(void* datasource, ogg_int64_t offset, int whence)
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

		long VorbisTellCallback(void* datasource)
		{
			Stream* stream = static_cast<Stream*>(datasource);
			return static_cast<long>(stream->GetCursorPos());
		}

		static ov_callbacks s_vorbisCallbacks = {
			&VorbisReadCallback,
			&VorbisSeekCallback,
			nullptr,
			&VorbisTellCallback
		};


		std::string VorbisErrToString(int errCode)
		{
			switch (errCode)
			{
			case 0:                 return "no error";
				case OV_EBADHEADER: return "invalid Vorbis bitstream header";
				case OV_EBADLINK:   return "an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt";
				case OV_EFAULT:     return "internal logic fault";
				case OV_EINVAL:     return "an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt";
				case OV_ENOTVORBIS: return "bitstream does not contain any Vorbis data";
				case OV_EREAD:      return "a read from media returned an error";
				case OV_EVERSION:   return "Vorbis version mismatch";
				case OV_HOLE:       return "there was an interruption in the data";
				default:            return "unknown error";
			}
		}

		UInt64 ReadOgg(OggVorbis_File* file, void* buffer, UInt64 sampleCount)
		{
			constexpr int bigendian = (PlatformEndianness == Endianness::LittleEndian) ? 0 : 1;

			char* ptr = reinterpret_cast<char*>(buffer);
			UInt64 remainingBytes = sampleCount * sizeof(Int16);
			do
			{
				long readBytes = ov_read(file, ptr, int(remainingBytes), bigendian, 2, 1, nullptr);
				if (readBytes == 0)
					break; //< End of file

				if (readBytes < 0)
				{
					NazaraErrorFmt("an error occurred while reading file: {0}", VorbisErrToString(readBytes));
					return 0;
				}

				assert(readBytes > 0 && UInt64(readBytes) <= remainingBytes);

				ptr += readBytes;
				remainingBytes -= readBytes;
			}
			while (remainingBytes > 0);

			return sampleCount - remainingBytes / sizeof(Int16);
		}

		bool IsVorbisSupported(std::string_view extension)
		{
			constexpr auto s_supportedExtensions = frozen::make_unordered_set<frozen::string>({ ".oga", ".ogg", ".ogm", ".ogv", ".ogx", ".opus", ".spx" });

			return s_supportedExtensions.find(extension) != s_supportedExtensions.end();
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadVorbisSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			OggVorbis_File file;
			int err = ov_open_callbacks(&stream, &file, nullptr, 0, s_vorbisCallbacks);
			if (err != 0)
				return Err(ResourceLoadingError::Unrecognized);

			CallOnExit clearOnExit([&] { ov_clear(&file); });

			vorbis_info* info = ov_info(&file, -1);
			assert(info);

			std::optional<AudioFormat> formatOpt = GuessAudioFormat(info->channels);
			if (!formatOpt)
			{
				NazaraErrorFmt("unexpected channel count: {0}", info->channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			AudioFormat format = *formatOpt;

			UInt64 frameCount = UInt64(ov_pcm_total(&file, -1));
			UInt64 sampleCount = UInt64(frameCount * info->channels);
			std::unique_ptr<Int16[]> samples = std::make_unique<Int16[]>(sampleCount); //< std::vector would default-init to zero

			UInt64 readSample = ReadOgg(&file, samples.get(), sampleCount);
			if (readSample == 0)
				return Err(ResourceLoadingError::DecodingError);

			if (readSample != sampleCount)
			{
				NazaraError("failed to read the whole file");
				return Err(ResourceLoadingError::DecodingError);
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

				Time GetDuration() const override
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

				Result<void, ResourceLoadingError> Open(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
				{
					std::unique_ptr<File> file = std::make_unique<File>();
					if (!file->Open(filePath, OpenMode::Read))
					{
						NazaraErrorFmt("failed to open stream from file: {0}", Error::GetLastError());
						return Err(ResourceLoadingError::FailedToOpenFile);
					}

					m_ownedStream = std::move(file);
					return Open(*m_ownedStream, parameters);
				}

				Result<void, ResourceLoadingError> Open(const void* data, std::size_t size, const SoundStreamParams& parameters)
				{
					m_ownedStream = std::make_unique<MemoryView>(data, size);
					return Open(*m_ownedStream, parameters);
				}

				Result<void, ResourceLoadingError> Open(Stream& stream, const SoundStreamParams& parameters)
				{
					int err = ov_open_callbacks(&stream, &m_decoder, nullptr, 0, s_vorbisCallbacks);
					if (err != 0)
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit clearOnError([&]
					{
						ov_clear(&m_decoder);
						m_decoder.datasource = nullptr;
					});

					vorbis_info* info = ov_info(&m_decoder, -1);
					assert(info);

					std::optional<AudioFormat> formatOpt = GuessAudioFormat(info->channels);
					if (!formatOpt)
					{
						NazaraErrorFmt("unexpected channel count: {0}", info->channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = *formatOpt;

					UInt64 frameCount = UInt64(ov_pcm_total(&m_decoder, -1));

					m_channelCount = info->channels;
					m_duration = Time::Microseconds(1'000'000LL * frameCount / info->rate);
					m_sampleCount = UInt64(frameCount * info->channels);
					m_sampleRate = info->rate;

					// Mixing to mono will be done on the fly
					if (parameters.forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = frameCount;
					}
					else
						m_mixToMono = false;

					clearOnError.Reset();

					return Ok();
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
				Time m_duration;
				UInt32 m_channelCount;
				UInt32 m_sampleRate;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadVorbisSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadVorbisSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadVorbisSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libvorbisStream> soundStream = std::make_shared<libvorbisStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_libvorbis()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsVorbisSupported;
			loaderEntry.streamLoader = LoadVorbisSoundBuffer;
			loaderEntry.parameterFilter = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinVorbisLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_libvorbis()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsVorbisSupported;
			loaderEntry.fileLoader = LoadVorbisSoundStreamFile;
			loaderEntry.memoryLoader = LoadVorbisSoundStreamMemory;
			loaderEntry.streamLoader = LoadVorbisSoundStreamStream;
			loaderEntry.parameterFilter = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinVorbisLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
