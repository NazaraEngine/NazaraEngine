// Copyright (C) 2015 Jérôme Leclercq - 2009 Cruden BV
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/DDSLoader.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Formats/DDSConstants.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "dds");
	}

	nzTernary Check(NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		nzUInt32 magic;
		if (stream.Read(&magic, sizeof(nzUInt32)) == sizeof(nzUInt32))
		{
			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&magic, sizeof(nzUInt32));
			#endif

			if (magic == DDS_MAGIC)
				return nzTernary_True;
		}

		return nzTernary_False;
	}

	bool Load(NzImage* image, NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		DDSHeader header;
		if (stream.Read(&header, sizeof(DDSHeader)) != sizeof(DDSHeader))
		{
			NazaraError("Failed to read DDS header");
			return false;
		}

		DDSHeaderDX10Ext headerDX10;
		if (header.format.flags & DDPF_FOURCC && header.format.fourCC == D3DFMT_DX10)
		{
			if (stream.Read(&headerDX10, sizeof(DDSHeaderDX10Ext)) != sizeof(DDSHeaderDX10Ext))
			{
				NazaraError("Failed to read DDS DX10 extension header");
				return false;
			}
		}
		else
		{
			headerDX10.arraySize = 1;
			headerDX10.dxgiFormat = DXGI_FORMAT_UNKNOWN;
			headerDX10.miscFlag = 0;
			headerDX10.resourceDimension = D3D10_RESOURCE_DIMENSION_UNKNOWN;
		}

		#ifdef NAZARA_BIG_ENDIAN
		// Les fichiers DDS sont en little endian
		NzByteSwap(&header.size, sizeof(nzUInt32));
		NzByteSwap(&header.flags, sizeof(nzUInt32));
		NzByteSwap(&header.height, sizeof(nzUInt32));
		NzByteSwap(&header.width, sizeof(nzUInt32));
		NzByteSwap(&header.pitch, sizeof(nzUInt32));
		NzByteSwap(&header.depth, sizeof(nzUInt32));
		NzByteSwap(&header.levelCount, sizeof(nzUInt32));

		// DDS_PixelFormat
		NzByteSwap(&header.format.size, sizeof(nzUInt32));
		NzByteSwap(&header.format.flags, sizeof(nzUInt32));
		NzByteSwap(&header.format.fourCC, sizeof(nzUInt32));
		NzByteSwap(&header.format.bpp, sizeof(nzUInt32));
		NzByteSwap(&header.format.redMask, sizeof(nzUInt32));
		NzByteSwap(&header.format.greenMask, sizeof(nzUInt32));
		NzByteSwap(&header.format.blueMask, sizeof(nzUInt32));
		NzByteSwap(&header.format.alphaMask, sizeof(nzUInt32));

		NzByteSwap(&header.ddsCaps1, sizeof(nzUInt32));
		NzByteSwap(&header.ddsCaps2, sizeof(nzUInt32));
		NzByteSwap(&header.ddsCaps3, sizeof(nzUInt32));
		NzByteSwap(&header.ddsCaps4, sizeof(nzUInt32));
		#endif

		unsigned int width = header.width;
		unsigned int height = header.height;
		unsigned int depth = std::max(header.depth, 1U);
		unsigned int levelCount = (parameters.levelCount > 0) ? std::min(parameters.levelCount, static_cast<nzUInt8>(header.levelCount)) : header.levelCount;

		// Détermination du type
		nzImageType type;
		if (header.ddsCaps2 & DDSCAPS2_CUBEMAP)
			type = nzImageType_Cubemap;
		else if (header.ddsCaps2 & DDSCAPS2_VOLUME)
			type = nzImageType_3D;

		// Détermination du format
		nzPixelFormat format;

		if (parameters.loadFormat != nzPixelFormat_Undefined)
			image->Convert(parameters.loadFormat);

		return true;
	}
}

void NzLoaders_DDS_Register()
{
	NzImageLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_DDS_Unregister()
{
	NzImageLoader::UnregisterLoader(IsSupported, Check, Load);
}
