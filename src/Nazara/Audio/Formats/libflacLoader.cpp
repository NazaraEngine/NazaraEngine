// Copyright (C) 2020 Jérôme Leclercq
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
#include <FLAC/stream_decoder.h>
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


		struct Userdata
		{
			std::function<void(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status)> errorCallback;
			std::function<void(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata)> metadataCallback;
			std::function<FLAC__StreamDecoderWriteStatus(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[])> writeCallback;
			Stream* stream;
		};

		FLAC__bool EofCallback(const FLAC__StreamDecoder* /*decoder*/, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			return ud->stream->EndOfStream();
		}

		void ErrorCallback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			assert(ud->errorCallback);
			return ud->errorCallback(decoder, status);
		}

		FLAC__StreamDecoderLengthStatus LengthCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64* stream_length, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			*stream_length = ud->stream->GetSize();

			return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
		}

		void MetadataCallback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			if (ud->metadataCallback)
				ud->metadataCallback(decoder, metadata);
		}

		FLAC__StreamDecoderReadStatus ReadCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__byte buffer[], size_t* bytes, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			std::size_t readBytes = ud->stream->Read(buffer, *bytes);

			*bytes = readBytes;
			if (readBytes == 0 && ud->stream->EndOfStream())
				return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
			else
				return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		}

		FLAC__StreamDecoderSeekStatus SeekCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64 absolute_byte_offset, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			if (ud->stream->SetCursorPos(absolute_byte_offset))
				return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
			else
				return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
		}

		FLAC__StreamDecoderTellStatus TellCallback(const FLAC__StreamDecoder* /*decoder*/, FLAC__uint64* absolute_byte_offset, void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			*absolute_byte_offset = ud->stream->GetCursorPos();
			return FLAC__STREAM_DECODER_TELL_STATUS_OK;
		}

		FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* client_data)
		{
			Userdata* ud = static_cast<Userdata*>(client_data);
			if (ud->writeCallback)
				return ud->writeCallback(decoder, frame, buffer);
			else
				return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
		}

		template<UInt32 Bits, typename TargetType>
		bool DecodeFrameSamplesImpl(const FLAC__Frame* frame, const FLAC__int32* const buffer[], TargetType* samples, UInt32 frameIndex, UInt32 frameCount)
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

		bool DecodeFrameSamples(const FLAC__Frame* frame, const FLAC__int32* const buffer[], Int16* samples, UInt32 frameIndex, UInt32 frameCount)
		{
			switch (frame->header.bits_per_sample)
			{
				case 8:  return DecodeFrameSamplesImpl<8>(frame, buffer, samples, frameIndex, frameCount);
				case 12: return DecodeFrameSamplesImpl<12>(frame, buffer, samples, frameIndex, frameCount);
				case 16: return DecodeFrameSamplesImpl<16>(frame, buffer, samples, frameIndex, frameCount);
				case 20: return DecodeFrameSamplesImpl<20>(frame, buffer, samples, frameIndex, frameCount);
				case 24: return DecodeFrameSamplesImpl<24>(frame, buffer, samples, frameIndex, frameCount);
				case 32: return DecodeFrameSamplesImpl<32>(frame, buffer, samples, frameIndex, frameCount);
				default: return false;
			}
		}

		bool DecodeFrameSamples(const FLAC__Frame* frame, const FLAC__int32* const buffer[], Int16* samples)
		{
			return DecodeFrameSamples(frame, buffer, samples, 0, frame->header.blocksize);
		}

		bool IsSupported(const std::string_view& extension)
		{
			return extension == "flac";
		}

		Ternary CheckFlac(Stream& stream, const ResourceParameters& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeFlacLoader", &skip) && skip)
				return Ternary::False;

			FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
			CallOnExit freeDecoder([&] { FLAC__stream_decoder_delete(decoder); });

			bool hasError = false;

			Userdata ud;
			ud.stream = &stream;
			ud.errorCallback = [&](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus /*status*/)
			{
				hasError = true;
			};

			FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(decoder, &ReadCallback, &SeekCallback, &TellCallback, &LengthCallback, &EofCallback, &WriteCallback, &MetadataCallback, &ErrorCallback, &ud);
			if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
			{
				NazaraWarning(FLAC__StreamDecoderInitStatusString[status]); //< an error shouldn't happen at this state
				return Ternary::False;
			}

			CallOnExit finishDecoder([&] { FLAC__stream_decoder_finish(decoder); });

			if (!FLAC__stream_decoder_process_until_end_of_metadata(decoder))
				return Ternary::False;

			if (hasError)
				return Ternary::False;

			return Ternary::True;
		}

		std::shared_ptr<SoundBuffer> LoadSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
			CallOnExit freeDecoder([&] { FLAC__stream_decoder_delete(decoder); });

			bool hasError = false;

			Userdata ud;
			ud.stream = &stream;
			ud.errorCallback = [&](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus status)
			{
				hasError = true;
				NazaraError(FLAC__StreamDecoderErrorStatusString[status]);
			};

			std::unique_ptr<Int16[]> samples;
			UInt32 channelCount = 0;
			UInt64 frameCount = 0;
			UInt64 sampleCount = 0;
			UInt64 sampleRate = 0;

			ud.metadataCallback = [&](const FLAC__StreamDecoder* /*decoder*/, const FLAC__StreamMetadata* meta)
			{
				if (meta->type == FLAC__METADATA_TYPE_STREAMINFO)
				{
					channelCount = meta->data.stream_info.channels;
					frameCount = meta->data.stream_info.total_samples;
					sampleCount = frameCount * channelCount;
					sampleRate = meta->data.stream_info.sample_rate;

					samples = std::make_unique<Int16[]>(channelCount * frameCount);
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

				if (!DecodeFrameSamples(frame, buffer, samples.get() + sampleIndex))
				{
					NazaraError("failed to decode samples");
					return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
				}

				sampleIndex += frameSampleCount;

				return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
			};

			FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(decoder, &ReadCallback, &SeekCallback, &TellCallback, &LengthCallback, &EofCallback, &WriteCallback, &MetadataCallback, &ErrorCallback, &ud);
			if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
			{
				NazaraWarning(FLAC__StreamDecoderInitStatusString[status]); //< an error shouldn't happen at this state
				return {};
			}

			CallOnExit finishDecoder([&] { FLAC__stream_decoder_finish(decoder); });

			if (!FLAC__stream_decoder_process_until_end_of_stream(decoder))
			{
				NazaraError("flac decoding failed");
				return {};
			}

			if (hasError)
			{
				NazaraError("an error occurred during decoding");
				return {};
			}

			if (channelCount == 0 || frameCount == 0 || sampleCount == 0 || sampleRate == 0)
			{
				NazaraError("invalid metadata");
				return {};
			}

			std::optional<AudioFormat> formatOpt = GuessFormat(channelCount);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: " + std::to_string(channelCount));
				return {};
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
					m_userData.stream = &stream;
					m_userData.errorCallback = [this](const FLAC__StreamDecoder* /*decoder*/, FLAC__StreamDecoderErrorStatus status)
					{
						m_errored = true;
						NazaraError(FLAC__StreamDecoderErrorStatusString[status]);
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

						m_duration = UInt32(1000ULL * frameCount / m_sampleRate);
					};

					FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(decoder, &ReadCallback, &SeekCallback, &TellCallback, &LengthCallback, &EofCallback, &WriteCallback, &MetadataCallback, &ErrorCallback, &m_userData);
					if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
					{
						NazaraWarning(FLAC__StreamDecoderInitStatusString[status]); //< an error shouldn't happen at this state
						return {};
					}

					CallOnExit finishDecoder([&] { FLAC__stream_decoder_finish(decoder); });

					if (!FLAC__stream_decoder_process_until_end_of_metadata(decoder))
					{
						NazaraError("failed to decode metadata");
						return false;
					}

					std::optional<AudioFormat> formatOpt = GuessFormat(m_channelCount);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: " + std::to_string(m_channelCount));
						return false;
					}

					m_format = *formatOpt;

					// Mixing to mono will be done on the fly
					if (forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = frameCount;
					}
					else
						m_mixToMono = false;

					finishDecoder.Reset();
					freeDecoder.Reset();
					m_decoder = decoder;

					return true;
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
								if (!DecodeFrameSamples(frame, framebuffer, buffer + readSample, 0, static_cast<UInt32>(sampleCount / frame->header.channels)))
									return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

								readSample += sampleCount;
							}

							if (!DecodeFrameSamples(frame, framebuffer, &m_overflowBuffer[overflownOffset], static_cast<UInt32>(sampleCount / frame->header.channels), frameCount))
								return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

							sampleCount = 0;
						}
						else
						{
							if (!DecodeFrameSamples(frame, framebuffer, buffer + readSample))
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
				Userdata m_userData;
				UInt32 m_channelCount;
				UInt32 m_duration;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_errored;
				bool m_mixToMono;
		};

		std::shared_ptr<SoundStream> LoadSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
			if (!soundStream->Open(filePath, parameters.forceMono))
			{
				NazaraError("failed to open sound stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
			if (!soundStream->Open(data, size, parameters.forceMono))
			{
				NazaraError("failed to open music stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<libflacStream> soundStream = std::make_shared<libflacStream>();
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
		SoundBufferLoader::Entry GetSoundBufferLoader_libflac()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundBufferParams& parameters) { return CheckFlac(stream, parameters); };
			loaderEntry.streamLoader = LoadSoundBuffer;

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_libflac()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundStreamParams& parameters) { return CheckFlac(stream, parameters); };
			loaderEntry.fileLoader = LoadSoundStreamFile;
			loaderEntry.memoryLoader = LoadSoundStreamMemory;
			loaderEntry.streamLoader = LoadSoundStreamStream;

			return loaderEntry;
		}
	}
}

