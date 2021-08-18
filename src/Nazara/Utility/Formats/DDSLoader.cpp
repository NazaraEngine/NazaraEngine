// Copyright (C) 2020 Jérôme Leclercq - 2009 Cruden BV
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/DDSLoader.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Formats/DDSConstants.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	class DDSLoader
	{
		public:
			DDSLoader() = delete;
			~DDSLoader() = delete;

			static bool IsSupported(const std::string_view& extension)
			{
				return (extension == "dds");
			}

			static Ternary Check(Stream& stream, const ImageParams& parameters)
			{
				bool skip;
				if (parameters.custom.GetBooleanParameter("SkipNativeDDSLoader", &skip) && skip)
					return Ternary::False;

				ByteStream byteStream(&stream);
				byteStream.SetDataEndianness(Endianness::LittleEndian);

				UInt32 magic;
				byteStream >> magic;

				return (magic == DDS_Magic) ? Ternary::True : Ternary::False;
			}

			static std::shared_ptr<Image> Load(Stream& stream, const ImageParams& parameters)
			{
				NazaraUnused(parameters);

				ByteStream byteStream(&stream);
				byteStream.SetDataEndianness(Endianness::LittleEndian);

				UInt32 magic;
				byteStream >> magic;
				NazaraAssert(magic == DDS_Magic, "Invalid DDS file"); // The Check function should make sure this doesn't happen

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

				unsigned int width = std::max(header.width, 1U);

				unsigned int height = 1U;
				if (header.flags & DDSD_HEIGHT)
					height = std::max(header.height, 1U);

				unsigned int depth = 1U;
				if (header.flags & DDSD_DEPTH)
					depth = std::max(header.depth, 1U);

				unsigned int levelCount = (parameters.levelCount > 0) ? std::min(parameters.levelCount, static_cast<UInt8>(header.levelCount)) : header.levelCount;

				// First, identify the type
				ImageType type;
				if (!IdentifyImageType(header, headerDX10, &type))
					return nullptr;

				// Then the format
				PixelFormat format;
				if (!IdentifyPixelFormat(header, headerDX10, &format))
					return nullptr;

				std::shared_ptr<Image> image = std::make_shared<Image>(type, format, width, height, depth, levelCount);

				// Read all mipmap levels
				for (unsigned int i = 0; i < image->GetLevelCount(); i++)
				{
					std::size_t byteCount = PixelFormatInfo::ComputeSize(format, width, height, depth);

					UInt8* ptr = image->GetPixels(0, 0, 0, i);

					if (byteStream.Read(ptr, byteCount) != byteCount)
					{
						NazaraError("Failed to read level #" + NumberToString(i));
						return nullptr;
					}

					if (width > 1)
						width >>= 1;

					if (height > 1)
						height >>= 1;

					if (depth > 1)
						depth >>= 1;
				}


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
						NazaraError("Cubemap arrays are not yet supported, sorry");
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
							NazaraError("Partial cubemap are not yet supported, sorry");
							return false;
						}

						*type = ImageType::Cubemap;
					}
					else if (headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_BUFFER)
					{
						NazaraError("Texture buffers are not yet supported, sorry");
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
					PixelFormatDescription info(PixelFormatContent::ColorRGBA, header.format.bpp, PixelFormatSubType::Unsigned);

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
							buf[0] = (header.format.fourCC >> 0) & 255;
							buf[1] = (header.format.fourCC >> 8) & 255;
							buf[2] = (header.format.fourCC >> 16) & 255;
							buf[3] = (header.format.fourCC >> 24) & 255;
							buf[4] = '\0';

							NazaraError("Unhandled format \"" + std::string(buf) + "\"");
							return false;
						}
					}
				}
				else
				{
					NazaraError("Invalid DDS file");
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
			loaderEntry.streamChecker = DDSLoader::Check;
			loaderEntry.streamLoader = DDSLoader::Load;

			return loaderEntry;
		}
	}
}
