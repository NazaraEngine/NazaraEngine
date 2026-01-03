// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/libflacLoader.hpp>
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
#include <FLAC/stream_decoder.h>
#include <optional>
#include <set>

namespace Nz
{
	namespace
	{
		struct FlacUserdata
		{
			std::function<void(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status)> errorCallback;
			std::function<void(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata)> metadataCallback;
			std::function<FLAC__StreamDecoderWriteStatus(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[])> writeCallback;
			Stream* stream;
		};

		FLAC__bool FlacEofCallback(const FLAC__StreamDecoder* /*decoder*/, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			return ud->stream->EndOfStream();
		}

		void ErrorCallback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			assert(ud->errorCallback);
			return ud->errorCallback(decoder, status);
		}

		FLAC__StreamDecoderLengthStatus FlacLengthCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64* stream_length, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			*stream_length = ud->stream->GetSize();

			return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
		}

		void MetadataCallback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			if (ud->metadataCallback)
				ud->metadataCallback(decoder, metadata);
		}

		FLAC__StreamDecoderReadStatus FlacReadCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__byte buffer[], size_t* bytes, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			std::size_t readBytes = ud->stream->Read(buffer, *bytes);

			*bytes = readBytes;
			if (readBytes == 0 && ud->stream->EndOfStream())
				return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
			else
				return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		}

		FLAC__StreamDecoderSeekStatus FlacSeekCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64 absolute_byte_offset, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			if (ud->stream->SetCursorPos(absolute_byte_offset))
				return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
			else
				return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
		}

		FLAC__StreamDecoderTellStatus FlacTellCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64* absolute_byte_offset, void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			*absolute_byte_offset = ud->stream->GetCursorPos();
			return FLAC__STREAM_DECODER_TELL_STATUS_OK;
		}

		FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* client_data)
		{
			FlacUserdata* ud = static_cast<FlacUserdata*>(client_data);
			if (ud->writeCallback)
				return ud->writeCallback(decoder, frame, buffer);
			else
				return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
		}

		template<UInt32 Bits, typename TargetType>
		bool DecodeFlacFrameSamplesImpl(const FLAC__Frame* frame, const FLAC__int32* const buffer[], TargetType* samples, UInt32 frameIndex, UInt32 frameCount)
		{
			constexpr UInt32 TargetBits = sizeof(TargetType) * CHAR_BIT;
			for (; frameIndex < frameCount; ++frameIndex)
			{
				for (UInt32 channelIndex = 0; channelIndex < frame->header.channels; ++channelIndex)
				{
					Int32 sample = buffer[channelIndex][frameIndex];
					if constexpr (Bits > TargetBits)
						sample >>= (Bits - TargetBits);
					else
						sample <<= (TargetBits - Bits);

					*samples++ = static_cast<TargetType>(sample);
				}
			}

			return true;
		}

		bool DecodeFlacFrameSamples(const FLAC__Frame* frame, const FLAC__int32* const buffer[], Int16* samples, UInt32 frameIndex, UInt32 frameCount)
		{
			switch (frame->header.bits_per_sample)
			{
				case 8:  return DecodeFlacFrameSamplesImpl<8>(frame, buffer, samples, frameIndex, frameCount);
				case 12: return DecodeFlacFrameSamplesImpl<12>(frame, buffer, samples, frameIndex, frameCount);
				case 16: return DecodeFlacFrameSamplesImpl<16>(frame, buffer, samples, frameIndex, frameCount);
				case 20: return DecodeFlacFrameSamplesImpl<20>(frame, buffer, samples, frameIndex, frameCount);
				case 24: return DecodeFlacFrameSamplesImpl<24>(frame, buffer, samples, frameIndex, frameCount);
				case 32: return DecodeFlacFrameSamplesImpl<32>(frame, buffer, samples, frameIndex, frameCount);
				default: return false;
			}
		}

		bool DecodeFlacFrameSamples(const FLAC__Frame* frame, const FLAC__int32* const buffer[], Int16* samples)
		{
			return DecodeFlacFrameSamples(frame, buffer, samples, 0, frame->header.blocksize);
		}

		bool IsFlacSupported(std::string_view extension)
		{
			return extension == ".flac";
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadFlacSoundBuffer(Stream& stream, const SoundBufferParams& /*parameters*/)
		{
			FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
			CallOnExit freeDecoder([&] { FLAC__stream_decoder_delete(decoder); });

			bool hasError = false;

			FlacUserdata ud;
			ud.stream = &stream;
			ud.errorCallback = [&](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus status)
			{
				hasError = true;
				NazaraError("FLAC error: {}", FLAC__StreamDecoderErrorStatusString[status]);
			};

			std::unique_ptr<Int16[]> samples;
			UInt32 channelCount = 0;
			UInt64 totalFrameCount = 0;
			UInt32 sampleRate = 0;

			ud.metadataCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__StreamMetadata* meta)
			{
				if (meta->type == FLAC__METADATA_TYPE_STREAMINFO)
				{
					channelCount = meta->data.stream_info.channels;
					totalFrameCount = meta->data.stream_info.total_samples;
					sampleRate = meta->data.stream_info.sample_rate;

					samples = std::make_unique_for_overwrite<Int16[]>(channelCount * totalFrameCount);
				}
			};

			UInt64 frameIndex = 0;
			ud.writeCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__Frame* frame, const FLAC__int32* const buffer[])
			{
				std::size_t frameCount = frame->header.blocksize;
				if (frameIndex + frameCount > totalFrameCount)
				{
					NazaraError("too many sample encountered");
					return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
				}

				if (!DecodeFlacFrameSamples(frame, buffer, samples.get() + frameIndex * channelCount))
				{
					NazaraError("failed to decode samples");
					return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
				}

				frameIndex += frameCount;

				return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
			};

			FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(decoder, &FlacReadCallback, &FlacSeekCallback, &FlacTellCallback, &FlacLengthCallback, &FlacEofCallback, &WriteCallback, &MetadataCallback, &ErrorCallback, &ud);
			if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
			{
				NazaraWarning("{}", FLAC__StreamDecoderInitStatusString[status]); //< an error shouldn't happen at this state
				return Err(ResourceLoadingError::Internal);
			}

			CallOnExit finishDecoder([&] { FLAC__stream_decoder_finish(decoder); });

			if (!FLAC__stream_decoder_process_until_end_of_metadata(decoder))
				return Err(ResourceLoadingError::Unrecognized);

			if (!FLAC__stream_decoder_process_until_end_of_stream(decoder))
			{
				NazaraError("flac decoding failed");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (hasError)
			{
				NazaraError("an error occurred during decoding");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (channelCount == 0 || totalFrameCount == 0 || sampleRate == 0)
			{
				NazaraError("invalid metadata");
				return Err(ResourceLoadingError::DecodingError);
			}

			std::span<const AudioChannel> channels = GetAudioChannelMap(channelCount);
			if (channels.empty())
			{
				NazaraError("unexpected channel count: {0}", channelCount);
				return Err(ResourceLoadingError::Unsupported);
			}

			return std::make_shared<SoundBuffer>(AudioFormat::Signed16, channels, totalFrameCount, sampleRate, samples.get());
		}

		class libflacStream : public SoundStream
		{
			public:
				libflacStream() :
				m_decoder(nullptr),
				m_currentFramePosition(0),
				m_errored(false)
				{
				}

				~libflacStream()
				{
					if (m_decoder)
					{
						FLAC__stream_decoder_finish(m_decoder);
						FLAC__stream_decoder_delete(m_decoder);
					}
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
					m_userData.stream = &stream;
					m_userData.errorCallback = [this](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus status)
					{
						m_errored = true;
						NazaraError("{}", FLAC__StreamDecoderErrorStatusString[status]);
					};

					FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
					CallOnExit freeDecoder([&] { FLAC__stream_decoder_delete(decoder); });

					m_userData.metadataCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__StreamMetadata* meta)
					{
						m_channelCount = meta->data.stream_info.channels;
						m_frameCount = meta->data.stream_info.total_samples;
						m_sampleRate = meta->data.stream_info.sample_rate;

						m_duration = Time::Microseconds(1'000'000LL * m_frameCount / m_sampleRate);
					};

					FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(decoder, &FlacReadCallback, &FlacSeekCallback, &FlacTellCallback, &FlacLengthCallback, &FlacEofCallback, &WriteCallback, &MetadataCallback, &ErrorCallback, &m_userData);
					if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
					{
						NazaraWarning("{}", FLAC__StreamDecoderInitStatusString[status]); //< an error shouldn't happen at this state
						return Err(ResourceLoadingError::Internal);
					}

					CallOnExit finishDecoder([&] { FLAC__stream_decoder_finish(decoder); });

					if (!FLAC__stream_decoder_process_until_end_of_metadata(decoder))
						return Err(ResourceLoadingError::Unrecognized);

					m_channels = GetAudioChannelMap(m_channelCount);
					if (m_channels.empty())
					{
						NazaraError("unexpected channel count: {0}", m_channelCount);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = AudioFormat::Signed16;

					finishDecoder.Reset();
					freeDecoder.Reset();
					m_decoder = decoder;

					return Ok();
				}

				Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) override
				{
					Int16* output = static_cast<Int16*>(frameOut);

					UInt64 readFrame = 0;
					if (m_currentFramePosition != startingFrameIndex)
					{
						FLAC__stream_decoder_seek_absolute(m_decoder, startingFrameIndex);
						m_currentFramePosition = startingFrameIndex;
						m_overflowBuffer.clear();
					}
					else
					{
						// Read overflown buffer first
						readFrame = std::min<UInt64>(m_overflowBuffer.size() / m_channelCount, frameCount);
						if (readFrame > 0)
						{
							std::memcpy(output, m_overflowBuffer.data(), readFrame * m_channelCount * sizeof(Int16));
							m_overflowBuffer.erase(m_overflowBuffer.begin(), m_overflowBuffer.begin() + readFrame * m_channelCount);
							frameCount -= readFrame;
							output += readFrame * m_channelCount;
						}

						if (frameCount == 0)
							return ReadData{ readFrame, m_currentFramePosition };
					}

					m_userData.writeCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__Frame* frame, const FLAC__int32* const framebuffer[])
					{
						UInt32 blockFrameCount = frame->header.blocksize;
						if (blockFrameCount > frameCount)
						{
							std::size_t overflownOffset = m_overflowBuffer.size();
							m_overflowBuffer.resize(overflownOffset + (blockFrameCount - frameCount) * m_channelCount);

							if (frameCount > 0)
							{
								if (!DecodeFlacFrameSamples(frame, framebuffer, output, 0, static_cast<UInt32>(frameCount)))
									return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

								readFrame += frameCount;
							}

							if (!DecodeFlacFrameSamples(frame, framebuffer, &m_overflowBuffer[overflownOffset], static_cast<UInt32>(frameCount), blockFrameCount))
								return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

							frameCount = 0;
						}
						else
						{
							if (!DecodeFlacFrameSamples(frame, framebuffer, output + readFrame * m_channelCount))
								return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

							readFrame += blockFrameCount;
							frameCount -= blockFrameCount;
						}

						m_currentFramePosition += blockFrameCount;

						return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
					};

					NAZARA_DEFER({ m_userData.writeCallback = nullptr; });

					while (frameCount > 0)
					{
						if (!FLAC__stream_decoder_process_single(m_decoder))
							break; //< an error occurred

						if (FLAC__stream_decoder_get_state(m_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
							break; //< we hit the end of the stream
					}

					return ReadData{ readFrame, m_currentFramePosition };
				}

			private:
				std::mutex m_mutex;
				std::span<const AudioChannel> m_channels;
				std::unique_ptr<Stream> m_ownedStream;
				std::vector<Int16> m_overflowBuffer;
				FLAC__StreamDecoder* m_decoder;
				AudioFormat m_format;
				FlacUserdata m_userData;
				Time m_duration;
				UInt32 m_channelCount;
				UInt32 m_sampleRate;
				UInt64 m_currentFramePosition;
				UInt64 m_frameCount;
				bool m_errored;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadFlacSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadFlacSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadFlacSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_libflac()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsFlacSupported;
			loaderEntry.streamLoader     = LoadFlacSoundBuffer;
			loaderEntry.parameterFilter  = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinFlacLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_libflac()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsFlacSupported;
			loaderEntry.fileLoader       = LoadFlacSoundStreamFile;
			loaderEntry.memoryLoader     = LoadFlacSoundStreamMemory;
			loaderEntry.streamLoader     = LoadFlacSoundStreamStream;
			loaderEntry.parameterFilter  = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinFlacLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
