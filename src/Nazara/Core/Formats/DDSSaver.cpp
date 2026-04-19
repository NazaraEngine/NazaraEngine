// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/DDSSaver.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/Formats/DDSConstants.hpp>
#include <stdexcept>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		bool SaveToStream(const Image& image, [[maybe_unused]] std::string_view format, Stream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);
			NazaraAssert(format == ".dds");

			if (!image.IsValid())
			{
				NazaraError("invalid image");
				return false;
			}

			ByteStream byteStream(&stream);
			byteStream.SetDataEndianness(Endianness::LittleEndian);

			byteStream << DDS_Magic;

			DDSHeader ddsHeader;
			std::memset(&ddsHeader, 0, sizeof(ddsHeader));
			ddsHeader.size = 124;
			ddsHeader.flags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
			ddsHeader.width = image.GetWidth();
			ddsHeader.height = image.GetHeight();
			ddsHeader.depth = image.GetDepth();
			ddsHeader.ddsCaps[0] = DDSCAPS_TEXTURE;

			PixelFormat pixelFormat = image.GetFormat();
			if (PixelFormatInfo::IsCompressed(pixelFormat))
			{
				ddsHeader.flags |= DDSD_LINEARSIZE;
				ddsHeader.pitchOrLinearSize = SafeCaster(PixelFormatInfo::ComputeSize(pixelFormat, image.GetWidth(), image.GetHeight(), image.GetDepth()));
			}
			else
			{
				ddsHeader.flags |= DDSD_PITCH;
				ddsHeader.pitchOrLinearSize = image.GetWidth() * PixelFormatInfo::GetBitsPerPixel(pixelFormat);
			}

			// DDS_PIXELFORMAT
			ddsHeader.format.size = 32;
			ddsHeader.format.flags = DDPF_FOURCC;
			ddsHeader.format.fourCC = D3DFMT_DX10;

			// FIXME: Is DDPF_RGB required if fourCC is set to D3DFMT_DX10?
			/*if (!PixelFormatInfo::IsCompressed(pixelFormat))
			{
				const PixelFormatDescription& desc = PixelFormatInfo::GetInfo(pixelFormat);

				std::size_t rgbBitCount = desc.redMask.Count() + desc.greenMask.Count() + desc.blueMask.Count();

				ddsHeader.format.flags |= DDPF_RGB;
				ddsHeader.format.rgbBitCount = SafeCaster(rgbBitCount);
				ddsHeader.format.rBitMask = desc.redMask.To<UInt32>();
				ddsHeader.format.gBitMask = desc.greenMask.To<UInt32>();
				ddsHeader.format.bBitMask = desc.blueMask.To<UInt32>();

				if (PixelFormatInfo::HasAlpha(pixelFormat))
				{
					ddsHeader.format.flags |= DDPF_ALPHAPIXELS;
					ddsHeader.format.aBitMask = desc.alphaMask.To<UInt32>();
				}
			}*/

			if (image.GetLevelCount() > 1 && (parameters.levelCount == 0 || parameters.levelCount > 1))
			{
				// DDS doesn't support sparse levels, write only contiguous levels
				UInt8 maxLevelCount = 0;
				for (UInt8 level = 0; level < image.GetLevelCount(); ++level)
				{
					if (!image.IsLevelAllocated(level))
						break;

					maxLevelCount++;
				}

				ddsHeader.levelCount = (parameters.levelCount == 0) ? maxLevelCount : std::min(parameters.levelCount, maxLevelCount);
				ddsHeader.flags |= DDSD_MIPMAPCOUNT;
				ddsHeader.ddsCaps[0] |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
			}
			else
				ddsHeader.levelCount = 1; // some software may try to read levelCount even if DDSD_MIPMAPCOUNT is not set

			if (image.GetType() == ImageType::Cubemap)
			{
				ddsHeader.ddsCaps[0] |= DDSCAPS_COMPLEX;
				ddsHeader.ddsCaps[1] |= DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALLFACES;
			}

			// TODO: Don't use DDSHeader10Ext if not required
			DDSHeaderDX10Ext ddsHeaderExt;
			std::memset(&ddsHeaderExt, 0, sizeof(ddsHeaderExt));

			UInt32 layerCount = 1;

			switch (image.GetType())
			{
				case ImageType::E1D:
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
					ddsHeaderExt.arraySize = 1;
					break;

				case ImageType::E1D_Array:
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
					ddsHeaderExt.arraySize = image.GetDepth();
					layerCount = image.GetDepth();
					break;

				case ImageType::E2D:
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
					ddsHeaderExt.arraySize = 1;
					break;

				case ImageType::E2D_Array:
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
					ddsHeaderExt.arraySize = image.GetDepth();
					layerCount = image.GetDepth();
					break;

				case ImageType::E3D:
					ddsHeader.flags |= DDSD_DEPTH;
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
					ddsHeaderExt.arraySize = 1;
					break;

				case ImageType::Cubemap:
					ddsHeaderExt.arraySize = image.GetDepth();
					ddsHeaderExt.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
					ddsHeaderExt.miscFlag = D3D10_RESOURCE_MISC_TEXTURECUBE;
					ddsHeader.ddsCaps[0] |= DDSCAPS_COMPLEX;
					ddsHeader.ddsCaps[1] |= DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALLFACES;
					layerCount = image.GetDepth() * 6;
					break;
			}

			switch (image.GetFormat())
			{
				case PixelFormat::A8:             ddsHeaderExt.dxgiFormat = DXGI_FORMAT_A8_UNORM; break;
				case PixelFormat::BGRA8:          ddsHeaderExt.dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM; break;
				case PixelFormat::BGRA8_SRGB:     ddsHeaderExt.dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;
				case PixelFormat::R8:             ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R8_UNORM; break;
				case PixelFormat::R16:            ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16_UNORM; break;
				case PixelFormat::R16F:           ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16_FLOAT; break;
				case PixelFormat::RG8:            ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R8G8_UNORM; break;
				case PixelFormat::RG16F:          ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16G16_FLOAT; break;
				case PixelFormat::RG32F:          ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32_FLOAT; break;
				case PixelFormat::RGBA8:          ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM; break;
				case PixelFormat::RGBA8_SRGB:     ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
				case PixelFormat::RGBA16F:        ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
				case PixelFormat::RGBA16I:        ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16G16B16A16_SINT; break;
				case PixelFormat::RGBA16UI:       ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R16G16B16A16_UINT; break;
				case PixelFormat::RGBA32F:        ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				case PixelFormat::RGBA32I:        ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32B32A32_SINT; break;
				case PixelFormat::RGBA32UI:       ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32B32A32_UINT; break;
				case PixelFormat::RGB32F:         ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case PixelFormat::RGB32I:         ddsHeaderExt.dxgiFormat = DXGI_FORMAT_R32G32B32_SINT; break;
				case PixelFormat::BC1_RGBA_Unorm: ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC1_UNORM; break;
				case PixelFormat::BC1_RGBA_sRGB:  ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
				case PixelFormat::BC2_Unorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC2_UNORM; break;
				case PixelFormat::BC2_sRGB:       ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
				case PixelFormat::BC3_Unorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC3_UNORM; break;
				case PixelFormat::BC3_sRGB:       ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
				case PixelFormat::BC4_Snorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC4_SNORM; break;
				case PixelFormat::BC4_Unorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC4_UNORM; break;
				case PixelFormat::BC5_Snorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC5_SNORM; break;
				case PixelFormat::BC5_Unorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC5_UNORM; break;
				case PixelFormat::BC6H_SFloat:    ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC6H_SF16; break;
				case PixelFormat::BC6H_UFloat:    ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC6H_UF16; break;
				case PixelFormat::BC7_Unorm:      ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC7_UNORM; break;
				case PixelFormat::BC7_sRGB:       ddsHeaderExt.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB; break;

				default:
				{
					NazaraError("unhandled pixel format {}", PixelFormatInfo::GetName(image.GetFormat()));
					return false;
				}
			}

			byteStream << ddsHeader;
			byteStream << ddsHeaderExt;

			for (UInt32 layer = 0; layer < layerCount; ++layer)
			{
				bool success = ImageUtils::ForEachLevel(ddsHeader.levelCount, image.GetType(), ddsHeader.width, ddsHeader.height, ddsHeader.depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
				{
					std::size_t bytePerLayer = PixelFormatInfo::ComputeSize(image.GetFormat(), width, height, depth);
					const UInt8* ptr = image.GetConstPixels(level);
					NazaraAssert(ptr);

					ptr += bytePerLayer * layer;

					if (byteStream.Write(ptr, bytePerLayer) != bytePerLayer)
					{
						NazaraError("failed to write level #{0} of layer {1}", level, layer);
						return false;
					}

					return true;
				});

				if (!success)
					return false;
			}

			return true;
		}
	}

	namespace Loaders
	{
		ImageSaver::Entry GetImageSaver_DDS()
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			ImageSaver::Entry entry;
			entry.formatSupport = [](std::string_view ext) { return ext == ".dds"; };
			entry.streamSaver = SaveToStream;

			return entry;
		}
	}
}
