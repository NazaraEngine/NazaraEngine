/*
Nazara Engine - FFmpeg Plugin

Copyright (C) 2022 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <NazaraUtils/Algorithm.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/ImageStream.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Plugins/FFmpegPlugin.hpp>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
}

#include <array>
#include <cstring>

namespace
{
	class FFmpegStream : public Nz::ImageStream
	{
		public:
			FFmpegStream() :
			m_codec(nullptr),
			m_codecContext(nullptr),
			m_formatContext(nullptr),
			m_rawFrame(nullptr),
			m_rgbaFrame(nullptr),
			m_ioContext(nullptr),
			m_conversionContext(nullptr),
			m_ioBuffer(nullptr),
			m_videoStream(-1)
			{
			}

			~FFmpegStream()
			{
				if (m_conversionContext)
					sws_freeContext(m_conversionContext);

				if (m_rawFrame)
					av_frame_free(&m_rawFrame);

				if (m_rgbaFrame)
					av_frame_free(&m_rgbaFrame);

				if (m_codecContext)
					avcodec_free_context(&m_codecContext);

				if (m_rawFrame)
					av_frame_free(&m_rawFrame);

				if (m_formatContext)
					avformat_close_input(&m_formatContext);

				if (m_ioContext)
					avio_context_free(&m_ioContext);

				// m_ioBuffer is freed by avio_close
				if (m_ioBuffer)
					av_free(&m_ioBuffer);
			}

			Nz::Result<void, Nz::ResourceLoadingError> Check()
			{
				constexpr std::size_t BufferSize = 32768;

				m_ioBuffer = av_malloc(BufferSize + AV_INPUT_BUFFER_PADDING_SIZE);
				m_ioContext = avio_alloc_context(static_cast<unsigned char*>(m_ioBuffer), BufferSize, 0, &m_byteStream, &FFmpegStream::Read, nullptr, &FFmpegStream::Seek);
				if (!m_ioContext)
				{
					NazaraError("failed to create io context");
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				m_formatContext = avformat_alloc_context();
				if (!m_formatContext)
				{
					NazaraError("failed to allocate format context");
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				m_formatContext->pb = m_ioContext;

				av_log_set_level(AV_LOG_FATAL);

				if (int errCode = avformat_open_input(&m_formatContext, "", nullptr, nullptr); errCode != 0)
				{
					NazaraErrorFmt("failed to open input: {0}", ErrorToString(errCode));
					return Nz::Err(Nz::ResourceLoadingError::Unrecognized);
				}

				if (int errCode = avformat_find_stream_info(m_formatContext, nullptr); errCode != 0)
				{
					NazaraErrorFmt("failed to find stream info: {0}", ErrorToString(errCode));
					return Nz::Err(Nz::ResourceLoadingError::Unrecognized);
				}

				m_videoStream = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
				if (m_videoStream < 0)
				{
					NazaraError("failed to find video stream");
					return Nz::Err(Nz::ResourceLoadingError::Unrecognized);
				}

				if (m_formatContext->streams[m_videoStream]->nb_frames == 0)
				{
					NazaraError("unhandled 0 frame count");
					return Nz::Err(Nz::ResourceLoadingError::Unsupported);
				}

				m_codec = avcodec_find_decoder(m_formatContext->streams[m_videoStream]->codecpar->codec_id);
				if (!m_codec)
				{
					NazaraError("codec not found");
					return Nz::Err(Nz::ResourceLoadingError::Unsupported);
				}

				return Nz::Ok();
			}

			bool DecodeNextFrame(void* frameBuffer, Nz::Time* frameTime) override
			{
				AVPacket packet;

				for (;;)
				{
					if (int errCode = av_read_frame(m_formatContext, &packet); errCode < 0)
					{
						if (errCode == AVERROR_EOF)
						{
							if (frameTime)
							{
								AVRational timebase = m_formatContext->streams[m_videoStream]->time_base;
								*frameTime = Nz::Time::Milliseconds(1000 * m_formatContext->streams[m_videoStream]->duration * timebase.num / timebase.den);
							}

							return false;
						}

						NazaraErrorFmt("failed to read frame: {0}", ErrorToString(errCode));
						return false;
					}

					if (packet.stream_index != m_videoStream)
						continue;

					if (int errCode = avcodec_send_packet(m_codecContext, &packet); errCode < 0)
					{
						NazaraErrorFmt("failed to send packet: {0}", ErrorToString(errCode));
						return false;
					}

					if (int errCode = avcodec_receive_frame(m_codecContext, m_rawFrame); errCode < 0)
					{
						if (errCode == AVERROR(EAGAIN))
							continue;

						NazaraErrorFmt("failed to receive frame: {0}", ErrorToString(errCode));
						return false;
					}

					break;
				}

				sws_scale(m_conversionContext, m_rawFrame->data, m_rawFrame->linesize, 0, m_codecContext->height, m_rgbaFrame->data, m_rgbaFrame->linesize);

				Nz::UInt8* dst = static_cast<Nz::UInt8*>(frameBuffer);
				Nz::UInt8* src = m_rgbaFrame->data[0];
				std::size_t lineSize = m_rgbaFrame->width * 4;
				if (lineSize != m_rgbaFrame->linesize[0])
				{
					for (int i = 0; i < m_rgbaFrame->height; ++i)
					{
						std::memcpy(dst, src, lineSize);
						dst += lineSize;
						src += m_rgbaFrame->linesize[0];
					}
				}
				else
					std::memcpy(dst, src, lineSize * m_rgbaFrame->height);

				if (frameTime)
				{
					AVRational timebase = m_formatContext->streams[m_videoStream]->time_base;
					*frameTime = Nz::Time::Milliseconds(1000 * m_rawFrame->pts * timebase.num / timebase.den);
				}

				return true;
			}

			Nz::UInt64 GetFrameCount() const override
			{
				return m_formatContext->streams[m_videoStream]->nb_frames;
			}

			Nz::PixelFormat GetPixelFormat() const override
			{
				return Nz::PixelFormat::RGBA8;
			}

			Nz::Vector2ui GetSize() const override
			{
				unsigned int width = Nz::SafeCast<unsigned int>(m_codecContext->width);
				unsigned int height = Nz::SafeCast<unsigned int>(m_codecContext->height);

				return { width, height };
			}

			Nz::Result<void, Nz::ResourceLoadingError> Open()
			{
				auto checkResult = Check();
				if (!checkResult)
					return checkResult;

				const AVCodecParameters* codecParameters = m_formatContext->streams[m_videoStream]->codecpar;

				m_codecContext = avcodec_alloc_context3(m_codec);
				if (!m_codecContext)
				{
					NazaraError("failed to allocate codec context");
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				if (int errCode = avcodec_parameters_to_context(m_codecContext, codecParameters); errCode < 0)
				{
					NazaraErrorFmt("failed to copy codec params to codec context: {0}", ErrorToString(errCode));
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				if (int errCode = avcodec_open2(m_codecContext, m_codec, nullptr); errCode < 0)
				{
					NazaraErrorFmt("could not open codec: {0}", ErrorToString(errCode));
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				m_rawFrame = av_frame_alloc();
				m_rgbaFrame = av_frame_alloc();
				if (!m_rawFrame || !m_rgbaFrame)
				{
					NazaraError("failed to allocate frames");
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				m_rgbaFrame->format = AVPixelFormat::AV_PIX_FMT_RGBA;
				m_rgbaFrame->width = codecParameters->width;
				m_rgbaFrame->height = codecParameters->height;

				if (int errCode = av_frame_get_buffer(m_rgbaFrame, 0); errCode < 0)
				{
					NazaraErrorFmt("failed to open input: {0}", ErrorToString(errCode));
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				m_conversionContext = sws_getContext(m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height, AVPixelFormat::AV_PIX_FMT_RGBA, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
				if (!m_conversionContext)
				{
					NazaraError("failed to allocate conversion context");
					return Nz::Err(Nz::ResourceLoadingError::Internal);
				}

				return Nz::Ok();
			}

			void Seek(Nz::UInt64 frameIndex) override
			{
				// TODO
				avio_seek(m_ioContext, 0, SEEK_SET);
				avformat_seek_file(m_formatContext, m_videoStream, std::numeric_limits<Nz::Int64>::min(), 0, std::numeric_limits<Nz::Int64>::max(), 0);
			}

			bool SetFile(const std::filesystem::path& filePath)
			{
				std::unique_ptr<Nz::File> file = std::make_unique<Nz::File>();
				if (!file->Open(filePath, Nz::OpenMode::Read))
				{
					NazaraErrorFmt("failed to open stream from file: {0}", Nz::Error::GetLastError());
					return false;
				}
				m_ownedStream = std::move(file);

				SetStream(*m_ownedStream);
				return true;
			}

			void SetMemory(const void* data, std::size_t size)
			{
				m_ownedStream = std::make_unique<Nz::MemoryView>(data, size);
				SetStream(*m_ownedStream);
			}

			void SetStream(Nz::Stream& stream)
			{
				m_byteStream.SetStream(&stream);
			}

			Nz::UInt64 Tell() override
			{
				return 0;
			}


		private:
			static std::string ErrorToString(int errCode)
			{
				// extract error tag
				unsigned int tag = static_cast<unsigned int>(-errCode);

				std::string errMessage(6 + AV_ERROR_MAX_STRING_SIZE, ' '); // "ABCD: <error>"
				for (std::size_t i = 0; i < 4; ++i)
					errMessage[i] = (tag >> i * 8) & 0xFF;

				errMessage[4] = ':';

				if (av_strerror(errCode, &errMessage[6], AV_ERROR_MAX_STRING_SIZE) == 0)
					std::strcpy(&errMessage[6], "<av_strerror failed>");

				errMessage.resize(std::strlen(errMessage.data())); //< ew

				return errMessage;
			}

			static int Read(void* opaque, Nz::UInt8* buf, int buf_size)
			{
				Nz::ByteStream& stream = *static_cast<Nz::ByteStream*>(opaque);
				std::size_t readSize = stream.Read(buf, buf_size);
				if (readSize == 0)
				{
					if (stream.GetStream()->EndOfStream())
						return AVERROR_EOF;

					return -1; //< failure
				}

				return Nz::SafeCast<int>(readSize);
			}

			static int64_t Seek(void* opaque, int64_t offset, int whence)
			{
				Nz::ByteStream& byteStream = *static_cast<Nz::ByteStream*>(opaque);
				Nz::Stream* stream = byteStream.GetStream();

				if (stream->IsSequential())
					return -1;

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

					case AVSEEK_SIZE:
						return stream->GetSize();

					default:
						NazaraInternalError("Seek mode not handled");
						return false;
				}

				return stream->GetCursorPos();
			}

			const AVCodec* m_codec;
			AVCodecContext* m_codecContext;
			AVFormatContext* m_formatContext;
			AVFrame* m_rawFrame;
			AVFrame* m_rgbaFrame;
			AVIOContext* m_ioContext;
			SwsContext* m_conversionContext;
			void* m_ioBuffer;
			std::unique_ptr<Nz::Stream> m_ownedStream;
			Nz::ByteStream m_byteStream;
			int m_videoStream;
	};

	bool CheckVideoExtension(std::string_view extension)
	{
		const AVOutputFormat* format = av_guess_format(nullptr, extension.data(), nullptr);
		if (!format)
			return false;

		return format->video_codec != AV_CODEC_ID_NONE;
	}

	Nz::Result<std::shared_ptr<Nz::ImageStream>, Nz::ResourceLoadingError> LoadFile(const std::filesystem::path& filePath, const Nz::ImageStreamParams& /*parameters*/)
	{
		std::shared_ptr<FFmpegStream> ffmpegStream = std::make_shared<FFmpegStream>();
		ffmpegStream->SetFile(filePath);

		Nz::Result<void, Nz::ResourceLoadingError> status = ffmpegStream->Open();
		return status.Map([&] { return std::move(ffmpegStream); });
	}

	Nz::Result<std::shared_ptr<Nz::ImageStream>, Nz::ResourceLoadingError> LoadMemory(const void* ptr, std::size_t size, const Nz::ImageStreamParams& /*parameters*/)
	{
		std::shared_ptr<FFmpegStream> ffmpegStream = std::make_shared<FFmpegStream>();
		ffmpegStream->SetMemory(ptr, size);

		Nz::Result<void, Nz::ResourceLoadingError> status = ffmpegStream->Open();
		return status.Map([&] { return std::move(ffmpegStream); });
	}

	Nz::Result<std::shared_ptr<Nz::ImageStream>, Nz::ResourceLoadingError> LoadStream(Nz::Stream& stream, const Nz::ImageStreamParams& /*parameters*/)
	{
		std::shared_ptr<FFmpegStream> ffmpegStream = std::make_shared<FFmpegStream>();
		ffmpegStream->SetStream(stream);

		Nz::Result<void, Nz::ResourceLoadingError> status = ffmpegStream->Open();
		return status.Map([&] { return std::move(ffmpegStream); });
	}

	class FFmpegPluginImpl final : public Nz::FFmpegPlugin
	{
		public:
			bool Activate() override
			{
				Nz::Core* core = Nz::Core::Instance();
				NazaraAssert(core, "core module is not instancied");

				Nz::ImageStreamLoader::Entry loaderEntry;
				loaderEntry.extensionSupport = CheckVideoExtension;
				loaderEntry.fileLoader = LoadFile;
				loaderEntry.memoryLoader = LoadMemory;
				loaderEntry.streamLoader = LoadStream;
				loaderEntry.parameterFilter = [](const Nz::ImageStreamParams& parameters)
				{
					if (auto result = parameters.custom.GetBooleanParameter("SkipFFMpegLoader"); result.GetValueOr(false))
						return false;

					return true;
				};

				Nz::ImageStreamLoader& imageStreamLoader = core->GetImageStreamLoader();
				m_ffmpegLoaderEntry = imageStreamLoader.RegisterLoader(loaderEntry);

				return true;
			}

			void Deactivate() override
			{
				Nz::Core* core = Nz::Core::Instance();
				NazaraAssert(core, "core module is not instanced");

				Nz::ImageStreamLoader& imageStreamLoader = core->GetImageStreamLoader();
				imageStreamLoader.UnregisterLoader(m_ffmpegLoaderEntry);

				m_ffmpegLoaderEntry = nullptr;
			}

			std::string_view GetDescription() const override
			{
				return "Adds supports to load and decode videos streams";
			}

			std::string_view GetName() const override
			{
				return "FFMpeg loader";
			}

			Nz::UInt32 GetVersion() const override
			{
				return 100;
			}

		private:
			const Nz::ImageStreamLoader::Entry* m_ffmpegLoaderEntry = nullptr;
	};
}

extern "C"
{
	NAZARA_EXPORT Nz::PluginInterface* PluginLoad()
	{
		Nz::Core* core = Nz::Core::Instance();
		if (!core)
		{
			NazaraError("Core module must be initialized");
			return nullptr;
		}

		std::unique_ptr<FFmpegPluginImpl> plugin = std::make_unique<FFmpegPluginImpl>();
		return plugin.release();
	}
}
