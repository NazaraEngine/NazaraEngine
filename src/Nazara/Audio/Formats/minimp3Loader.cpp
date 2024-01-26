// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Formats/minimp3Loader.hpp>
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
#include <optional>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_STDIO
#include <minimp3_ex.h>

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::string MP3ErrorToString(int errCode)
		{
			switch (errCode)
			{
				case 0:              return "no error";
				case MP3D_E_PARAM:   return "wrong parameters";
				case MP3D_E_MEMORY:  return "not enough memory";
				case MP3D_E_IOERROR: return "I/O error";
				case MP3D_E_USER:    return "aborted";
				case MP3D_E_DECODE:  return "decoding error";
				default:             return "unknown error";
			}
		}

		size_t MP3ReadCallback(void* buf, size_t size, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return static_cast<size_t>(stream->Read(buf, size));
		}

		int MP3SeekCallback(uint64_t position, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return (stream->SetCursorPos(position)) ? 0 : MP3D_E_IOERROR;
		}

		bool IsMP3Supported(std::string_view extension)
		{
			return extension == ".mp3";
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadMP3SoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			static_assert(std::is_same_v<mp3d_sample_t, Int16>);

			mp3dec_io_t io;
			io.read = &MP3ReadCallback;
			io.read_data = &stream;
			io.seek = &MP3SeekCallback;
			io.seek_data = &stream;

			struct UserData
			{
				std::vector<Int16> samples;
			};

			UserData userdata;

			mp3dec_t dec;
			mp3dec_file_info_t info;

			Nz::UInt64 cursorPos = stream.GetCursorPos();

			std::unique_ptr<UInt8[]> buffer = std::make_unique<UInt8[]>(MINIMP3_BUF_SIZE);
			if (mp3dec_detect_cb(&io, buffer.get(), MINIMP3_BUF_SIZE) != 0)
				return Err(ResourceLoadingError::Unrecognized);

			stream.SetCursorPos(cursorPos);

			int err = mp3dec_load_cb(&dec, &io, buffer.get(), MINIMP3_BUF_SIZE, &info, nullptr, &userdata);
			if (err != 0)
			{
				NazaraError(MP3ErrorToString(err));
				return Err(ResourceLoadingError::DecodingError);
			}

			CallOnExit freeBuffer([&] { std::free(info.buffer); });

			std::optional<AudioFormat> formatOpt = GuessAudioFormat(info.channels);
			if (!formatOpt)
			{
				NazaraErrorFmt("unexpected channel count: {0}", info.channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			AudioFormat format = *formatOpt;

			UInt64 sampleCount = UInt64(info.samples);

			if (parameters.forceMono && format != AudioFormat::I16_Mono)
			{
				UInt64 frameCount = UInt64(info.samples / info.channels);
				MixToMono(info.buffer, info.buffer, UInt32(info.channels), frameCount);

				format = AudioFormat::I16_Mono;
				sampleCount = frameCount;
			}

			return std::make_shared<SoundBuffer>(format, sampleCount, info.hz, info.buffer);
		}

		class minimp3Stream : public SoundStream
		{
			public:
				minimp3Stream() :
				m_readSampleCount(0)
				{
					std::memset(&m_decoder, 0, sizeof(m_decoder));
				}

				~minimp3Stream()
				{
					mp3dec_ex_close(&m_decoder);
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
					m_io.read = &MP3ReadCallback;
					m_io.read_data = &stream;
					m_io.seek = &MP3SeekCallback;
					m_io.seek_data = &stream;

					Nz::UInt64 cursorPos = stream.GetCursorPos();

					std::unique_ptr<UInt8[]> buffer = std::make_unique<UInt8[]>(MINIMP3_BUF_SIZE);
					if (mp3dec_detect_cb(&m_io, buffer.get(), MINIMP3_BUF_SIZE) != 0)
						return Err(ResourceLoadingError::Unrecognized);

					stream.SetCursorPos(cursorPos);

					int err = mp3dec_ex_open_cb(&m_decoder, &m_io, MP3D_SEEK_TO_SAMPLE);
					if (err != 0)
					{
						NazaraError(MP3ErrorToString(err));
						return Err(ResourceLoadingError::DecodingError);
					}

					CallOnExit resetOnError([this]
					{
						mp3dec_ex_close(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::optional<AudioFormat> formatOpt = GuessAudioFormat(m_decoder.info.channels);
					if (!formatOpt)
					{
						NazaraErrorFmt("unexpected channel count: {0}", m_decoder.info.channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = *formatOpt;

					m_duration = Time::Microseconds(1'000'000LL * m_decoder.samples / (m_decoder.info.hz * m_decoder.info.channels));
					m_sampleCount = m_decoder.samples;
					m_sampleRate = m_decoder.info.hz;

					// Mixing to mono will be done on the fly
					if (parameters.forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = static_cast<UInt32>(m_decoder.samples / m_decoder.info.channels);
					}
					else
						m_mixToMono = false;

					resetOnError.Reset();

					return Ok();
				}

				UInt64 Read(void* buffer, UInt64 sampleCount) override
				{
					// Convert to mono in the fly if necessary
					if (m_mixToMono)
					{
						UInt32 channelCount = GetChannelCount(m_format);

						// Keep a buffer to the side to prevent allocation
						m_mixBuffer.resize(channelCount * sampleCount);
						std::size_t readSample = mp3dec_ex_read(&m_decoder, m_mixBuffer.data(), channelCount * sampleCount);
						m_readSampleCount += readSample;

						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), channelCount, sampleCount);

						return readSample / channelCount;
					}
					else
					{
						UInt64 readSample = mp3dec_ex_read(&m_decoder, static_cast<Int16*>(buffer), sampleCount);
						m_readSampleCount += readSample;

						return readSample;
					}
				}

				void Seek(UInt64 offset) override
				{
					mp3dec_ex_seek(&m_decoder, offset);
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
				AudioFormat m_format;
				mp3dec_ex_t m_decoder;
				mp3dec_io_t m_io;
				Time m_duration;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMP3SoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMP3SoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMP3SoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_minimp3()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsMP3Supported;
			loaderEntry.streamLoader = LoadMP3SoundBuffer;
			loaderEntry.parameterFilter = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMP3Loader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_minimp3()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsMP3Supported;
			loaderEntry.fileLoader = LoadMP3SoundStreamFile;
			loaderEntry.memoryLoader = LoadMP3SoundStreamMemory;
			loaderEntry.streamLoader = LoadMP3SoundStreamStream;
			loaderEntry.parameterFilter = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMP3Loader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
