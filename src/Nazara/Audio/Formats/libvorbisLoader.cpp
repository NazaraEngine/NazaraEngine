// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/libvorbisLoader.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Export.hpp>
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

namespace Nz
{
	namespace
	{
		inline std::span<const AudioChannel> GetVorbisAudioChannelMap(UInt32 channelCount)
		{
			switch (channelCount)
			{
				case 0:
					return {};

				case 1:
				{
					static constexpr std::array s_channels = { AudioChannel::Mono };
					return s_channels;
				}

				case 2:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight };
					return s_channels;
				}

				case 3:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontCenter, AudioChannel::FrontRight };
					return s_channels;
				}

				case 4:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::BackLeft, AudioChannel::BackRight };
					return s_channels;
				}

				case 5:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontCenter, AudioChannel::FrontRight, AudioChannel::BackLeft, AudioChannel::BackRight };
					return s_channels;
				}

				case 6:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontCenter, AudioChannel::FrontRight, AudioChannel::BackLeft, AudioChannel::BackRight, AudioChannel::LFE };
					return s_channels;
				}

				case 7:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontCenter, AudioChannel::FrontRight, AudioChannel::SideLeft, AudioChannel::SideRight, AudioChannel::BackCenter, AudioChannel::LFE };
					return s_channels;
				}

				case 8:
				{
					static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontCenter, AudioChannel::FrontRight, AudioChannel::SideLeft, AudioChannel::SideRight, AudioChannel::BackLeft, AudioChannel::BackRight, AudioChannel::LFE };
					return s_channels;
				}

				default:
					return GetAudioChannelMap(channelCount);
			}
		}

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

		constexpr ov_callbacks s_vorbisCallbacks = {
			&VorbisReadCallback,
			&VorbisSeekCallback,
			nullptr,
			&VorbisTellCallback
		};


		std::string VorbisErrToString(int errCode)
		{
			switch (errCode)
			{
				case 0:             return "no error";
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

		UInt64 ReadOgg(OggVorbis_File* file, void* buffer, UInt64 frameCount)
		{
			constexpr int s_bigEndian = (PlatformEndianness == Endianness::LittleEndian) ? 0 : 1;

			char* ptr = reinterpret_cast<char*>(buffer);
			UInt64 remainingBytes = frameCount * file->vi->channels * sizeof(Int16);
			do
			{
				long readBytes = ov_read(file, ptr, int(remainingBytes), s_bigEndian, 2, 1, nullptr);
				if (readBytes == 0)
					break; //< End of file

				if (readBytes < 0)
				{
					NazaraError("an error occurred while reading file: {0}", VorbisErrToString(readBytes));
					return 0;
				}

				assert(readBytes > 0 && UInt64(readBytes) <= remainingBytes);

				ptr += readBytes;
				remainingBytes -= readBytes;
			}
			while (remainingBytes > 0);

			return frameCount - remainingBytes / (file->vi->channels * sizeof(Int16));
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

			NAZARA_DEFER(ov_clear(&file););

			vorbis_info* info = ov_info(&file, -1);
			assert(info);

			std::span<const AudioChannel> audioChannels = GetVorbisAudioChannelMap(info->channels);
			if (audioChannels.empty())
			{
				NazaraError("unexpected channel count: {0}", info->channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			UInt64 frameCount = UInt64(ov_pcm_total(&file, -1));

			AudioFormat format = AudioFormat::Signed16;

			std::shared_ptr<SoundBuffer> soundBuffer = std::make_shared<SoundBuffer>(format, audioChannels, frameCount, info->rate, nullptr);
			void* samples = soundBuffer->GetSamples();

			UInt64 readFrame = ReadOgg(&file, samples, frameCount);
			if (readFrame == 0)
				return Err(ResourceLoadingError::DecodingError);

			if (readFrame != frameCount)
			{
				NazaraError("failed to read the whole file");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (parameters.format != format)
				soundBuffer->ConvertFormat(parameters.format);

			return soundBuffer;
		}

		class libvorbisStream : public SoundStream
		{
			public:
				libvorbisStream() :
				m_currentFramePosition(0)
				{
					m_decoder.datasource = nullptr;
				}

				~libvorbisStream()
				{
					if (m_decoder.datasource)
						ov_clear(&m_decoder);
				}

				std::span<const AudioChannel> GetChannels() const override
				{
					return m_channels;
				}

				Time GetDuration() const override
				{
					return m_duration;
				}

				AudioFormat GetFormat() const override
				{
					return m_format;
				}

				UInt64 GetFrameCount() const override
				{
					return m_frameCount;
				}

				std::mutex* GetMutex() override
				{
					return &m_mutex;
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
						NazaraError("failed to open stream from file: {0}", Error::GetLastError());
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

				Result<void, ResourceLoadingError> Open(Stream& stream, const SoundStreamParams& /*parameters*/)
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

					m_channels = GetVorbisAudioChannelMap(info->channels);
					if (m_channels.empty())
					{
						NazaraError("unexpected channel count: {0}", info->channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = AudioFormat::Signed16;

					UInt64 frameCount = UInt64(ov_pcm_total(&m_decoder, -1));

					m_channelCount = info->channels;
					m_duration = Time::Microseconds(1'000'000LL * frameCount / info->rate);
					m_frameCount = UInt64(frameCount * info->channels);
					m_sampleRate = info->rate;

					clearOnError.Reset();

					return Ok();
				}

				Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) override
				{
					if (m_currentFramePosition != startingFrameIndex)
						ov_pcm_seek(&m_decoder, SafeCaster(startingFrameIndex));

					UInt64 readFrame = ReadOgg(&m_decoder, frameOut, frameCount);
					m_currentFramePosition = SafeCaster(ov_pcm_tell(&m_decoder));

					return ReadData{ readFrame, m_currentFramePosition };
				}

			private:
				std::mutex m_mutex;
				std::span<const AudioChannel> m_channels;
				std::unique_ptr<Stream> m_ownedStream;
				AudioFormat m_format;
				OggVorbis_File m_decoder;
				Time m_duration;
				UInt32 m_channelCount;
				UInt32 m_sampleRate;
				UInt64 m_currentFramePosition;
				UInt64 m_frameCount;
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
