// Copyright (C) 2009 Cruden BV - 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_DDS_CONSTANTS_HPP
#define NAZARA_LOADERS_DDS_CONSTANTS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	inline constexpr UInt32 DDS_FourCC(UInt32 a, UInt32 b, UInt32 c, UInt32 d)
	{
		return a << 0 |
		       b << 8 |
		       c << 16 |
		       d << 24;
	}

	constexpr UInt32 DDS_Magic = DDS_FourCC('D', 'D', 'S', ' ');

	enum D3D10_RESOURCE_DIMENSION
	{
		D3D10_RESOURCE_DIMENSION_UNKNOWN   = 0,
		D3D10_RESOURCE_DIMENSION_BUFFER    = 1,
		D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
		D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
		D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
	};

	enum D3D10_RESOURCE_MISC
	{
		D3D10_RESOURCE_MISC_GENERATE_MIPS     = 0x1L,
		D3D10_RESOURCE_MISC_SHARED            = 0x2L,
		D3D10_RESOURCE_MISC_TEXTURECUBE       = 0x4L,
		D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x10L,
		D3D10_RESOURCE_MISC_GDI_COMPATIBLE    = 0x20L
	};

	enum D3DFMT
	{
		D3DFMT_UNKNOWN              =  0,

		D3DFMT_R8G8B8               = 20,
		D3DFMT_A8R8G8B8             = 21,
		D3DFMT_X8R8G8B8             = 22,
		D3DFMT_R5G6B5               = 23,
		D3DFMT_X1R5G5B5             = 24,
		D3DFMT_A1R5G5B5             = 25,
		D3DFMT_A4R4G4B4             = 26,
		D3DFMT_R3G3B2               = 27,
		D3DFMT_A8                   = 28,
		D3DFMT_A8R3G3B2             = 29,
		D3DFMT_X4R4G4B4             = 30,
		D3DFMT_A2B10G10R10          = 31,
		D3DFMT_A8B8G8R8             = 32,
		D3DFMT_X8B8G8R8             = 33,
		D3DFMT_G16R16               = 34,
		D3DFMT_A2R10G10B10          = 35,
		D3DFMT_A16B16G16R16         = 36,

		D3DFMT_A8P8                 = 40,
		D3DFMT_P8                   = 41,

		D3DFMT_L8                   = 50,
		D3DFMT_A8L8                 = 51,
		D3DFMT_A4L4                 = 52,

		D3DFMT_V8U8                 = 60,
		D3DFMT_L6V5U5               = 61,
		D3DFMT_X8L8V8U8             = 62,
		D3DFMT_Q8W8V8U8             = 63,
		D3DFMT_V16U16               = 64,
		D3DFMT_A2W10V10U10          = 67,

		D3DFMT_UYVY                 = DDS_FourCC('U', 'Y', 'V', 'Y'),
		D3DFMT_R8G8_B8G8            = DDS_FourCC('R', 'G', 'B', 'G'),
		D3DFMT_YUY2                 = DDS_FourCC('Y', 'U', 'Y', '2'),
		D3DFMT_G8R8_G8B8            = DDS_FourCC('G', 'R', 'G', 'B'),
		D3DFMT_DXT1                 = DDS_FourCC('D', 'X', 'T', '1'),
		D3DFMT_DXT2                 = DDS_FourCC('D', 'X', 'T', '2'),
		D3DFMT_DXT3                 = DDS_FourCC('D', 'X', 'T', '3'),
		D3DFMT_DXT4                 = DDS_FourCC('D', 'X', 'T', '4'),
		D3DFMT_DXT5                 = DDS_FourCC('D', 'X', 'T', '5'),

		D3DFMT_D16_LOCKABLE         = 70,
		D3DFMT_D32                  = 71,
		D3DFMT_D15S1                = 73,
		D3DFMT_D24S8                = 75,
		D3DFMT_D24X8                = 77,
		D3DFMT_D24X4S4              = 79,
		D3DFMT_D16                  = 80,

		D3DFMT_D32F_LOCKABLE        = 82,
		D3DFMT_D24FS8               = 83,

		D3DFMT_L16                  = 81,

		D3DFMT_VERTEXDATA           = 100,
		D3DFMT_INDEX16              = 101,
		D3DFMT_INDEX32              = 102,

		D3DFMT_Q16W16V16U16         = 110,

		D3DFMT_MULTI2_ARGB8         = DDS_FourCC('M','E','T','1'),

		D3DFMT_R16F                 = 111,
		D3DFMT_G16R16F              = 112,
		D3DFMT_A16B16G16R16F        = 113,

		D3DFMT_R32F                 = 114,
		D3DFMT_G32R32F              = 115,
		D3DFMT_A32B32G32R32F        = 116,

		D3DFMT_CxV8U8               = 117,

		D3DFMT_DX10                 = DDS_FourCC('D', 'X', '1', '0')
	};

	enum DDPF
	{
		DDPF_ALPHAPIXELS = 0x00001,
		DDPF_ALPHA       = 0x00002,
		DDPF_FOURCC      = 0x00004,
		DDPF_RGB         = 0x00040,
		DDPF_YUV         = 0x00200,
		DDPF_LUMINANCE   = 0x20000
	};

	enum DDSD
	{
		DDSD_CAPS           = 0x00000001,
		DDSD_HEIGHT         = 0x00000002,
		DDSD_WIDTH          = 0x00000004,
		DDSD_PITCH          = 0x00000008,
		DDSD_PIXELFORMAT    = 0x00001000,
		DDSD_MIPMAPCOUNT    = 0x00020000,
		DDSD_LINEARSIZE     = 0x00080000,
		DDSD_DEPTH          = 0x00800000
	};

	enum DDSCAPS
	{
		DDSCAPS_COMPLEX = 0x00000008,
		DDSCAPS_MIPMAP  = 0x00400000,
		DDSCAPS_TEXTURE = 0x00001000
	};

	enum DDSCAPS2
	{
		DDSCAPS2_CUBEMAP           = 0x00000200,
		DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
		DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
		DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
		DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
		DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
		DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
		DDSCAPS2_VOLUME            = 0x00200000,

		DDSCAPS2_CUBEMAP_ALLFACES  = DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX |
		                             DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | 
		                             DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ
	};

	enum DDS_COLOR
	{
		DDS_COLOR_DEFAULT = 0,
		DDS_COLOR_DISTANCE,
		DDS_COLOR_LUMINANCE,
		DDS_COLOR_INSET_BBOX,
		DDS_COLOR_MAX
	};

	enum DDS_COMPRESS
	{
		DDS_COMPRESS_NONE = 0,
		DDS_COMPRESS_BC1,        /* DXT1  */
		DDS_COMPRESS_BC2,        /* DXT3  */
		DDS_COMPRESS_BC3,        /* DXT5  */
		DDS_COMPRESS_BC3N,       /* DXT5n */
		DDS_COMPRESS_BC4,        /* ATI1  */
		DDS_COMPRESS_BC5,        /* ATI2  */
		DDS_COMPRESS_AEXP,       /* DXT5  */
		DDS_COMPRESS_YCOCG,      /* DXT5  */
		DDS_COMPRESS_YCOCGS,     /* DXT5  */
		DDS_COMPRESS_MAX
	};

	enum DDS_FORMAT
	{
		DDS_FORMAT_DEFAULT = 0,
		DDS_FORMAT_RGB8,
		DDS_FORMAT_RGBA8,
		DDS_FORMAT_BGR8,
		DDS_FORMAT_ABGR8,
		DDS_FORMAT_R5G6B5,
		DDS_FORMAT_RGBA4,
		DDS_FORMAT_RGB5A1,
		DDS_FORMAT_RGB10A2,
		DDS_FORMAT_R3G3B2,
		DDS_FORMAT_A8,
		DDS_FORMAT_L8,
		DDS_FORMAT_L8A8,
		DDS_FORMAT_AEXP,
		DDS_FORMAT_YCOCG,
		DDS_FORMAT_MAX
	};

	enum DDS_MIPMAP
	{
		DDS_MIPMAP_DEFAULT = 0,
		DDS_MIPMAP_NEAREST,
		DDS_MIPMAP_BOX,
		DDS_MIPMAP_BILINEAR,
		DDS_MIPMAP_BICUBIC,
		DDS_MIPMAP_LANCZOS,
		DDS_MIPMAP_MAX
	};

	enum DDS_SAVE
	{
		DDS_SAVE_SELECTED_LAYER = 0,
		DDS_SAVE_CUBEMAP,
		DDS_SAVE_VOLUMEMAP,
		DDS_SAVE_MAX
	};

	enum DXGI_FORMAT
	{
		DXGI_FORMAT_UNKNOWN                      = 0,
		DXGI_FORMAT_R32G32B32A32_TYPELESS        = 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT           = 2,
		DXGI_FORMAT_R32G32B32A32_UINT            = 3,
		DXGI_FORMAT_R32G32B32A32_SINT            = 4,
		DXGI_FORMAT_R32G32B32_TYPELESS           = 5,
		DXGI_FORMAT_R32G32B32_FLOAT              = 6,
		DXGI_FORMAT_R32G32B32_UINT               = 7,
		DXGI_FORMAT_R32G32B32_SINT               = 8,
		DXGI_FORMAT_R16G16B16A16_TYPELESS        = 9,
		DXGI_FORMAT_R16G16B16A16_FLOAT           = 10,
		DXGI_FORMAT_R16G16B16A16_UNORM           = 11,
		DXGI_FORMAT_R16G16B16A16_UINT            = 12,
		DXGI_FORMAT_R16G16B16A16_SNORM           = 13,
		DXGI_FORMAT_R16G16B16A16_SINT            = 14,
		DXGI_FORMAT_R32G32_TYPELESS              = 15,
		DXGI_FORMAT_R32G32_FLOAT                 = 16,
		DXGI_FORMAT_R32G32_UINT                  = 17,
		DXGI_FORMAT_R32G32_SINT                  = 18,
		DXGI_FORMAT_R32G8X24_TYPELESS            = 19,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT         = 20,
		DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS     = 21,
		DXGI_FORMAT_X32_TYPELESS_G8X24_UINT      = 22,
		DXGI_FORMAT_R10G10B10A2_TYPELESS         = 23,
		DXGI_FORMAT_R10G10B10A2_UNORM            = 24,
		DXGI_FORMAT_R10G10B10A2_UINT             = 25,
		DXGI_FORMAT_R11G11B10_FLOAT              = 26,
		DXGI_FORMAT_R8G8B8A8_TYPELESS            = 27,
		DXGI_FORMAT_R8G8B8A8_UNORM               = 28,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB          = 29,
		DXGI_FORMAT_R8G8B8A8_UINT                = 30,
		DXGI_FORMAT_R8G8B8A8_SNORM               = 31,
		DXGI_FORMAT_R8G8B8A8_SINT                = 32,
		DXGI_FORMAT_R16G16_TYPELESS              = 33,
		DXGI_FORMAT_R16G16_FLOAT                 = 34,
		DXGI_FORMAT_R16G16_UNORM                 = 35,
		DXGI_FORMAT_R16G16_UINT                  = 36,
		DXGI_FORMAT_R16G16_SNORM                 = 37,
		DXGI_FORMAT_R16G16_SINT                  = 38,
		DXGI_FORMAT_R32_TYPELESS                 = 39,
		DXGI_FORMAT_D32_FLOAT                    = 40,
		DXGI_FORMAT_R32_FLOAT                    = 41,
		DXGI_FORMAT_R32_UINT                     = 42,
		DXGI_FORMAT_R32_SINT                     = 43,
		DXGI_FORMAT_R24G8_TYPELESS               = 44,
		DXGI_FORMAT_D24_UNORM_S8_UINT            = 45,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS        = 46,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT         = 47,
		DXGI_FORMAT_R8G8_TYPELESS                = 48,
		DXGI_FORMAT_R8G8_UNORM                   = 49,
		DXGI_FORMAT_R8G8_UINT                    = 50,
		DXGI_FORMAT_R8G8_SNORM                   = 51,
		DXGI_FORMAT_R8G8_SINT                    = 52,
		DXGI_FORMAT_R16_TYPELESS                 = 53,
		DXGI_FORMAT_R16_FLOAT                    = 54,
		DXGI_FORMAT_D16_UNORM                    = 55,
		DXGI_FORMAT_R16_UNORM                    = 56,
		DXGI_FORMAT_R16_UINT                     = 57,
		DXGI_FORMAT_R16_SNORM                    = 58,
		DXGI_FORMAT_R16_SINT                     = 59,
		DXGI_FORMAT_R8_TYPELESS                  = 60,
		DXGI_FORMAT_R8_UNORM                     = 61,
		DXGI_FORMAT_R8_UINT                      = 62,
		DXGI_FORMAT_R8_SNORM                     = 63,
		DXGI_FORMAT_R8_SINT                      = 64,
		DXGI_FORMAT_A8_UNORM                     = 65,
		DXGI_FORMAT_R1_UNORM                     = 66,
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP           = 67,
		DXGI_FORMAT_R8G8_B8G8_UNORM              = 68,
		DXGI_FORMAT_G8R8_G8B8_UNORM              = 69,
		DXGI_FORMAT_BC1_TYPELESS                 = 70,
		DXGI_FORMAT_BC1_UNORM                    = 71,
		DXGI_FORMAT_BC1_UNORM_SRGB               = 72,
		DXGI_FORMAT_BC2_TYPELESS                 = 73,
		DXGI_FORMAT_BC2_UNORM                    = 74,
		DXGI_FORMAT_BC2_UNORM_SRGB               = 75,
		DXGI_FORMAT_BC3_TYPELESS                 = 76,
		DXGI_FORMAT_BC3_UNORM                    = 77,
		DXGI_FORMAT_BC3_UNORM_SRGB               = 78,
		DXGI_FORMAT_BC4_TYPELESS                 = 79,
		DXGI_FORMAT_BC4_UNORM                    = 80,
		DXGI_FORMAT_BC4_SNORM                    = 81,
		DXGI_FORMAT_BC5_TYPELESS                 = 82,
		DXGI_FORMAT_BC5_UNORM                    = 83,
		DXGI_FORMAT_BC5_SNORM                    = 84,
		DXGI_FORMAT_B5G6R5_UNORM                 = 85,
		DXGI_FORMAT_B5G5R5A1_UNORM               = 86,
		DXGI_FORMAT_B8G8R8A8_UNORM               = 87,
		DXGI_FORMAT_B8G8R8X8_UNORM               = 88,
		DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM   = 89,
		DXGI_FORMAT_B8G8R8A8_TYPELESS            = 90,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB          = 91,
		DXGI_FORMAT_B8G8R8X8_TYPELESS            = 92,
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB          = 93,
		DXGI_FORMAT_BC6H_TYPELESS                = 94,
		DXGI_FORMAT_BC6H_UF16                    = 95,
		DXGI_FORMAT_BC6H_SF16                    = 96,
		DXGI_FORMAT_BC7_TYPELESS                 = 97,
		DXGI_FORMAT_BC7_UNORM                    = 98,
		DXGI_FORMAT_BC7_UNORM_SRGB               = 99
	};

	struct DDSPixelFormat // DDPIXELFORMAT
	{
		UInt32 size;
		UInt32 flags;
		UInt32 fourCC;
		UInt32 bpp;
		UInt32 redMask;
		UInt32 greenMask;
		UInt32 blueMask;
		UInt32 alphaMask;
	};

	struct DDSHeader
	{
		UInt32 size;
		UInt32 flags;
		UInt32 height;
		UInt32 width;
		UInt32 pitch;
		UInt32 depth;
		UInt32 levelCount;
		UInt32 reserved1[11];
		DDSPixelFormat format;
		UInt32 ddsCaps[4];
		UInt32 reserved2;
	};

	struct DDSHeaderDX10Ext
	{
		DXGI_FORMAT dxgiFormat;
		D3D10_RESOURCE_DIMENSION resourceDimension;
		UInt32 miscFlag;
		UInt32 arraySize;
		UInt32 reserved;
	};

	NAZARA_UTILITY_API bool Unserialize(SerializationContext& context, DDSHeader* header);
	NAZARA_UTILITY_API bool Unserialize(SerializationContext& context, DDSHeaderDX10Ext* header);
	NAZARA_UTILITY_API bool Unserialize(SerializationContext& context, DDSPixelFormat* pixelFormat);
}

#endif // NAZARA_LOADERS_DDS_CONSTANTS_HPP
