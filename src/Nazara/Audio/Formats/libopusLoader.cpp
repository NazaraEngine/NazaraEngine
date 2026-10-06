// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/libopusLoader.hpp>
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
#include <opus/opusfile.h>
#include <optional>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr UInt32 OpusFileSampleRate = 48000;

		inline std::span<const AudioChannel> GetOpusAudioChannelMap(UInt32 channelCount)
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

		int OpusReadCallback(void* streamPtr, unsigned char* ptr, int nbytes)
		{
			Stream* stream = static_cast<Stream*>(streamPtr);
			return SafeCast<int>(stream->Read(ptr, nbytes));
		}

		int OpusSeekCallback(void* streamPtr, opus_int64 offset, int whence)
		{
			Stream* stream = static_cast<Stream*>(streamPtr);
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

		opus_int64 OpusTellCallback(void* streamPtr)
		{
			Stream* stream = static_cast<Stream*>(streamPtr);
			return static_cast<opus_int64>(stream->GetCursorPos());
		}

		constexpr OpusFileCallbacks s_opusCallbacks = {
			&OpusReadCallback,
			&OpusSeekCallback,
			&OpusTellCallback,
			nullptr
		};


		std::string OpusErrToString(int errCode)
		{
			switch (errCode)
			{
				case 0:                return "no error";
				case OP_FALSE:         return "A request did not succeed";
				case OP_HOLE:          return "There was a hole in the page sequence numbers (e.g., a page was corrupt or missing)";
				case OP_EREAD:         return "An underlying read, seek, or tell operation failed when it should have succeeded";
				case OP_EFAULT:        return "A NULL pointer was passed where one was unexpected, or an internal memory allocation failed, or an internal library error was encountered";
				case OP_EIMPL:         return "The stream used a feature that is not implemented, such as an unsupported channel family";
				case OP_EINVAL:        return "One or more parameters to a function were invalid";
				case OP_ENOTFORMAT:    return R"(A purported Ogg Opus stream did not begin with an Ogg page, a purported header packet did not start with one of the required strings, "OpusHead" or "OpusTags", or a link in a chained file was encountered that did not contain any logical Opus streams)";
				case OP_EBADHEADER:    return "A required header packet was not properly formatted, contained illegal values, or was missing altogether";
				case OP_EVERSION:      return "The ID header contained an unrecognized version number";
				case OP_EBADPACKET:    return "An audio packet failed to decode properly. This is usually caused by a multistream Ogg packet where the durations of the individual Opus packets contained in it are not all the same";
				case OP_EBADLINK:      return "We failed to find data we had seen before, or the bitstream structure was sufficiently malformed that seeking to the target destination was impossible";
				case OP_ENOSEEK:       return "An operation that requires seeking was requested on an unseekable stream";
				case OP_EBADTIMESTAMP: return "The first or last granule position of a link failed basic validity checks";
				default:               return fmt::format("unknown error {}", errCode);
			}
		}

		UInt64 ReadOggS16(OggOpusFile* file, int channelCount, Int16* buffer, UInt64 frameCount)
		{
			UInt64 remainingBytesPerChannel = frameCount * sizeof(Int16);
			UInt64 totalFrameCount = 0;
			do
			{
				int frameRead = op_read(file, buffer, SafeCaster(remainingBytesPerChannel), nullptr);
				if (frameRead == 0)
					break; //< End of file

				if (frameRead < 0)
				{
					NazaraError("an error occurred while reading file: {0}", OpusErrToString(frameRead));
					return 0;
				}

				assert(frameRead > 0 && UInt64(frameRead) <= remainingBytesPerChannel);

				totalFrameCount += UInt64(frameRead);
				buffer += frameRead * channelCount;
				remainingBytesPerChannel -= frameRead * sizeof(Int16);
			}
			while (remainingBytesPerChannel > 0);

			return totalFrameCount;
		}

		UInt64 ReadOggF32(OggOpusFile* file, int channelCount, float* buffer, UInt64 frameCount)
		{
			UInt64 remainingBytesPerChannel = frameCount * sizeof(float);
			UInt64 totalFrameCount = 0;
			do
			{
				int frameRead = op_read_float(file, buffer, SafeCaster(remainingBytesPerChannel), nullptr);
				if (frameRead == 0)
					break; //< End of file

				if (frameRead < 0)
				{
					NazaraError("an error occurred while reading file: {0}", OpusErrToString(frameRead));
					return 0;
				}

				assert(frameRead > 0 && UInt64(frameRead) <= remainingBytesPerChannel);

				totalFrameCount += UInt64(frameRead);
				buffer += frameRead * channelCount;
				remainingBytesPerChannel -= frameRead * sizeof(float);
			}
			while (remainingBytesPerChannel > 0);

			return totalFrameCount;
		}

		bool IsOpusSupported(std::string_view extension)
		{
			constexpr auto s_supportedExtensions = frozen::make_unordered_set<frozen::string>({ ".oga", ".ogg", ".ogm", ".ogv", ".ogx", ".opus" });

			return s_supportedExtensions.find(extension) != s_supportedExtensions.end();
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadOpusSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			int err;
			OggOpusFile* file = op_open_callbacks(&stream, &s_opusCallbacks, nullptr, 0, &err);
			if (!file)
				return Err(ResourceLoadingError::Unrecognized);

			NAZARA_DEFER(op_free(file););

			int channelCount = op_channel_count(file, -1);
			std::span<const AudioChannel> audioChannels = GetOpusAudioChannelMap(channelCount);
			if (audioChannels.empty())
			{
				NazaraError("unexpected channel count: {0}", channelCount);
				return Err(ResourceLoadingError::Unsupported);
			}

			UInt64 frameCount = UInt64(op_pcm_total(file, -1));

			AudioFormat format = parameters.format;
			if (format != AudioFormat::Signed16)
				format = AudioFormat::Floating32;

			std::shared_ptr<SoundBuffer> soundBuffer = std::make_shared<SoundBuffer>(format, audioChannels, frameCount, OpusFileSampleRate, nullptr);
			void* samples = soundBuffer->GetSamples();

			UInt64 readFrame;
			if (format == AudioFormat::Floating32)
				readFrame = ReadOggF32(file, channelCount, static_cast<float*>(samples), frameCount);
			else
				readFrame = ReadOggS16(file, channelCount, static_cast<Int16*>(samples), frameCount);

			if (readFrame == 0)
				return Err(ResourceLoadingError::DecodingError);

			if (readFrame != frameCount)
			{
				NazaraError("failed to read the whole file");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (parameters.format != format)
				soundBuffer->ConvertToFormat(parameters.format);

			return soundBuffer;
		}

		class libopusStream : public SoundStream
		{
			public:
				libopusStream() :
				m_decoder(nullptr),
				m_currentFramePosition(0)
				{
				}

				~libopusStream()
				{
					if (m_decoder)
						op_free(m_decoder);
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
					return OpusFileSampleRate;
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
					int err;
					m_decoder = op_open_callbacks(&stream, &s_opusCallbacks, nullptr, 0, &err);
					if (!m_decoder)
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit clearOnError([&]
					{
						op_free(m_decoder);
						m_decoder = nullptr;
					});

					m_channelCount = SafeCaster(op_channel_count(m_decoder, -1));

					m_channels = GetOpusAudioChannelMap(m_channelCount);
					if (m_channels.empty())
					{
						NazaraError("unexpected channel count: {0}", m_channelCount);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = AudioFormat::Signed16;
					m_frameCount = UInt64(op_pcm_total(m_decoder, -1));
					m_duration = Time::Microseconds(1'000'000LL * m_frameCount / OpusFileSampleRate);

					clearOnError.Reset();

					return Ok();
				}

				Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) override
				{
					if (m_currentFramePosition != startingFrameIndex)
						op_pcm_seek(m_decoder, SafeCaster(startingFrameIndex));

					UInt64 readFrame = ReadOggS16(m_decoder, m_channelCount, static_cast<Int16*>(frameOut), frameCount);
					m_currentFramePosition = SafeCaster(op_pcm_tell(m_decoder));

					return ReadData{ readFrame, m_currentFramePosition };
				}

			private:
				std::mutex m_mutex;
				std::span<const AudioChannel> m_channels;
				std::unique_ptr<Stream> m_ownedStream;
				AudioFormat m_format;
				OggOpusFile* m_decoder;
				Time m_duration;
				UInt32 m_channelCount;
				UInt64 m_currentFramePosition;
				UInt64 m_frameCount;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadOpusSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libopusStream> soundStream = std::make_shared<libopusStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadOpusSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libopusStream> soundStream = std::make_shared<libopusStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadOpusSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libopusStream> soundStream = std::make_shared<libopusStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_libopus()
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsOpusSupported;
			loaderEntry.streamLoader = LoadOpusSoundBuffer;
			loaderEntry.parameterFilter = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinOpusLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_libopus()
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsOpusSupported;
			loaderEntry.fileLoader = LoadOpusSoundStreamFile;
			loaderEntry.memoryLoader = LoadOpusSoundStreamMemory;
			loaderEntry.streamLoader = LoadOpusSoundStreamStream;
			loaderEntry.parameterFilter = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinOpusLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
