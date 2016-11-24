// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <stb/stb_image_write.h>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <map>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		using FormatHandler = bool(*)(const Image& image, const ImageParams& parameters, Stream& stream);

		std::map<String, FormatHandler> s_formatHandlers;

		int ConvertToFloatFormat(Image& image)
		{
			switch (image.GetFormat())
			{
				case PixelFormatType_R32F:
					return 1;

				case PixelFormatType_RG32F:
					return 2;

				case PixelFormatType_RGB32F:
					return 3;

				case PixelFormatType_RGBA32F:
					return 4;

				default:
				{
					if (PixelFormat::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormatType_RGBA32F))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormatType_RGB32F))
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
				case PixelFormatType_L8:
				case PixelFormatType_R8:
					return 1;

				case PixelFormatType_LA8:
				case PixelFormatType_RG8:
					return 2;

				case PixelFormatType_RGB8:
					return 3;

				case PixelFormatType_RGBA8:
					return 4;

				default:
				{
					if (PixelFormat::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormatType_RGBA8))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormatType_RGB8))
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

		bool FormatQuerier(const String& extension)
		{
			return s_formatHandlers.find(extension) != s_formatHandlers.end();
		}

		bool SaveToStream(const Image& image, const String& format, Stream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);

			if (!image.IsValid())
			{
				NazaraError("Invalid image");
				return false;
			}

			ImageType type = image.GetType();
			if (type != ImageType_1D && type != ImageType_2D)
			{
				NazaraError("Image type 0x" + String::Number(type, 16) + " is not in a supported format");
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
		void RegisterSTBSaver()
		{
			s_formatHandlers["bmp"] = &SaveBMP;
			s_formatHandlers["hdr"] = &SaveHDR;
			s_formatHandlers["png"] = &SavePNG;
			s_formatHandlers["tga"] = &SaveTGA;

			ImageSaver::RegisterSaver(FormatQuerier, SaveToStream);
		}

		void UnregisterSTBSaver()
		{
			ImageSaver::UnregisterSaver(FormatQuerier, SaveToStream);
			s_formatHandlers.clear();
		}
	}
}
