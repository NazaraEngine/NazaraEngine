// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadFlacSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
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
			UInt64 frameCount = 0;
			UInt64 sampleCount = 0;
			UInt32 sampleRate = 0;

			ud.metadataCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__StreamMetadata* meta)
			{
				if (meta->type == FLAC__METADATA_TYPE_STREAMINFO)
				{
					channelCount = meta->data.stream_info.channels;
					frameCount = meta->data.stream_info.total_samples;
					sampleCount = frameCount * channelCount;
					sampleRate = meta->data.stream_info.sample_rate;

					samples = std::make_unique_for_overwrite<Int16[]>(channelCount * frameCount);
				}
			};

			UInt64 sampleIndex = 0;
			ud.writeCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__Frame* frame, const FLAC__int32* const buffer[])
			{
				std::size_t frameSampleCount = frame->header.blocksize * frame->header.channels;
				if (sampleIndex + frameSampleCount > sampleCount)
				{
					NazaraError("too many sample encountered");
					return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
				}

				if (!DecodeFlacFrameSamples(frame, buffer, samples.get() + sampleIndex))
				{
					NazaraError("failed to decode samples");
					return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
				}

				sampleIndex += frameSampleCount;

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

			if (channelCount == 0 || frameCount == 0 || sampleCount == 0 || sampleRate == 0)
			{
				NazaraError("invalid metadata");
				return Err(ResourceLoadingError::DecodingError);
			}

			std::optional<AudioFormat> formatOpt = GuessAudioFormat(channelCount);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: {0}", channelCount);
				return Err(ResourceLoadingError::Unsupported);
			}

			AudioFormat format = *formatOpt;

			if (parameters.forceMono && format != AudioFormat::I16_Mono)
			{
				MixToMono(samples.get(), samples.get(), channelCount, frameCount);

				format = AudioFormat::I16_Mono;
				sampleCount = frameCount;
			}

			return std::make_shared<SoundBuffer>(format, sampleCount, sampleRate, samples.get());
		}

		class libflacStream : public SoundStream
		{
			public:
				libflacStream() :
				m_decoder(nullptr),
				m_readSampleCount(0),
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

				Result<void, ResourceLoadingError> Open(Stream& stream, const SoundStreamParams& parameters)
				{
					m_userData.stream = &stream;
					m_userData.errorCallback = [this](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus status)
					{
						m_errored = true;
						NazaraError("{}", FLAC__StreamDecoderErrorStatusString[status]);
					};

					FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
					CallOnExit freeDecoder([&] { FLAC__stream_decoder_delete(decoder); });

					UInt64 frameCount;

					m_userData.metadataCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__StreamMetadata* meta)
					{
						m_channelCount = meta->data.stream_info.channels;
						frameCount = meta->data.stream_info.total_samples;
						m_sampleCount = frameCount * m_channelCount;
						m_sampleRate = meta->data.stream_info.sample_rate;

						m_duration = Time::Microseconds(1'000'000LL * frameCount / m_sampleRate);
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

					std::optional<AudioFormat> formatOpt = GuessAudioFormat(m_channelCount);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: {0}", m_channelCount);
						return Err(ResourceLoadingError::Unrecognized);
					}

					m_format = *formatOpt;

					// Mixing to mono will be done on the fly
					if (parameters.forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = frameCount;
					}
					else
						m_mixToMono = false;

					finishDecoder.Reset();
					freeDecoder.Reset();
					m_decoder = decoder;

					return Ok();
				}

				UInt64 Read(void* buffer, UInt64 sampleCount) override
				{
					// Convert to mono in the fly if necessary
					if (m_mixToMono)
					{
						// Keep a buffer to the side to prevent allocation
						m_mixBuffer.resize(sampleCount * m_channelCount);

						std::size_t readSample = ReadFlac(m_mixBuffer.data(), sampleCount * m_channelCount);
						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), m_channelCount, sampleCount);

						return readSample / m_channelCount;
					}
					else
						return ReadFlac(static_cast<Int16*>(buffer), sampleCount);
				}

				UInt64 ReadFlac(Int16* buffer, UInt64 sampleCount)
				{
					// Read overflown buffer first
					UInt64 readSample = std::min<UInt64>(m_overflowBuffer.size(), sampleCount);
					if (readSample > 0)
					{
						std::memcpy(buffer, m_overflowBuffer.data(), readSample * sizeof(Int16));
						m_overflowBuffer.erase(m_overflowBuffer.begin(), m_overflowBuffer.begin() + readSample);
						sampleCount -= readSample;
					}

					if (sampleCount == 0)
						return readSample;

					m_userData.writeCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__Frame* frame, const FLAC__int32* const framebuffer[])
					{
						UInt32 frameCount = frame->header.blocksize;
						UInt32 blockSampleCount = frameCount * frame->header.channels;
						if (blockSampleCount > sampleCount)
						{
							std::size_t overflownOffset = m_overflowBuffer.size();
							m_overflowBuffer.resize(overflownOffset + blockSampleCount - sampleCount);

							if (sampleCount > 0)
							{
								assert(sampleCount % frame->header.channels == 0);
								if (!DecodeFlacFrameSamples(frame, framebuffer, buffer + readSample, 0, static_cast<UInt32>(sampleCount / frame->header.channels)))
									return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

								readSample += sampleCount;
							}

							if (!DecodeFlacFrameSamples(frame, framebuffer, &m_overflowBuffer[overflownOffset], static_cast<UInt32>(sampleCount / frame->header.channels), frameCount))
								return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

							sampleCount = 0;
						}
						else
						{
							if (!DecodeFlacFrameSamples(frame, framebuffer, buffer + readSample))
								return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

							readSample += blockSampleCount;
							sampleCount -= blockSampleCount;
						}

						if (m_mixToMono)
							m_readSampleCount += frameCount;
						else
							m_readSampleCount += blockSampleCount;

						return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
					};

					CallOnExit resetWriteCallback([&] { m_userData.writeCallback = nullptr; });

					while (sampleCount > 0)
					{
						if (!FLAC__stream_decoder_process_single(m_decoder))
							break; //< an error occurred

						if (FLAC__stream_decoder_get_state(m_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
							break; //< we hit the end of the stream
					}

					return readSample;
				}

				void Seek(UInt64 offset) override
				{
					FLAC__stream_decoder_seek_absolute(m_decoder, (m_mixToMono) ? offset : offset / m_channelCount);
					m_readSampleCount = offset;
				}

				UInt64 Tell() override
				{
					return m_readSampleCount;
				}

			private:
				std::mutex m_mutex;
				std::unique_ptr<Stream> m_ownedStream;
				std::vector<Int16> m_mixBuffer;
				std::vector<Int16> m_overflowBuffer;
				FLAC__StreamDecoder* m_decoder;
				AudioFormat m_format;
				FlacUserdata m_userData;
				Time m_duration;
				UInt32 m_channelCount;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_errored;
				bool m_mixToMono;
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
