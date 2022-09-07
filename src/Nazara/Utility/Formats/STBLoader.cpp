// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Nazara/Utils/Endianness.hpp>
#include <frozen/string.h>
#include <frozen/unordered_set.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		int StbiEof(void* userdata)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			return stream->GetCursorPos() >= stream->GetSize();
		}

		int StbiRead(void* userdata, char* data, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			return static_cast<int>(stream->Read(data, size));
		}

		void StbiSkip(void* userdata, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			stream->SetCursorPos(static_cast<Int64>(stream->GetCursorPos()) + static_cast<Int64>(size));
		}

		static stbi_io_callbacks s_stbiCallbacks = { StbiRead, StbiSkip, StbiEof };

		constexpr auto s_supportedExtensions = frozen::make_unordered_set<frozen::string>({ ".bmp", ".gif", ".hdr", ".jpg", ".jpeg", ".pic", ".png", ".ppm", ".pgm", ".psd", ".tga" });

		bool IsSTBSupported(const std::string_view& extension)
		{
			return s_supportedExtensions.find(extension) != s_supportedExtensions.end();
		}

		Result<std::shared_ptr<Image>, ResourceLoadingError> LoadSTB(Stream& stream, const ImageParams& parameters)
		{
			UInt64 streamPos = stream.GetCursorPos();

			int width, height, bpp;
			if (!stbi_info_from_callbacks(&s_stbiCallbacks, &stream, &width, &height, &bpp))
				return Err(ResourceLoadingError::Unrecognized);

			stream.SetCursorPos(streamPos);

			// Load everything as RGBA8 and then convert using the Image::Convert method
			// This is because of a STB bug when loading some JPG images with default settings

			UInt8* ptr = stbi_load_from_callbacks(&s_stbiCallbacks, &stream, &width, &height, &bpp, STBI_rgb_alpha);
			if (!ptr)
			{
				NazaraError("Failed to load image: " + std::string(stbi_failure_reason()));
				return Err(ResourceLoadingError::Unrecognized);
			}

			CallOnExit freeStbiImage([ptr]()
			{
				stbi_image_free(ptr);
			});

			std::shared_ptr<Image> image = std::make_shared<Image>();
			if (!image->Create(ImageType::E2D, PixelFormat::RGBA8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
			{
				NazaraError("Failed to create image");
				return Err(ResourceLoadingError::Internal);
			}

			image->Update(ptr);

			freeStbiImage.CallAndReset();

			if (parameters.loadFormat != PixelFormat::Undefined)
			{
				if (!image->Convert(parameters.loadFormat))
				{
					NazaraError("Failed to convert image to required format");
					return Err(ResourceLoadingError::Internal);
				}
			}

			return image;
		}
	}

	namespace Loaders
	{
		ImageLoader::Entry GetImageLoader_STB()
		{
			ImageLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsSTBSupported;
			loaderEntry.streamLoader = LoadSTB;
			loaderEntry.parameterFilter = [](const ImageParams& parameters)
			{
				bool skip;
				if (parameters.custom.GetBooleanParameter("SkipBuiltinSTBLoader", &skip) && skip)
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
