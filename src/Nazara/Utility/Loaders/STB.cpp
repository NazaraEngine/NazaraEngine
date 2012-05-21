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
#include <Nazara/Utility/Loaders/STB/stb_image.c>

#include <Nazara/Utility/Debug.hpp>

// Auteur du loader original : David Henry

namespace
{
    int Read(void* userdata, char* data, int size)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(userdata);
        return static_cast<int>(stream->Read(data, size));
    }

    void Skip(void* userdata, unsigned int size)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(userdata);
        stream->Read(nullptr, size);
    }

    int Eof(void* userdata)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(userdata);
        return stream->GetCursorPos() >= stream->GetSize();
    }

	static stbi_io_callbacks callbacks = {Read, Skip, Eof};

	bool NzLoader_STB_LoadStream(NzImage* resource, NzInputStream& stream, const NzImageParams& parameters);

	bool NzLoader_STB_LoadFile(NzImage* resource, const NzString& filePath, const NzImageParams& parameters)
	{
		NzFile file(filePath);
		if (!file.Open(NzFile::ReadOnly))
		{
			NazaraError("Failed to open file");
			return false;
		}

		return NzLoader_STB_LoadStream(resource, file, parameters);
	}

	bool NzLoader_STB_LoadMemory(NzImage* resource, const void* data, unsigned int size, const NzImageParams& parameters)
	{
		NzMemoryStream stream(data, size);
		return NzLoader_STB_LoadStream(resource, stream, parameters);
	}

	bool NzLoader_STB_LoadStream(NzImage* resource, NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		static nzPixelFormat format[4] = {
			nzPixelFormat_L8,
			nzPixelFormat_L8A8,
			nzPixelFormat_R8G8B8,
			nzPixelFormat_R8G8B8A8
		};

		int width, height, bpp;
		nzUInt8* ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &bpp, STBI_default);

		if (!ptr)
		{
			NazaraError("Failed to load image: " + NzString(stbi_failure_reason()));
			return false;
		}

		if (!resource->Create(nzImageType_2D, format[bpp-1], width, height))
		{
			NazaraError("Failed to create image");
			stbi_image_free(ptr);

			return false;
		}

		resource->Update(ptr);
		stbi_image_free(ptr);

		return true;
	}

	bool NzLoader_STB_IsMemoryLoadingSupported(const void* data, unsigned int size, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		int width, height, bpp;
		return stbi_info_from_memory(static_cast<const stbi_uc*>(data), size, &width, &height, &bpp);
	}

	bool NzLoader_STB_IsStreamLoadingSupported(NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		int width, height, bpp;
		return stbi_info_from_callbacks(&callbacks, &stream, &width, &height, &bpp);
	}
}

void NzLoaders_STB_Register()
{
	NzImage::RegisterFileLoader("bmp, gif, hdr, jpg, jpeg, pic, png, psd, tga", NzLoader_STB_LoadFile);
	NzImage::RegisterMemoryLoader(NzLoader_STB_IsMemoryLoadingSupported, NzLoader_STB_LoadMemory);
	NzImage::RegisterStreamLoader(NzLoader_STB_IsStreamLoadingSupported, NzLoader_STB_LoadStream);
}

void NzLoaders_STB_Unregister()
{
	NzImage::UnregisterStreamLoader(NzLoader_STB_IsStreamLoadingSupported, NzLoader_STB_LoadStream);
	NzImage::UnregisterMemoryLoader(NzLoader_STB_IsMemoryLoadingSupported, NzLoader_STB_LoadMemory);
	NzImage::UnregisterFileLoader("bmp, gif, hdr, jpg, jpeg, pic, png, psd, tga", NzLoader_STB_LoadFile);
}
