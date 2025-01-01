// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com) - 2009 Cruden BV
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/DDSLoader.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/Formats/DDSConstants.hpp>

namespace Nz
{
	class DDSLoader
	{
		public:
			DDSLoader() = delete;
			~DDSLoader() = delete;

			static bool IsSupported(std::string_view extension)
			{
				return (extension == ".dds");
			}

			static Result<std::shared_ptr<Image>, ResourceLoadingError> Load(Stream& stream, const ImageParams& parameters)
			{
				ByteStream byteStream(&stream);
				byteStream.SetDataEndianness(Endianness::LittleEndian);

				UInt32 magic;
				byteStream >> magic;
				if (magic != DDS_Magic)
					return Nz::Err(ResourceLoadingError::Unrecognized);

				DDSHeader header;
				byteStream >> header;

				DDSHeaderDX10Ext headerDX10;
				if (header.format.flags & DDPF_FOURCC && header.format.fourCC == D3DFMT_DX10)
					byteStream >> headerDX10;
				else
				{
					headerDX10.arraySize = 1;
					headerDX10.dxgiFormat = DXGI_FORMAT_UNKNOWN;
					headerDX10.miscFlag = 0;
					headerDX10.resourceDimension = D3D10_RESOURCE_DIMENSION_UNKNOWN;
				}

				if ((header.flags & DDSD_WIDTH) == 0)
					NazaraWarning("Ill-formed DDS file, doesn't have a width flag");

				UInt32 width = std::max(header.width, 1U);

				UInt32 height = 1U;
				if (header.flags & DDSD_HEIGHT)
					height = std::max(header.height, 1U);

				UInt32 depth = 1U;
				if (header.flags & DDSD_DEPTH)
					depth = std::max(header.depth, 1U);

				UInt8 levelCount = (parameters.levelCount > 0) ? std::min(parameters.levelCount, SafeCast<UInt8>(header.levelCount)) : SafeCast<UInt8>(header.levelCount);

				// First, identify the type
				ImageType type;
				if (!IdentifyImageType(header, headerDX10, &type))
					return Nz::Err(ResourceLoadingError::Unsupported);

				// Then the format
				PixelFormat format;
				if (!IdentifyPixelFormat(header, headerDX10, &format))
					return Nz::Err(ResourceLoadingError::Unsupported);

				std::shared_ptr<Image> image = std::make_shared<Image>(type, format, width, height, depth, levelCount);

				// Read all mipmap levels
				bool succeeded = ImageUtils::ForEachLevel(levelCount, type, width, height, depth, [&](UInt8 level, UInt32 width, UInt32 height, UInt32 depth)
				{
					std::size_t byteCount = PixelFormatInfo::ComputeSize(format, width, height, depth);

					if (!parameters.levels.test(level))
					{
						byteStream.Read(nullptr, byteCount);
						return true;
					}

					UInt8* ptr = image->GetPixels(0, 0, 0, level);

					if (byteStream.Read(ptr, byteCount) != byteCount)
					{
						NazaraError("failed to read level #{0}", level);
						return false;
					}

					return true;
				});

				if (!succeeded)
					return Nz::Err(ResourceLoadingError::DecodingError);

				if (parameters.loadFormat != PixelFormat::Undefined)
					image->Convert(parameters.loadFormat);

				return image;
			}

		private:
			static bool IdentifyImageType(const DDSHeader& header, const DDSHeaderDX10Ext& headerExt, ImageType* type)
			{
				if (headerExt.arraySize > 1)
				{
					if (header.ddsCaps[1] & DDSCAPS2_CUBEMAP)
					{
						NazaraError("cubemap arrays are not yet supported, sorry");
						return false;
					}
					else if (header.flags & DDSD_HEIGHT)
						*type = ImageType::E2D_Array;
					else
						*type = ImageType::E1D_Array;
				}
				else
				{
					if (header.ddsCaps[1] & DDSCAPS2_CUBEMAP)
					{
						if ((header.ddsCaps[1] & DDSCAPS2_CUBEMAP_ALLFACES) != DDSCAPS2_CUBEMAP_ALLFACES)
						{
							NazaraError("partial cubemap are not yet supported, sorry");
							return false;
						}

						*type = ImageType::Cubemap;
					}
					else if (headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_BUFFER)
					{
						NazaraError("texture buffers are not yet supported, sorry");
						return false;
					}
					else if (headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE1D)
						*type = ImageType::E1D;
					else if (header.ddsCaps[1] & DDSCAPS2_VOLUME || header.flags & DDSD_DEPTH || headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D)
						*type = ImageType::E3D;
					else
						*type = ImageType::E2D;
				}

				return true;
			}

			static bool IdentifyPixelFormat(const DDSHeader& header, const DDSHeaderDX10Ext& headerExt, PixelFormat* format)
			{
				if (header.format.flags & (DDPF_RGB | DDPF_ALPHA | DDPF_ALPHAPIXELS | DDPF_LUMINANCE))
				{
					PixelFormatDescription info(PixelFormatContent::ColorRGBA, SafeCast<UInt8>(header.format.bpp), PixelFormatSubType::Unsigned);

					if (header.format.flags & DDPF_RGB)
					{
						// Reverse bits for our masks
						info.redMask = header.format.redMask;
						info.greenMask = header.format.greenMask;
						info.blueMask = header.format.blueMask;
					}
					else if (header.format.flags & DDPF_LUMINANCE)
						info.redMask = header.format.redMask;

					if (header.format.flags & (DDPF_ALPHA | DDPF_ALPHAPIXELS))
						info.alphaMask = header.format.alphaMask;

					*format = PixelFormatInfo::IdentifyFormat(info);
					if (!PixelFormatInfo::IsValid(*format))
						return false;
				}
				else if (header.format.flags & DDPF_FOURCC)
				{
					switch (header.format.fourCC)
					{
						case D3DFMT_DXT1:
							*format = PixelFormat::DXT1;
							break;

						case D3DFMT_DXT3:
							*format = PixelFormat::DXT3;
							break;

						case D3DFMT_DXT5:
							*format = PixelFormat::DXT3;
							break;

						case D3DFMT_DX10:
						{
							switch (headerExt.dxgiFormat)
							{
								case DXGI_FORMAT_R32G32B32A32_FLOAT:
									*format = PixelFormat::RGBA32F;
									break;
								case DXGI_FORMAT_R32G32B32A32_UINT:
									*format = PixelFormat::RGBA32UI;
									break;
								case DXGI_FORMAT_R32G32B32A32_SINT:
									*format = PixelFormat::RGBA32I;
									break;
								case DXGI_FORMAT_R32G32B32_FLOAT:
									*format = PixelFormat::RGB32F;
									break;
								case DXGI_FORMAT_R32G32B32_UINT:
									//*format = PixelFormat::RGB32U;
									return false;
								case DXGI_FORMAT_R32G32B32_SINT:
									*format = PixelFormat::RGB32I;
									break;
								case DXGI_FORMAT_R16G16B16A16_SNORM:
								case DXGI_FORMAT_R16G16B16A16_SINT:
								case DXGI_FORMAT_R16G16B16A16_UINT:
									*format = PixelFormat::RGBA16I;
									break;
								case DXGI_FORMAT_R16G16B16A16_UNORM:
									*format = PixelFormat::RGBA16UI;
									break;

								default:
									//TODO
									NazaraError("TODO");
									break;
							}
							break;
						}

						default:
						{
							char buf[5];
							buf[0] = (header.format.fourCC >>  0) & 255;
							buf[1] = (header.format.fourCC >>  8) & 255;
							buf[2] = (header.format.fourCC >> 16) & 255;
							buf[3] = (header.format.fourCC >> 24) & 255;
							buf[4] = '\0';

							NazaraError("unhandled format \"{0}\"", buf);
							return false;
						}
					}
				}
				else
				{
					NazaraError("invalid DDS file");
					return false;
				}

				return true;
			}
	};

	namespace Loaders
	{
		ImageLoader::Entry GetImageLoader_DDS()
		{
			ImageLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = DDSLoader::IsSupported;
			loaderEntry.streamLoader = DDSLoader::Load;
			loaderEntry.parameterFilter = [](const ImageParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinDDSLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
