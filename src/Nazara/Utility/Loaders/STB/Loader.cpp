// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/STB.hpp>
#include <stb_image/stb_image.h>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
    int Read(void* userdata, char* data, int size)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(userdata);
        return static_cast<int>(stream->Read(data, size));
    }

    void Skip(void* userdata, unsigned int size)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(userdata);
        stream->Read(nullptr, size);
    }

    int Eof(void* userdata)
    {
        NzInputStream* stream = reinterpret_cast<NzInputStream*>(userdata);
        return stream->GetCursorPos() >= stream->GetSize();
    }

	static stbi_io_callbacks callbacks = {Read, Skip, Eof};

	bool IsSupported(const NzString& extension)
	{
		static std::set<NzString> supportedExtensions = {"bmp", "gif", "hdr", "jpg", "jpeg", "pic", "png", "psd", "tga"};
		return supportedExtensions.find(extension) != supportedExtensions.end();
	}

	bool Check(NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		int width, height, bpp;
		return stbi_info_from_callbacks(&callbacks, &stream, &width, &height, &bpp);
	}

	bool Load(NzImage* image, NzInputStream& stream, const NzImageParams& parameters)
	{
		static const nzPixelFormat formats[4] =
		{
			nzPixelFormat_L8,
			nzPixelFormat_LA8,
			nzPixelFormat_RGB8,
			nzPixelFormat_RGBA8
		};

		nzPixelFormat format;
		int stbiFormat;
		switch (parameters.loadFormat)
		{
			case nzPixelFormat_L8:
				format = nzPixelFormat_L8;
				stbiFormat = STBI_grey;
				break;

			case nzPixelFormat_LA8:
				format = nzPixelFormat_LA8;
				stbiFormat = STBI_grey_alpha;
				break;

			case nzPixelFormat_RGB8:
				format = nzPixelFormat_RGB8;
				stbiFormat = STBI_rgb;
				break;

			case nzPixelFormat_RGBA8:
				format = nzPixelFormat_RGBA8;
				stbiFormat = STBI_rgb_alpha;
				break;

			default:
				format = nzPixelFormat_Undefined;
				stbiFormat = STBI_default;
		}

		int width, height, bpp;
		nzUInt8* ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &bpp, stbiFormat);

		if (!ptr)
		{
			NazaraError("Failed to load image: " + NzString(stbi_failure_reason()));
			return false;
		}

		if (format == nzPixelFormat_Undefined)
			format = formats[bpp-1];

		if (!image->Create(nzImageType_2D, format, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
		{
			NazaraError("Failed to create image");
			stbi_image_free(ptr);

			return false;
		}

		image->Update(ptr);

		stbi_image_free(ptr);

		if (stbiFormat == STBI_default && parameters.loadFormat != nzPixelFormat_Undefined)
			image->Convert(parameters.loadFormat);

		return true;
	}
}

void NzLoaders_STB_Register()
{
	NzImageLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_STB_Unregister()
{
	NzImageLoader::UnregisterLoader(IsSupported, Check, Load);
}
