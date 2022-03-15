// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <unordered_set>
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

		bool IsSTBSupported(const std::string_view& extension)
		{
			static std::unordered_set<std::string_view> supportedExtensions = {"bmp", "gif", "hdr", "jpg", "jpeg", "pic", "png", "ppm", "pgm", "psd", "tga"};
			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary CheckSTB(Stream& stream, const ImageParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeSTBLoader", &skip) && skip)
				return Ternary::False;

			int width, height, bpp;
			if (stbi_info_from_callbacks(&s_stbiCallbacks, &stream, &width, &height, &bpp))
				return Ternary::True;
			else
				return Ternary::False;
		}

		std::shared_ptr<Image> LoadSTB(Stream& stream, const ImageParams& parameters)
		{
			// Je charge tout en RGBA8 et je converti ensuite via la méthode Convert
			// Ceci à cause d'un bug de STB lorsqu'il s'agit de charger certaines images (ex: JPG) en "default"

			int width, height, bpp;
			UInt8* ptr = stbi_load_from_callbacks(&s_stbiCallbacks, &stream, &width, &height, &bpp, STBI_rgb_alpha);
			if (!ptr)
			{
				NazaraError("Failed to load image: " + std::string(stbi_failure_reason()));
				return {};
			}

			CallOnExit freeStbiImage([ptr]()
			{
				stbi_image_free(ptr);
			});

			std::shared_ptr<Image> image = std::make_shared<Image>();
			if (!image->Create(ImageType::E2D, PixelFormat::RGBA8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
			{
				NazaraError("Failed to create image");
				return {};
			}

			image->Update(ptr);

			freeStbiImage.CallAndReset();

			if (parameters.loadFormat != PixelFormat::Undefined)
			{
				if (!image->Convert(parameters.loadFormat))
				{
					NazaraError("Failed to convert image to required format");
					return {};
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
			loaderEntry.streamChecker = CheckSTB;
			loaderEntry.streamLoader = LoadSTB;

			return loaderEntry;
		}
	}
}
