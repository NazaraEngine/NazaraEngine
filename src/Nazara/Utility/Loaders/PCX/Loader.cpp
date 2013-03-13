// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Debug.hpp>

// Auteur du loader original : David Henry

namespace
{
	struct pcx_header
	{
		nzUInt8 manufacturer;
		nzUInt8 version;
		nzUInt8 encoding;
		nzUInt8 bitsPerPixel;

		nzUInt16 xmin, ymin;
		nzUInt16 xmax, ymax;
		nzUInt16 horzRes, vertRes;

		nzUInt8 palette[48];
		nzUInt8 reserved;
		nzUInt8 numColorPlanes;

		nzUInt16 bytesPerScanLine;
		nzUInt16 paletteType;
		nzUInt16 horzSize, vertSize;

		nzUInt8 padding[54];
	};

	//static_assert(sizeof(pcx_header) == 1024, "PCX header must be 1024 bytes sized");

	bool Check(NzInputStream& stream, const NzImageParams& parameters)
	{
		NazaraUnused(parameters);

		nzUInt8 manufacturer;
		if (stream.Read(&manufacturer, 1) != 1)
			return false;

		return manufacturer == 0x0a;
	}

	bool Load(NzImage* image, NzInputStream& stream, const NzImageParams& parameters)
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
		NzByteSwap(&header.xmin, sizeof(nzUInt16));
		NzByteSwap(&header.ymin, sizeof(nzUInt16));
		NzByteSwap(&header.xmax, sizeof(nzUInt16));
		NzByteSwap(&header.ymax, sizeof(nzUInt16));
		NzByteSwap(&header.horzRes, sizeof(nzUInt16));
		NzByteSwap(&header.vertRes, sizeof(nzUInt16));

		NzByteSwap(&header.bytesPerScanLine, sizeof(nzUInt16));
		NzByteSwap(&header.paletteType, sizeof(nzUInt16));
		NzByteSwap(&header.horzSize, sizeof(nzUInt16));
		NzByteSwap(&header.vertSize, sizeof(nzUInt16));
		#endif

		unsigned int bitCount = header.bitsPerPixel * header.numColorPlanes;
		unsigned int width = header.xmax - header.xmin+1;
		unsigned int height = header.ymax - header.ymin+1;

		if (!image->Create(nzImageType_2D, nzPixelFormat_RGB8, width, height, 1, (parameters.levelCount > 0) ? parameters.levelCount : 1))
		{
			NazaraError("Failed to create image");
			return false;
		}

		nzUInt8* pixels = image->GetPixels();

		int rle_value = 0;
		unsigned int rle_count = 0;

		switch (bitCount)
		{
			case 1:
			{
				for (unsigned int y = 0; y < height; ++y)
				{
					nzUInt8* ptr = &pixels[y * width * 3];
					int bytes = header.bytesPerScanLine;

					/* decode line number y */
					while (bytes--)
					{
						if (rle_count == 0)
						{
							if (!stream.Read(&rle_value, 1))
							{
								NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
								return false;
							}

							if (rle_value < 0xc0)
								rle_count = 1;
							else
							{
								rle_count = rle_value - 0xc0;
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
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
				nzUInt8* colorIndex = new nzUInt8[width];
				nzUInt8* line = new nzUInt8[header.bytesPerScanLine];

				for (unsigned int y = 0; y < height; ++y)
				{
					nzUInt8* ptr = &pixels[y * width * 3];

					std::memset(colorIndex, 0, width);

					for (unsigned int c = 0; c < 4; ++c)
					{
						nzUInt8* pLine = line;
						int bytes = header.bytesPerScanLine;

						/* decode line number y */
						while (bytes--)
						{
							if (rle_count == 0)
							{
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
									delete[] colorIndex;
									delete[] line;

									return false;
								}

								if (rle_value < 0xc0)
									rle_count = 1;
								else
								{
									rle_count = rle_value - 0xc0;
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
										delete[] colorIndex;
										delete[] line;

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

				/* release memory */
				delete[] colorIndex;
				delete[] line;
				break;
			}

			case 8:
			{
				nzUInt8 palette[768];

				/* the palette is contained in the last 769 bytes of the file */
				unsigned int curPos = stream.GetCursorPos();
				stream.SetCursorPos(stream.GetSize()-769);
				nzUInt8 magic;
				if (!stream.Read(&magic, 1))
				{
					NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
					return false;
				}

				/* first byte must be equal to 0x0c (12) */
				if (magic != 0x0c)
				{
					NazaraError("Colormap's first byte must be 0x0c (0x" + NzString::Number(magic, 16) + ')');
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
					nzUInt8* ptr = &pixels[y * width * 3];
					int bytes = header.bytesPerScanLine;

					/* decode line number y */
					while (bytes--)
					{
						if (rle_count == 0)
						{
							if (!stream.Read(&rle_value, 1))
							{
								NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
								return false;
							}

							if (rle_value < 0xc0)
								rle_count = 1;
							else
							{
								rle_count = rle_value - 0xc0;
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
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
						nzUInt8* ptr = &pixels[y * width * 4];
						int bytes = header.bytesPerScanLine;

						/* decode line number y */
						while (bytes--)
						{
							if (rle_count == 0)
							{
								if (!stream.Read(&rle_value, 1))
								{
									NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
									return false;
								}

								if (rle_value < 0xc0)
									rle_count = 1;
								else
								{
									rle_count = rle_value - 0xc0;
									if (!stream.Read(&rle_value, 1))
									{
										NazaraError("Failed to read stream (byte " + NzString::Number(stream.GetCursorPos()) + ')');
										return false;
									}
								}
							}

							rle_count--;
							ptr[c] = static_cast<nzUInt8>(rle_value);
							ptr += 3;
						}
					}
				}
				break;
			}

			default:
				NazaraError("Failed to load " + NzString::Number(bitCount) + " bitcount pcx files");
				return false;
		}

		if (parameters.loadFormat != nzPixelFormat_Undefined)
			image->Convert(parameters.loadFormat);

		return true;
	}
}

void NzLoaders_PCX_Register()
{
	NzImageLoader::RegisterLoader("pcx", Check, Load);
}

void NzLoaders_PCX_Unregister()
{
	NzImageLoader::UnregisterLoader("pcx", Check, Load);
}
