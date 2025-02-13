// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/Formats/drwavLoader.hpp>
#include <Nazara/Audio2/Algorithm.hpp>
#include <Nazara/Audio2/Audio2.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Audio2/SoundBuffer.hpp>
#include <Nazara/Audio2/SoundStream.hpp>
#include <Nazara/Audio2/Formats/drwavLoader.hpp>
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

			std::span<const AudioChannel> audioChannels = GetAudioChannelMap(wav.channels);
			if (audioChannels.empty())
			{
				NazaraError("unexpected channel count: {0}", wav.channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			UInt64 frameCount = wav.totalPCMFrameCount;

			AudioFormat format = parameters.format;
			if (format != AudioFormat::Signed16)
				format = AudioFormat::Floating32;

			std::shared_ptr<SoundBuffer> soundBuffer = std::make_shared<SoundBuffer>(format, audioChannels, frameCount, wav.sampleRate, nullptr);
			void* samples = soundBuffer->GetSamples();

			if (format == AudioFormat::Signed16)
			{
				if (drwav_read_pcm_frames_s16(&wav, frameCount, static_cast<Int16*>(samples)) != frameCount)
				{
					NazaraError("failed to read stream content");
					return Err(ResourceLoadingError::DecodingError);
				}
			}
			else
			{
				if (drwav_read_pcm_frames_f32(&wav, frameCount, static_cast<float*>(samples)) != frameCount)
				{
					NazaraError("failed to read stream content");
					return Err(ResourceLoadingError::DecodingError);
				}
			}

			if (parameters.format != format)
				soundBuffer->ConvertFormat(parameters.format);

			return soundBuffer;
		}

		class drwavStream : public SoundStream
		{
			public:
				drwavStream() :
				m_currentFramePosition(0)
				{
					std::memset(&m_decoder, 0, sizeof(m_decoder));
				}

				~drwavStream()
				{
					drwav_uninit(&m_decoder);
				}
				
				std::span<const AudioChannel> GetChannels() const override
				{
					return m_channels;
				}

				AudioFormat GetFormat() const override
				{
					return AudioFormat::Floating32;
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

				Result<ReadData, std::string> Read(UInt64 startingFrameIndex, void* frameOut, UInt64 frameCount) override
				{
					if (startingFrameIndex != m_currentFramePosition)
					{
						if (!drwav_seek_to_pcm_frame(&m_decoder, startingFrameIndex))
							return Err(fmt::format("failed to seek to frame {}", startingFrameIndex));
					}

					UInt64 readFrame = drwav_read_pcm_frames_f32(&m_decoder, frameCount, reinterpret_cast<float*>(frameOut));
					m_currentFramePosition += readFrame;

					return ReadData{ readFrame, m_currentFramePosition };
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
					if (!drwav_init(&m_decoder, &ReadWavCallback, &SeekWavCallback, &stream, nullptr))
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit resetOnError([this]
					{
						drwav_uninit(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::span<const AudioChannel> channels = GetAudioChannelMap(m_decoder.channels);
					if (channels.empty())
					{
						NazaraError("unexpected channel count: {0}", m_decoder.channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					UInt64 frameCount = m_decoder.totalPCMFrameCount;

					m_duration = Time::Microseconds(1'000'000LL * frameCount / m_decoder.sampleRate);
					m_frameCount = frameCount;
					m_sampleRate = m_decoder.sampleRate;

					resetOnError.Reset();

					return Ok();
				}

			private:
				std::mutex m_mutex;
				std::span<const AudioChannel> m_channels;
				std::unique_ptr<Stream> m_ownedStream;
				AudioFormat m_format;
				drwav m_decoder;
				Time m_duration;
				UInt32 m_sampleRate;
				UInt64 m_currentFramePosition;
				UInt64 m_frameCount;
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
