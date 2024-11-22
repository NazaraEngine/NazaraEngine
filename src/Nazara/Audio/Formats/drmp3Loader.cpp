// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/Formats/drmp3Loader.hpp>
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

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#include <dr_mp3.h>

namespace Nz
{
	namespace
	{
		std::size_t ReadMp3Callback(void* pUserData, void* pBufferOut, size_t bytesToRead)
		{
			Stream* stream = static_cast<Stream*>(pUserData);
			return static_cast<std::size_t>(stream->Read(pBufferOut, bytesToRead));
		}

		drmp3_bool32 SeekMp3Callback(void* pUserData, int offset, drmp3_seek_origin origin)
		{
			Stream* stream = static_cast<Stream*>(pUserData);
			switch (origin)
			{
				case drmp3_seek_origin_start:
					return stream->SetCursorPos(offset);

				case drmp3_seek_origin_current:
					return (stream->Read(nullptr, static_cast<std::size_t>(offset)) != 0);

				default:
					NazaraInternalError("Seek mode not handled");
					return false;
			}
		}

		bool IsMp3Supported(std::string_view extension)
		{
			return extension == ".mp3";
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadMp3SoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			drmp3 mp3;
			if (!drmp3_init(&mp3, &ReadMp3Callback, &SeekMp3Callback, &stream, nullptr))
				return Err(ResourceLoadingError::Unrecognized);

			CallOnExit uninitOnExit([&] { drmp3_uninit(&mp3); });

			std::optional<AudioFormat> formatOpt = GuessAudioFormat(mp3.channels);
			if (!formatOpt)
			{
				NazaraError("unexpected channel count: {0}", mp3.channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			AudioFormat format = *formatOpt;

			UInt64 frameCount = drmp3_get_pcm_frame_count(&mp3);

			UInt64 sampleCount = frameCount * mp3.channels;
			std::unique_ptr<Int16[]> samples = std::make_unique<Int16[]>(sampleCount); //< std::vector would default-init to zero

			if (drmp3_read_pcm_frames_s16(&mp3, frameCount, samples.get()) != frameCount)
			{
				NazaraError("failed to read stream content");
				return Err(ResourceLoadingError::DecodingError);
			}

			if (parameters.forceMono && format != AudioFormat::I16_Mono)
			{
				MixToMono(samples.get(), samples.get(), static_cast<UInt32>(mp3.channels), frameCount);

				format = AudioFormat::I16_Mono;
				sampleCount = frameCount;
			}

			return std::make_shared<SoundBuffer>(format, sampleCount, mp3.sampleRate, samples.get());
		}

		class drmp3Stream : public SoundStream
		{
			public:
				drmp3Stream() :
				m_readSampleCount(0)
				{
					std::memset(&m_decoder, 0, sizeof(m_decoder));
				}

				~drmp3Stream()
				{
					drmp3_uninit(&m_decoder);
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
					if (!drmp3_init(&m_decoder, &ReadMp3Callback, &SeekMp3Callback, &stream, nullptr))
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit resetOnError([this]
					{
						drmp3_uninit(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::optional<AudioFormat> formatOpt = GuessAudioFormat(m_decoder.channels);
					if (!formatOpt)
					{
						NazaraError("unexpected channel count: {0}", m_decoder.channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					m_format = *formatOpt;

					UInt64 frameCount = drmp3_get_pcm_frame_count(&m_decoder);

					m_duration = Time::Microseconds(1'000'000LL * frameCount / m_decoder.sampleRate);
					m_sampleCount = frameCount * m_decoder.channels;
					m_sampleRate = m_decoder.sampleRate;

					// Mixing to mono will be done on the fly
					if (parameters.forceMono && m_format != AudioFormat::I16_Mono)
					{
						m_mixToMono = true;
						m_sampleCount = frameCount;
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
						std::size_t readSample = drmp3_read_pcm_frames_s16(&m_decoder, sampleCount, static_cast<Int16*>(m_mixBuffer.data()));
						m_readSampleCount += readSample;

						MixToMono(m_mixBuffer.data(), static_cast<Int16*>(buffer), m_decoder.channels, sampleCount);

						return readSample;
					}
					else
					{
						UInt64 readSample = drmp3_read_pcm_frames_s16(&m_decoder, sampleCount / m_decoder.channels, static_cast<Int16*>(buffer));
						m_readSampleCount += readSample * m_decoder.channels;

						return readSample * m_decoder.channels;
					}
				}

				void Seek(UInt64 offset) override
				{
					drmp3_seek_to_pcm_frame(&m_decoder, (m_mixToMono) ? offset : offset / m_decoder.channels);
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
				drmp3 m_decoder;
				Time m_duration;
				UInt32 m_sampleRate;
				UInt64 m_readSampleCount;
				UInt64 m_sampleCount;
				bool m_mixToMono;
		};

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMp3SoundStreamFile(const std::filesystem::path& filePath, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drmp3Stream> soundStream = std::make_shared<drmp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(filePath, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMp3SoundStreamMemory(const void* data, std::size_t size, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drmp3Stream> soundStream = std::make_shared<drmp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(data, size, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}

		Result<std::shared_ptr<SoundStream>, ResourceLoadingError> LoadMp3SoundStreamStream(Stream& stream, const SoundStreamParams& parameters)
		{
			std::shared_ptr<drmp3Stream> soundStream = std::make_shared<drmp3Stream>();
			Result<void, ResourceLoadingError> status = soundStream->Open(stream, parameters);

			return status.Map([&] { return std::move(soundStream); });
		}
	}

	namespace Loaders
	{
		SoundBufferLoader::Entry GetSoundBufferLoader_drmp3()
		{
			SoundBufferLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsMp3Supported;
			loaderEntry.streamLoader = LoadMp3SoundBuffer;
			loaderEntry.parameterFilter = [](const SoundBufferParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMp3Loader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}

		SoundStreamLoader::Entry GetSoundStreamLoader_drmp3()
		{
			SoundStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsMp3Supported;
			loaderEntry.fileLoader = LoadMp3SoundStreamFile;
			loaderEntry.memoryLoader = LoadMp3SoundStreamMemory;
			loaderEntry.streamLoader = LoadMp3SoundStreamStream;
			loaderEntry.parameterFilter = [](const SoundStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMp3Loader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
