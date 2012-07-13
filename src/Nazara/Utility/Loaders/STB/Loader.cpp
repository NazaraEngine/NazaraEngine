// Copyright (C) 2011 Jérôme Leclercq
// This file is part of the "Ungine".
// For conditions of distribution and use, see copyright notice in Core.h

#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Utility/Image.hpp>

#define STBI_HEADER_FILE_ONLY
#include <Nazara/Utility/Loaders/STB/stb_image.cpp>

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

	bool NzLoader_STB_LoadStream(NzImage* image, NzInputStream& stream, const NzImageParams& parameters);

	bool NzLoader_STB_LoadFile(NzImage* image, const NzString& filePath, const NzImageParams& parameters)
	{
		NzFile file(filePath);
		if (!file.Open(NzFile::ReadOnly))
		{
			NazaraError("Failed to open file");
			return false;
		}

		return NzLoader_STB_LoadStream(image, file, parameters);
	}

	bool NzLoader_STB_LoadMemory(NzImage* image, const void* data, unsigned int size, const NzImageParams& parameters)
	{
		NzMemoryStream stream(data, size);
		return NzLoader_STB_LoadStream(image, stream, parameters);
	}

	bool NzLoader_STB_LoadStream(NzImage* image, NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

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

	bool NzLoader_STB_IdentifyMemory(const void* data, unsigned int size, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		int width, height, bpp;
		return stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(data), size, &width, &height, &bpp);
	}

	bool NzLoader_STB_IdentifyStream(NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		int width, height, bpp;
		return stbi_info_from_callbacks(&callbacks, &stream, &width, &height, &bpp);
	}
}

void NzLoaders_STB_Register()
{
	NzImageLoader::RegisterFileLoader("bmp, gif, hdr, jpg, jpeg, pic, png, psd, tga", NzLoader_STB_LoadFile);
	NzImageLoader::RegisterMemoryLoader(NzLoader_STB_IdentifyMemory, NzLoader_STB_LoadMemory);
	NzImageLoader::RegisterStreamLoader(NzLoader_STB_IdentifyStream, NzLoader_STB_LoadStream);
}

void NzLoaders_STB_Unregister()
{
	NzImageLoader::UnregisterStreamLoader(NzLoader_STB_IdentifyStream, NzLoader_STB_LoadStream);
	NzImageLoader::UnregisterMemoryLoader(NzLoader_STB_IdentifyMemory, NzLoader_STB_LoadMemory);
	NzImageLoader::UnregisterFileLoader("bmp, gif, hdr, jpg, jpeg, pic, png, psd, tga", NzLoader_STB_LoadFile);
}
