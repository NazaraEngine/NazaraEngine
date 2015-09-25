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

namespace Nz
{
	namespace
	{
		bool IsSupported(const String& extension)
		{
			return (extension == "dds");
		}

		Ternary Check(InputStream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);

			UInt32 magic;
			if (stream.Read(&magic, sizeof(UInt32)) == sizeof(UInt32))
			{
				#ifdef NAZARA_BIG_ENDIAN
				NzByteSwap(&magic, sizeof(UInt32));
				#endif

				if (magic == DDS_MAGIC)
					return Ternary_True;
			}

			return Ternary_False;
		}

		bool Load(Image* image, InputStream& stream, const ImageParams& parameters)
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
			NzByteSwap(&header.size, sizeof(UInt32));
			NzByteSwap(&header.flags, sizeof(UInt32));
			NzByteSwap(&header.height, sizeof(UInt32));
			NzByteSwap(&header.width, sizeof(UInt32));
			NzByteSwap(&header.pitch, sizeof(UInt32));
			NzByteSwap(&header.depth, sizeof(UInt32));
			NzByteSwap(&header.levelCount, sizeof(UInt32));

			// DDS_PixelFormat
			NzByteSwap(&header.format.size, sizeof(UInt32));
			NzByteSwap(&header.format.flags, sizeof(UInt32));
			NzByteSwap(&header.format.fourCC, sizeof(UInt32));
			NzByteSwap(&header.format.bpp, sizeof(UInt32));
			NzByteSwap(&header.format.redMask, sizeof(UInt32));
			NzByteSwap(&header.format.greenMask, sizeof(UInt32));
			NzByteSwap(&header.format.blueMask, sizeof(UInt32));
			NzByteSwap(&header.format.alphaMask, sizeof(UInt32));

			NzByteSwap(&header.ddsCaps1, sizeof(UInt32));
			NzByteSwap(&header.ddsCaps2, sizeof(UInt32));
			NzByteSwap(&header.ddsCaps3, sizeof(UInt32));
			NzByteSwap(&header.ddsCaps4, sizeof(UInt32));
			#endif

			unsigned int width = header.width;
			unsigned int height = header.height;
			unsigned int depth = std::max(header.depth, 1U);
			unsigned int levelCount = (parameters.levelCount > 0) ? std::min(parameters.levelCount, static_cast<UInt8>(header.levelCount)) : header.levelCount;

			// Détermination du type
			ImageType type;
			if (header.ddsCaps2 & DDSCAPS2_CUBEMAP)
				type = ImageType_Cubemap;
			else if (header.ddsCaps2 & DDSCAPS2_VOLUME)
				type = ImageType_3D;

			// Détermination du format
			PixelFormatType format;

			if (parameters.loadFormat != PixelFormatType_Undefined)
				image->Convert(parameters.loadFormat);

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterDDS()
		{
			ImageLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterDDS()
		{
			ImageLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
