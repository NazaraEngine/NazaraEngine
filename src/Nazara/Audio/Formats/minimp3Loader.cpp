// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Formats/minimp3Loader.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <optional>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_STDIO
#include <minimp3_ex.h>

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

		std::string ErrToString(int errCode)
		{
			switch (errCode)
			{
				case 0: return "no error";
				case MP3D_E_PARAM: return "wrong parameters";
				case MP3D_E_MEMORY: return "not enough memory";
				case MP3D_E_IOERROR: return "I/O error";
				case MP3D_E_USER: return "aborted";
				case MP3D_E_DECODE: return "decoding error";
				default: return "unknown error";
			}
		}

		size_t ReadCallback(void* buf, size_t size, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return static_cast<size_t>(stream->Read(buf, size));
		}

		int SeekCallback(uint64_t position, void* user_data)
		{
			Stream* stream = static_cast<Stream*>(user_data);
			return (stream->SetCursorPos(position)) ? 0 : MP3D_E_IOERROR;
		}

		bool IsSupported(const std::string_view& extension)
		{
			return extension == "mp3";
		}

		Ternary CheckMp3(Stream& stream)
		{
			mp3dec_io_t io;
			io.read = &ReadCallback;
			io.read_data = &stream;
			io.seek = &SeekCallback;
			io.seek_data = &stream;

			std::vector<UInt8> buffer(MINIMP3_BUF_SIZE);
			return (mp3dec_detect_cb(&io, buffer.data(), buffer.size()) == 0) ? Ternary::True : Ternary::False;
		}

		std::shared_ptr<SoundBuffer> LoadSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			static_assert(std::is_same_v<mp3d_sample_t, Int16>);

			mp3dec_io_t io;
			io.read = &ReadCallback;
			io.read_data = &stream;
			io.seek = &SeekCallback;
			io.seek_data = &stream;

			struct UserData
			{
				std::vector<Int16> samples;
			};

			UserData userdata;

			mp3dec_t dec;
			mp3dec_file_info_t info;
			std::vector<UInt8> buffer(MINIMP3_BUF_SIZE);
			int err = mp3dec_load_cb(&dec, &io, buffer.data(), buffer.size(), &info, nullptr, &userdata);
			if (err != 0)
			{
				NazaraError(ErrToString(err));
				return {};
			}

			CallOnExit freeBuffer([&] { std::free(info.buffer); });

			std::optional<AudioFormat> formatOpt = GuessFormat(info.channels);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: " + std::to_string(info.channels));
				return {};
			}

			AudioFormat format = *formatOpt;

			UInt32 sampleCount = static_cast<UInt32>(info.samples);

			if (parameters.forceMono && format != AudioFormat::U16_Mono)
			{
				UInt64 frameCount = UInt64(info.samples / info.channels);
				MixToMono(info.buffer, info.buffer, static_cast<UInt32>(info.channels), frameCount);

				format = AudioFormat::U16_Mono;
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

				UInt32 GetDuration() const override
				{
					return m_duration;
				}

				AudioFormat GetFormat() const override
				{
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
					m_io.read = &ReadCallback;
					m_io.read_data = &stream;
					m_io.seek = &SeekCallback;
					m_io.seek_data = &stream;

					int err = mp3dec_ex_open_cb(&m_decoder, &m_io, MP3D_SEEK_TO_SAMPLE);
					if (err != 0)
					{
						NazaraError(ErrToString(err));
						return {};
					}

					CallOnExit resetOnError([this]
					{
						mp3dec_ex_close(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::optional<AudioFormat> formatOpt = GuessFormat(m_decoder.info.channels);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: " + std::to_string(m_decoder.info.channels));
						return false;
					}

					m_format = *formatOpt;

					m_duration = static_cast<UInt32>(1000ULL * m_decoder.samples / (m_decoder.info.hz * m_decoder.info.channels));
					m_sampleCount = m_decoder.samples;
					m_sampleRate = m_decoder.info.hz;

					// Mixing to mono will be done on the fly
					if (forceMono && m_format != AudioFormat::U16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = static_cast<UInt32>(m_decoder.samples / m_decoder.info.channels);
					}
					else
						m_mixToMono = false;

					resetOnError.Reset();

					return true;
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
				UInt32 m_duration;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		std::shared_ptr<SoundStream> LoadSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
			if (!soundStream->Open(filePath, parameters.forceMono))
			{
				NazaraError("failed to open sound stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
			if (!soundStream->Open(data, size, parameters.forceMono))
			{
				NazaraError("failed to open music stream");
				return {};
			}

			return soundStream;
		}

		std::shared_ptr<SoundStream> LoadSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<minimp3Stream> soundStream = std::make_shared<minimp3Stream>();
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
		SoundBufferLoader::Entry GetSoundBufferLoader_minimp3()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundBufferParams&) { return CheckMp3(stream); };
			loaderEntry.streamLoader = LoadSoundBuffer;

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_minimp3()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSupported;
			loaderEntry.streamChecker = [](Stream& stream, const SoundStreamParams&) { return CheckMp3(stream); };
			loaderEntry.fileLoader = LoadSoundStreamFile;
			loaderEntry.memoryLoader = LoadSoundStreamMemory;
			loaderEntry.streamLoader = LoadSoundStreamStream;

			return loaderEntry;
		}
	}
}
