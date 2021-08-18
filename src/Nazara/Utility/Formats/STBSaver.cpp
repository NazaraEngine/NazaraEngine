// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <map>
#include <stdexcept>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		using FormatHandler = bool(*)(const Image& image, const ImageParams& parameters, Stream& stream);

		std::map<std::string_view, FormatHandler> s_formatHandlers;

		int ConvertToFloatFormat(Image& image)
		{
			switch (image.GetFormat())
			{
				case PixelFormat::R32F:
					return 1;

				case PixelFormat::RG32F:
					return 2;

				case PixelFormat::RGB32F:
					return 3;

				case PixelFormat::RGBA32F:
					return 4;

				default:
				{
					if (PixelFormatInfo::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormat::RGBA32F))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormat::RGB32F))
							break;

						return 3;
					}
				}
			}

			return 0;
		}

		int ConvertToIntegerFormat(Image& image)
		{
			switch (image.GetFormat())
			{
				case PixelFormat::L8:
				case PixelFormat::R8:
					return 1;

				case PixelFormat::LA8:
				case PixelFormat::RG8:
					return 2;

				case PixelFormat::RGB8:
					return 3;

				case PixelFormat::RGBA8:
					return 4;

				default:
				{
					if (PixelFormatInfo::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormat::RGBA8))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormat::RGB8))
							break;

						return 3;
					}
				}
			}

			return 0;
		}

		void WriteToStream(void* userdata, void* data, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			if (stream->Write(data, size) != static_cast<std::size_t>(size))
				throw std::runtime_error("Failed to write to stream");
		}

		bool FormatQuerier(const std::string_view& extension)
		{
			return s_formatHandlers.find(extension) != s_formatHandlers.end();
		}

		bool SaveToStream(const Image& image, const std::string& format, Stream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);

			if (!image.IsValid())
			{
				NazaraError("Invalid image");
				return false;
			}

			ImageType type = image.GetType();
			if (type != ImageType::E1D && type != ImageType::E2D)
			{
				NazaraError("Image type 0x" + NumberToString(UnderlyingCast(type), 16) + " is not in a supported format");
				return false;
			}

			auto it = s_formatHandlers.find(format);
			NazaraAssert(it != s_formatHandlers.end(), "Invalid handler");

			const FormatHandler& handler = it->second;
			try
			{
				return handler(image, parameters, stream);
			}
			catch (const std::exception& e)
			{
				NazaraError(e.what());
				return false;
			}
		}

		bool SaveBMP(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required
			
			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_bmp_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels()))
			{
				NazaraError("Failed to write BMP to stream");
				return false;
			}

			return true;
		}

		bool SaveJPEG(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			long long imageQuality;
			if (parameters.custom.GetIntegerParameter("NativeJPEGSaver_Quality", &imageQuality))
			{
				if (imageQuality <= 0 || imageQuality > 100)
				{
					NazaraError("NativeJPEGSaver_Quality value (" + Nz::NumberToString(imageQuality) + ") does not fit in bounds ]0, 100], clamping...");
					imageQuality = Nz::Clamp(imageQuality, 1LL, 100LL);
				}
			}
			else
				imageQuality = 100;

			if (!stbi_write_jpg_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels(), int(imageQuality)))
			{
				NazaraError("Failed to write JPEG to stream");
				return false;
			}

			return true;
		}

		bool SaveHDR(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToFloatFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_hdr_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, reinterpret_cast<const float*>(tempImage.GetConstPixels())))
			{
				NazaraError("Failed to write HDR to stream");
				return false;
			}

			return true;
		}

		bool SavePNG(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_png_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels(), 0))
			{
				NazaraError("Failed to write PNG to stream");
				return false;
			}

			return true;
		}

		bool SaveTGA(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_tga_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels()))
			{
				NazaraError("Failed to write TGA to stream");
				return false;
			}

			return true;
		}
	}

	namespace Loaders
	{
		ImageSaver::Entry GetImageSaver_STB()
		{
			s_formatHandlers["bmp"] = &SaveBMP;
			s_formatHandlers["hdr"] = &SaveHDR;
			s_formatHandlers["jpg"] = &SaveJPEG;
			s_formatHandlers["jpeg"] = &SaveJPEG;
			s_formatHandlers["png"] = &SavePNG;
			s_formatHandlers["tga"] = &SaveTGA;

			ImageSaver::Entry entry;
			entry.formatSupport = FormatQuerier;
			entry.streamSaver = SaveToStream;

			return entry;
		}
	}
}
