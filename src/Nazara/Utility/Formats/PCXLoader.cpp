// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/PCXLoader.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

// Auteur du loader original : David Henry

namespace Nz
{
	namespace
	{
		struct pcx_header
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

		static_assert(sizeof(pcx_header) == (6+48+54)*sizeof(UInt8) + 10*sizeof(UInt16), "pcx_header struct must be packed");

		bool IsSupported(const String& extension)
		{
			return (extension == "pcx");
		}

		Ternary Check(Stream& stream, const ImageParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativePCXLoader", &skip) && skip)
				return Ternary_False;

			UInt8 manufacturer;
			if (stream.Read(&manufacturer, 1) == 1)
			{
				if (manufacturer == 0x0a)
					return Ternary_True;
			}

			return Ternary_False;
		}

		bool Load(Image* image, Stream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);

			pcx_header header;
			if (stream.Read(&header, sizeof(pcx_header)) != sizeof(pcx_header))
			{
				NazaraError("Failed to read header");
				return false;
			}

			#ifdef NAZARA_BIG_ENDIAN
			// Les fichiers PCX sont en little endian
			SwapBytes(&header.xmin, sizeof(UInt16));
			SwapBytes(&header.ymin, sizeof(UInt16));
			SwapBytes(&header.xmax, sizeof(UInt16));
			SwapBytes(&header.ymax, sizeof(UInt16));
			SwapBytes(&header.horzRes, sizeof(UInt16));
			SwapBytes(&header.vertRes, sizeof(UInt16));

			SwapBytes(&header.bytesPerScanLine, sizeof(UInt16));
			SwapBytes(&header.paletteType, sizeof(UInt16));
			SwapBytes(&header.horzSize, sizeof(UInt16));
			SwapBytes(&header.vertSize, sizeof(UInt16));
			#endif

			unsigned int bitCount = header.bitsPerPixel * header.numColorPlanes;
			unsigned int width = header.xmax - header.xmin+1;
			unsigned int height = header.ymax - header.ymin+1;

			if (!image->Create(ImageType_2D, PixelFormatType_RGB8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
			{
				NazaraError("Failed to create image");
				return false;
			}

			UInt8* pixels = image->GetPixels();

			int rle_value = 0;
			unsigned int rle_count = 0;

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
							if (rle_count == 0)
							{
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
									return false;
								}

								if (rle_value < 0xc0)
									rle_count = 1;
								else
								{
									rle_count = rle_value - 0xc0;
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
										return false;
									}
								}
							}

							rle_count--;

							for (int i = 7; i >= 0; --i)
							{
								int colorIndex = ((rle_value & (1 << i)) > 0);

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
								if (rle_count == 0)
								{
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
										return false;
									}

									if (rle_value < 0xc0)
										rle_count = 1;
									else
									{
										rle_count = rle_value - 0xc0;
										if (!stream.Read(&rle_value, 1))
										{
											NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
											return false;
										}
									}
								}

								rle_count--;
								*(pLine++) = rle_value;
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
						NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
						return false;
					}

					/* first byte must be equal to 0x0c (12) */
					if (magic != 0x0c)
					{
						NazaraError("Colormap's first byte must be 0x0c (0x" + String::Number(magic, 16) + ')');
						return false;
					}

					/* read palette */
					if (stream.Read(palette, 768) != 768)
					{
						NazaraError("Failed to read palette");
						return false;
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
							if (rle_count == 0)
							{
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
									return false;
								}

								if (rle_value < 0xc0)
									rle_count = 1;
								else
								{
									rle_count = rle_value - 0xc0;
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
										return false;
									}
								}
							}

							rle_count--;

							*ptr++ = palette[rle_value * 3 + 0];
							*ptr++ = palette[rle_value * 3 + 1];
							*ptr++ = palette[rle_value * 3 + 2];
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
								if (rle_count == 0)
								{
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
										return false;
									}

									if (rle_value < 0xc0)
										rle_count = 1;
									else
									{
										rle_count = rle_value - 0xc0;
										if (!stream.Read(&rle_value, 1))
										{
											NazaraError("Failed to read stream (byte " + String::Number(stream.GetCursorPos()) + ')');
											return false;
										}
									}
								}

								rle_count--;
								ptr[c] = static_cast<UInt8>(rle_value);
								ptr += 3;
							}
						}
					}
					break;
				}

				default:
					NazaraError("Unsupported " + String::Number(bitCount) + " bitcount for pcx files");
					return false;
			}

			if (parameters.loadFormat != PixelFormatType_Undefined)
				image->Convert(parameters.loadFormat);

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterPCX()
		{
			ImageLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterPCX()
		{
			ImageLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
