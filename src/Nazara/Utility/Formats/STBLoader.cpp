// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <stb/stb_image.h>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <set>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		int Eof(void* userdata)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			return stream->GetCursorPos() >= stream->GetSize();
		}

		int Read(void* userdata, char* data, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			return static_cast<int>(stream->Read(data, size));
		}

		void Skip(void* userdata, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			stream->SetCursorPos(static_cast<Int64>(stream->GetCursorPos()) + static_cast<Int64>(size));
		}

		static stbi_io_callbacks callbacks = {Read, Skip, Eof};

		bool IsSupported(const String& extension)
		{
			static std::set<String> supportedExtensions = {"bmp", "gif", "hdr", "jpg", "jpeg", "pic", "png", "ppm", "pgm", "psd", "tga"};
			return supportedExtensions.find(extension) != supportedExtensions.end();
		}

		Ternary Check(Stream& stream, const ImageParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeSTBLoader", &skip) && skip)
				return Ternary_False;

			int width, height, bpp;
			if (stbi_info_from_callbacks(&callbacks, &stream, &width, &height, &bpp))
				return Ternary_True;
			else
				return Ternary_False;
		}

		bool Load(Image* image, Stream& stream, const ImageParams& parameters)
		{
			// Je charge tout en RGBA8 et je converti ensuite via la méthode Convert
			// Ceci à cause d'un bug de STB lorsqu'il s'agit de charger certaines images (ex: JPG) en "default"

			int width, height, bpp;
			UInt8* ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &bpp, STBI_rgb_alpha);
			if (!ptr)
			{
				NazaraError("Failed to load image: " + String(stbi_failure_reason()));
				return false;
			}

			if (!image->Create(ImageType_2D, PixelFormatType_RGBA8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
			{
				NazaraError("Failed to create image");
				stbi_image_free(ptr);

				return false;
			}

			image->Update(ptr);
			stbi_image_free(ptr);

			if (parameters.loadFormat != PixelFormatType_Undefined)
				image->Convert(parameters.loadFormat);

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterSTBLoader()
		{
			ImageLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterSTBLoader()
		{
			ImageLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
