// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

#define DR_MP3_FLOAT_OUTPUT
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
				case DRMP3_SEEK_SET:
					return stream->SetCursorPos(offset);

				case DRMP3_SEEK_CUR:
					return (stream->Read(nullptr, static_cast<std::size_t>(offset)) != 0);

				case DRMP3_SEEK_END:
					return stream->SetCursorPos(stream->GetSize());

				default:
					NazaraInternalError("Seek mode not handled");
					return false;
			}
		}

		drmp3_bool32 TellMp3Callback(void* pUserData, drmp3_int64* pCursor)
		{
			Stream* stream = static_cast<Stream*>(pUserData);
			*pCursor = static_cast<drmp3_int64>(stream->GetCursorPos());
			return true;
		}

		bool IsMp3Supported(std::string_view extension)
		{
			return extension == ".mp3";
		}

		Result<std::shared_ptr<SoundBuffer>, ResourceLoadingError> LoadMp3SoundBuffer(Stream& stream, const SoundBufferParams& parameters)
		{
			drmp3 mp3;
			if (!drmp3_init(&mp3, &ReadMp3Callback, &SeekMp3Callback, &TellMp3Callback, nullptr, &stream, nullptr))
				return Err(ResourceLoadingError::Unrecognized);

			CallOnExit uninitOnExit([&] { drmp3_uninit(&mp3); });

			std::span<const AudioChannel> audioChannels = GetAudioChannelMap(mp3.channels);
			if (audioChannels.empty())
			{
				NazaraError("unexpected channel count: {0}", mp3.channels);
				return Err(ResourceLoadingError::Unsupported);
			}

			UInt64 frameCount = drmp3_get_pcm_frame_count(&mp3);

			AudioFormat format = parameters.format;
			if (format != AudioFormat::Signed16)
				format = AudioFormat::Floating32;

			std::shared_ptr<SoundBuffer> soundBuffer = std::make_shared<SoundBuffer>(format, audioChannels, frameCount, mp3.sampleRate, nullptr);
			void* samples = soundBuffer->GetSamples();

			if (format == AudioFormat::Signed16)
			{
				if (drmp3_read_pcm_frames_s16(&mp3, frameCount, static_cast<Int16*>(samples)) != frameCount)
				{
					NazaraError("failed to read stream content");
					return Err(ResourceLoadingError::DecodingError);
				}
			}
			else
			{
				if (drmp3_read_pcm_frames_f32(&mp3, frameCount, static_cast<float*>(samples)) != frameCount)
				{
					NazaraError("failed to read stream content");
					return Err(ResourceLoadingError::DecodingError);
				}
			}

			if (parameters.format != format)
				soundBuffer->ConvertFormat(parameters.format);

			return soundBuffer;
		}

		class drmp3Stream : public SoundStream
		{
			public:
				drmp3Stream() :
				m_currentFramePosition(0)
				{
					std::memset(&m_decoder, 0, sizeof(m_decoder));
				}

				~drmp3Stream()
				{
					drmp3_uninit(&m_decoder);
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
						if (!drmp3_seek_to_pcm_frame(&m_decoder, startingFrameIndex))
							return Err(fmt::format("failed to seek to frame {}", startingFrameIndex));
					}

					UInt64 readFrame = drmp3_read_pcm_frames_f32(&m_decoder, frameCount, reinterpret_cast<float*>(frameOut));
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

				Result<void, ResourceLoadingError> Open(Stream& stream, const SoundStreamParams& /*parameters*/)
				{
					if (!drmp3_init(&m_decoder, &ReadMp3Callback, &SeekMp3Callback, &TellMp3Callback, nullptr, &stream, nullptr))
						return Err(ResourceLoadingError::Unrecognized);

					CallOnExit resetOnError([this]
					{
						drmp3_uninit(&m_decoder);
						std::memset(&m_decoder, 0, sizeof(m_decoder));
					});

					std::span<const AudioChannel> channels = GetAudioChannelMap(m_decoder.channels);
					if (channels.empty())
					{
						NazaraError("unexpected channel count: {0}", m_decoder.channels);
						return Err(ResourceLoadingError::Unsupported);
					}

					UInt64 frameCount = drmp3_get_pcm_frame_count(&m_decoder);

					m_channels = channels;
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
				drmp3 m_decoder;
				Time m_duration;
				UInt32 m_sampleRate;
				UInt64 m_currentFramePosition;
				UInt64 m_frameCount;
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
