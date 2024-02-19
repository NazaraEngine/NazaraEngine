// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/PCXLoader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <memory>

// Auteur du loader original : David Henry

namespace Nz
{
	namespace
	{
		struct PCXHeader
		{
			UInt8 manufacturer;
			UInt8 version;
			UInt8 encoding;
			UInt8 bitsPerPixel;

			UInt16 xmin, ymin;
			UInt16 xmax, ymax;
			UInt16 horzRes, vertRes;

			UInt8 palette[48];
			UInt8 reserved;
			UInt8 numColorPlanes;

			UInt16 bytesPerScanLine;
			UInt16 paletteType;
			UInt16 horzSize, vertSize;

			UInt8 padding[54];
		};

		static_assert(sizeof(PCXHeader) == (6+48+54)*sizeof(UInt8) + 10*sizeof(UInt16), "pcx_header struct must be packed");

		bool IsPCXSupported(std::string_view extension)
		{
			return (extension == ".pcx");
		}

		Result<std::shared_ptr<Image>, ResourceLoadingError> LoadPCX(Stream& stream, const ImageParams& parameters)
		{
			PCXHeader header;
			if (stream.Read(&header, sizeof(PCXHeader)) != sizeof(PCXHeader))
				return Err(ResourceLoadingError::Unrecognized);

			if (header.manufacturer != 0x0a)
				return Err(ResourceLoadingError::Unrecognized);

#ifdef NAZARA_BIG_ENDIAN
			// PCX files are little-endian
			header.xmin = ByteSwap(header.xmin);
			header.ymin = ByteSwap(header.ymin);
			header.xmax = ByteSwap(header.xmax);
			header.ymax = ByteSwap(header.ymax);
			header.horzRes = ByteSwap(header.horzRes);
			header.vertRes = ByteSwap(header.vertRes);

			header.bytesPerScanLine = ByteSwap(header.bytesPerScanLine);
			header.paletteType = ByteSwap(header.paletteType);
			header.horzSize = ByteSwap(header.horzSize);
			header.vertSize = ByteSwap(header.vertSize);
#endif

			unsigned int bitCount = header.bitsPerPixel * header.numColorPlanes;
			unsigned int width = header.xmax - header.xmin+1;
			unsigned int height = header.ymax - header.ymin+1;

			std::shared_ptr<Image> image = std::make_shared<Image>();
			if (!image->Create(ImageType::E2D, PixelFormat::RGB8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
			{
				NazaraError("failed to create image");
				return Err(ResourceLoadingError::Internal);
			}

			UInt8* pixels = image->GetPixels();

			UInt8 rleValue = 0;
			UInt8 rleCount = 0;

			switch (bitCount)
			{
				case 1:
				{
					for (unsigned int y = 0; y < height; ++y)
					{
						UInt8* ptr = &pixels[y * width * 3];
						int bytes = header.bytesPerScanLine;

						/* decode line number y */
						while (bytes--)
						{
							if (rleCount == 0)
							{
								if (!stream.Read(&rleValue, 1))
								{
									NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
									return Err(ResourceLoadingError::DecodingError);
								}

								if (rleValue < 0xc0)
									rleCount = 1;
								else
								{
									rleCount = rleValue - 0xc0;
									if (!stream.Read(&rleValue, 1))
									{
										NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
										return Err(ResourceLoadingError::DecodingError);
									}
								}
							}

							rleCount--;

							for (int i = 7; i >= 0; --i)
							{
								int colorIndex = ((rleValue & (1 << i)) > 0);

								*ptr++ = header.palette[colorIndex * 3 + 0];
								*ptr++ = header.palette[colorIndex * 3 + 1];
								*ptr++ = header.palette[colorIndex * 3 + 2];
							}
						}
					}
					break;
				}

				case 4:
				{
					std::unique_ptr<UInt8[]> colorIndex(new UInt8[width]);
					std::unique_ptr<UInt8[]> line(new UInt8[header.bytesPerScanLine]);

					for (unsigned int y = 0; y < height; ++y)
					{
						UInt8* ptr = &pixels[y * width * 3];

						std::memset(colorIndex.get(), 0, width);

						for (unsigned int c = 0; c < 4; ++c)
						{
							UInt8* pLine = line.get();
							int bytes = header.bytesPerScanLine;

							/* decode line number y */
							while (bytes--)
							{
								if (rleCount == 0)
								{
									if (!stream.Read(&rleValue, 1))
									{
										NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
										return Err(ResourceLoadingError::DecodingError);
									}

									if (rleValue < 0xc0)
										rleCount = 1;
									else
									{
										rleCount = rleValue - 0xc0;
										if (!stream.Read(&rleValue, 1))
										{
											NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
											return Err(ResourceLoadingError::DecodingError);
										}
									}
								}

								rleCount--;
								*(pLine++) = rleValue;
							}

							/* compute line's color indexes */
							for (unsigned int x = 0; x < width; ++x)
							{
								if (line[x / 8] & (128 >> (x % 8)))
									colorIndex[x] += (1 << c);
							}
						}

						/* decode scan line.  color index => rgb  */
						for (unsigned int x = 0; x < width; ++x)
						{
							*ptr++ = header.palette[colorIndex[x] * 3 + 0];
							*ptr++ = header.palette[colorIndex[x] * 3 + 1];
							*ptr++ = header.palette[colorIndex[x] * 3 + 2];
						}
					}

					break;
				}

				case 8:
				{
					UInt8 palette[768];

					/* the palette is contained in the last 769 bytes of the file */
					UInt64 curPos = stream.GetCursorPos();
					stream.SetCursorPos(stream.GetSize()-769);
					UInt8 magic;
					if (!stream.Read(&magic, 1))
					{
						NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
						return Err(ResourceLoadingError::DecodingError);
					}

					/* first byte must be equal to 0x0c (12) */
					if (magic != 0x0c)
					{
						NazaraErrorFmt("Colormap's first byte must be 0x0c ({0:#x})", magic);
						return Err(ResourceLoadingError::DecodingError);
					}

					/* read palette */
					if (stream.Read(palette, 768) != 768)
					{
						NazaraError("failed to read palette");
						return Err(ResourceLoadingError::DecodingError);
					}

					stream.SetCursorPos(curPos);

					/* read pixel data */
					for (unsigned int y = 0; y < height; ++y)
					{
						UInt8* ptr = &pixels[y * width * 3];
						int bytes = header.bytesPerScanLine;

						/* decode line number y */
						while (bytes--)
						{
							if (rleCount == 0)
							{
								if (!stream.Read(&rleValue, 1))
								{
									NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
									return Err(ResourceLoadingError::DecodingError);
								}

								if (rleValue < 0xc0)
									rleCount = 1;
								else
								{
									rleCount = rleValue - 0xc0;
									if (!stream.Read(&rleValue, 1))
									{
										NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
										return Err(ResourceLoadingError::DecodingError);
									}
								}
							}

							rleCount--;

							*ptr++ = palette[rleValue * 3 + 0];
							*ptr++ = palette[rleValue * 3 + 1];
							*ptr++ = palette[rleValue * 3 + 2];
						}
					}
					break;
				}

				case 24:
				{
					for (unsigned int y = 0; y < height; ++y)
					{
						/* for each color plane */
						for (int c = 0; c < 3; ++c)
						{
							UInt8* ptr = &pixels[y * width * 3];
							int bytes = header.bytesPerScanLine;

							/* decode line number y */
							while (bytes--)
							{
								if (rleCount == 0)
								{
									if (!stream.Read(&rleValue, 1))
									{
										NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
										return Err(ResourceLoadingError::DecodingError);
									}

									if (rleValue < 0xc0)
										rleCount = 1;
									else
									{
										rleCount = rleValue - 0xc0;
										if (!stream.Read(&rleValue, 1))
										{
											NazaraErrorFmt("failed to read stream (byte {0})", stream.GetCursorPos());
											return Err(ResourceLoadingError::DecodingError);
										}
									}
								}

								rleCount--;
								ptr[c] = static_cast<UInt8>(rleValue);
								ptr += 3;
							}
						}
					}
					break;
				}

				default:
					NazaraErrorFmt("unsupported {0} bitcount for pcx files", bitCount);
					return Err(ResourceLoadingError::DecodingError);
			}

			if (parameters.loadFormat != PixelFormat::Undefined)
				image->Convert(parameters.loadFormat);

			return image;
		}
	}

	namespace Loaders
	{
		ImageLoader::Entry GetImageLoader_PCX()
		{
			ImageLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = IsPCXSupported;
			loaderEntry.streamLoader = LoadPCX;
			loaderEntry.parameterFilter = [](const ImageParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinPCXLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
