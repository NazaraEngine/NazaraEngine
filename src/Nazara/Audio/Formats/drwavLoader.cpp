// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/drwavLoader.hpp>
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
#include <optional>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#include <dr_wav.h>


namespace Nz
{
	namespace
	{
		std::size_t ReadWavCallback(void* pUserData, void* pBufferOut, size_t bytesToRead)
		{
			Stream* stream = static_cast<Stream*>(pUserData);
			return static_cast<std::size_t>(stream->Read(pBufferOut, bytesToRead));
		}

		drwav_bool32 SeekWavCallback(void* pUserData, int offset, drwav_seek_origin origin)
		{
			Stream* stream = static_cast<Stream*>(pUserData);
			switch (origin)
			{
				case drwav_seek_origin_start:
					return stream->SetCursorPos(offset);

				case drwav_seek_origin_current:
					return (stream->Read(nullptr, static_cast<std::size_t>(offset)) != 0);

				default:
					NazaraInternalError("Seek mode not handled");
					return false;
			}
		}

		bool IsWavSupported(std::string_view extension)
		{
			return extension == ".riff" || extension == ".rf64" || extension == ".wav" || extension == ".w64";
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadWavSoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			drwav wav;
			if (!drwav_init(&wav, &ReadWavCallback, &SeekWavCallback, &stream, nullptr))
				return Err(ResourceLoadingError::Unrecognized);

			CallOnExit uninitOnExit([&] { drwav_uninit(&wav); });

			std::optional<AudioFormat> formatOpt = GuessAudioFormat(wav.channels);
			if (!formatOpt)
			{
				NazaraErrorFmt("unexpected channel count: {0}", wav.channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			AudioFormat format = *formatOpt;

			UInt64 sampleCount = wav.totalPCMFrameCount * wav.channels;
			std::unique_ptr<Int16[]> samples = std::make_unique<Int16[]>(sampleCount); //< std::vector would default-init to zero

			if (drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, samples.get()) != wav.totalPCMFrameCount)
			{
				NazaraError("failed to read stream content");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (parameters.forceMono && format != AudioFormat::I16_Mono)
			{
				MixToMono(samples.get(), samples.get(), static_cast<UInt32>(wav.channels), wav.totalPCMFrameCount);

				format = AudioFormat::I16_Mono;
				sampleCount = wav.totalPCMFrameCount;
			}

			return std::make_shared<SoundBuffer>(format, sampleCount, wav.sampleRate, samples.get());
		}

		class drwavStream : public SoundStream
		{
			public:
				drwavStream() :
				m_readSampleCount(0)
				{
					std::memset(&m_decoder, 0, sizeof(m_decoder));
				}

				~drwavStream()
				{
					drwav_uninit(&m_decoder);
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
					if (!drwav_init(&m_decoder, &ReadWavCallback, &SeekWavCallback, &stream, nullptr))
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit resetOnError([this]
					{
						drwav_uninit(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::optional<AudioFormat> formatOpt = GuessAudioFormat(m_decoder.channels);
					if (!formatOpt)
					{
						NazaraErrorFmt("unexpected channel count: {0}", m_decoder.channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = *formatOpt;

					m_duration = Time::Microseconds(1'000'000LL * m_decoder.totalPCMFrameCount / m_decoder.sampleRate);
					m_sampleCount = m_decoder.totalPCMFrameCount * m_decoder.channels;
					m_sampleRate = m_decoder.sampleRate;

					// Mixing to mono will be done on the fly
					if (parameters.forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = m_decoder.totalPCMFrameCount;
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
						// Keep a buffer to the side to prevent allocation
						m_mixBuffer.resize(sampleCount * m_decoder.channels);
						std::size_t readSample = drwav_read_pcm_frames_s16(&m_decoder, sampleCount, static_cast<Int16*>(m_mixBuffer.data()));
						m_readSampleCount += readSample;

						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), m_decoder.channels, sampleCount);

						return readSample;
					}
					else
					{
						UInt64 readSample = drwav_read_pcm_frames_s16(&m_decoder, sampleCount / m_decoder.channels, static_cast<Int16*>(buffer));
						m_readSampleCount += readSample * m_decoder.channels;

						return readSample * m_decoder.channels;
					}
				}

				void Seek(UInt64 offset) override
				{
					drwav_seek_to_pcm_frame(&m_decoder, (m_mixToMono) ? offset : offset / m_decoder.channels);
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
				drwav m_decoder;
				Time m_duration;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadWavSoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drwavStream> soundStream = std::make_shared<drwavStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadWavSoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drwavStream> soundStream = std::make_shared<drwavStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadWavSoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drwavStream> soundStream = std::make_shared<drwavStream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_drwav()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsWavSupported;
			loaderEntry.streamLoader = LoadWavSoundBuffer;
			loaderEntry.parameterFilter = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinWavLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_drwav()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsWavSupported;
			loaderEntry.fileLoader = LoadWavSoundStreamFile;
			loaderEntry.memoryLoader = LoadWavSoundStreamMemory;
			loaderEntry.streamLoader = LoadWavSoundStreamStream;
			loaderEntry.parameterFilter = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinWavLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
