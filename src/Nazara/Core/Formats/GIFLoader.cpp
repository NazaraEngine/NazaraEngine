// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/GIFLoader.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Formats/STBLoader.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/Endianness.hpp>

// based on https://www.w3.org/Graphics/GIF/spec-gif89a.txt, with help from the following public domain libraries source code:
// - https://github.com/lecram/gifdec
// - https://github.com/nothings/stb/blob/master/stb_image.h

namespace Nz
{
	namespace
	{
		constexpr UInt8 DisposeToBackground = 2;
		constexpr UInt8 DisposeToPrevious = 3;

		class GIFImageStream : public ImageStream
		{
			public:
				GIFImageStream()
				{
					m_byteStream.SetDataEndianness(Endianness::LittleEndian);
				}

				~GIFImageStream()
				{
				}

				bool Check()
				{
					std::array<UInt8, 6> header; //< 3 bytes for signature + 3 bytes for version (87a and 89a supported)
					if (m_byteStream.Read(header.data(), header.size()) != header.size())
						return false;

					if (std::memcmp(&header[0], "GIF", 3) != 0)
						return false;

					if (std::memcmp(&header[3], "87a", 3) != 0 && std::memcmp(&header[3], "89a", 3) != 0)
						return false;

					return true;
				}

				bool DecodeNextFrame(void* frameBuffer, Time* frameTime) override
				{
					if (m_currentFrame >= m_frames.size())
					{
						if (frameTime)
							*frameTime = m_endFrameTime;

						return false;
					}

					UInt8* outputImage = static_cast<UInt8*>(frameBuffer);
					auto& frameData = m_frames[m_currentFrame];

					if (frameTime)
						*frameTime = frameData.time;

					UInt16 left;
					UInt16 top;
					UInt16 width;
					UInt16 height;
					UInt8 flag;

					m_byteStream.GetStream()->SetCursorPos(frameData.streamOffset);
					m_byteStream >> left >> top >> width >> height >> flag;

					ImageDecodingData decodingData;
					decodingData.lineSize = m_header.width * 4;
					decodingData.startX = left * 4;
					decodingData.startY = top * decodingData.lineSize;
					decodingData.maxX = decodingData.startX + width * 4;
					decodingData.maxY = decodingData.startY + decodingData.lineSize * height;
					decodingData.currentX = decodingData.startX;
					decodingData.currentY = decodingData.startY;

					// Render to previous frame if frame history is required
					if (m_requiresFrameHistory)
						decodingData.outputImage = m_previousFrame.get();
					else
						decodingData.outputImage = outputImage;

					std::size_t pixelCount = m_header.width * m_header.height;

					if (m_currentFrame == 0)
					{
						if (m_requiresFrameHistory)
							std::memset(m_previousFrame.get(), 0, pixelCount * 4);
						else if (outputImage)
							std::memset(outputImage, 0, pixelCount * 4);

						if (m_disposedRendering)
							std::memset(m_disposedRendering.get(), 0, pixelCount * 4);
					}
					else if (m_requiresFrameHistory)
					{
						if (m_frames[m_currentFrame - 1].disposalMethod == DisposeToBackground)
						{
							// FIXME: Is background color something else than transparent?
							std::array<UInt8, 4> backgroundColor;
							backgroundColor.fill(0);

							// restore affected pixels to background
							for (std::size_t i = 0; i < pixelCount; ++i)
							{
								if (m_affectedPixels[i])
									std::memcpy(&m_previousFrame[i * 4], &backgroundColor[0], 4);
							}
						}
						else if (m_frames[m_currentFrame - 1].disposalMethod == DisposeToPrevious)
						{
							// restore affected pixels to frame N - 2
							for (std::size_t i = 0; i < pixelCount; ++i)
							{
								if (m_affectedPixels[i])
									std::memcpy(&m_previousFrame[i * 4], &m_disposedRendering[i * 4], 4);
							}
						}

						if (m_disposedRendering)
							std::memcpy(&m_disposedRendering[0], &m_previousFrame[0], pixelCount * 4);
					}
					else if (m_frames[m_currentFrame - 1].disposalMethod == DisposeToBackground)
					{
						// Special case where each frame dispose to background but does full rendering
						// simply clear to transparent
						if (outputImage)
							std::memset(outputImage, 0, pixelCount * 4);
					}

					// if the width of the specified rectangle is 0, that means
					// we may not see *any* pixels or the image is malformed;
					// to make sure this is caught, move the current y down to
					// max_y (which is what out_gif_code checks).
					if (width == 0)
						decodingData.currentY = decodingData.maxY;

					bool interlace = (flag & 0b0100'0000);
					if (interlace)
					{
						decodingData.step = 8 * decodingData.lineSize;
						decodingData.parseMode = 3;
					}
					else
					{
						decodingData.step = decodingData.lineSize;
						decodingData.parseMode = 0;
					}

					bool hasLocalColorTable = (flag & 0b1000'0000);
					if (hasLocalColorTable)
					{
						UInt16 numEntries = 2ULL << (flag & 0b0000'0111);
						m_localColorTable.resize(numEntries);
						for (std::size_t i = 0; i < numEntries; ++i)
						{
							m_byteStream >> m_localColorTable[i].r >> m_localColorTable[i].g >> m_localColorTable[i].b;
							m_localColorTable[i].a = 0xFF;
						}

						decodingData.colorTable = &m_localColorTable[0];
						decodingData.transparentColorIndex = frameData.transparentIndex;
					}
					else if (!m_globalColorTable.empty())
					{
						decodingData.colorTable = &m_globalColorTable[0];
						decodingData.transparentColorIndex = frameData.transparentIndex;
					}
					else
					{
						// this error should have been caught already when loading
						NazaraInternalError("expected color table");
						return false;
					}

					UInt8 minimumCodeSize;
					m_byteStream >> minimumCodeSize;
					if (minimumCodeSize > 12)
					{
						NazaraInternalError("unexpected LZW Minimum Code Size ({0})", minimumCodeSize);
						return false;
					}

					if (decodingData.outputImage)
					{
						if (!DecodeImageDescriptor(minimumCodeSize, decodingData))
							return false;
					}
					else
						SkipUntilTerminationBlock();

					if (m_currentFrame == 0)
					{
						// if first frame, any pixel not drawn to gets the background color
						if (!m_globalColorTable.empty())
						{
							for (std::size_t i = 0; i < pixelCount; ++i)
							{
								if (!m_affectedPixels[i])
								{
									UInt8* outputPixel = &outputImage[i * 4];
									outputPixel[0] = m_globalColorTable[m_header.backgroundPaletteIndex].r;
									outputPixel[1] = m_globalColorTable[m_header.backgroundPaletteIndex].g;
									outputPixel[2] = m_globalColorTable[m_header.backgroundPaletteIndex].b;
									outputPixel[3] = m_globalColorTable[m_header.backgroundPaletteIndex].a;
								}
							}
						}
					}

					if (outputImage && decodingData.outputImage != outputImage)
						std::memcpy(outputImage, decodingData.outputImage, pixelCount * 4);

					m_currentFrame++;
					return true;
				}

				UInt64 GetFrameCount() const override
				{
					return m_frames.size();
				}

				PixelFormat GetPixelFormat() const override
				{
					return PixelFormat::RGBA8; //< TODO: Set SRGB
				}

				Vector2ui GetSize() const override
				{
					return Vector2ui(m_header.width, m_header.height);
				}

				void Seek(UInt64 frameIndex) override
				{
					assert(frameIndex <= m_frames.size());

					if (m_currentFrame == frameIndex)
						return;

					if (m_requiresFrameHistory)
					{
						if (m_currentFrame > frameIndex)
							m_currentFrame = 0;

						while (m_currentFrame < frameIndex)
							DecodeNextFrame(nullptr, nullptr);
					}
					else
						m_currentFrame = frameIndex;
				}

				UInt64 Tell() override
				{
					return m_currentFrame;
				}

				Result<void, ResourceLoadingError> Open()
				{
					if (!Check())
						return Err(ResourceLoadingError::Unrecognized);

					m_byteStream >> m_header.width >> m_header.height;
					m_byteStream >> m_header.flags >> m_header.backgroundPaletteIndex >> m_header.ratio;

					bool hasGlobalColorTable = (m_header.flags & 0b1000'0000);
					if (hasGlobalColorTable)
					{
						std::size_t numEntries = 2ULL << (m_header.flags & 0b0000'0111);
						m_globalColorTable.resize(numEntries);
						for (std::size_t i = 0; i < numEntries; ++i)
						{
							m_byteStream >> m_globalColorTable[i].r >> m_globalColorTable[i].g >> m_globalColorTable[i].b;
							m_globalColorTable[i].a = 0xFF;
						}
					}

					m_frames.clear();
					m_requiresFrameHistory = false;
					bool hasDisposeToPrevious = false;
					bool hasPartialRendering = false;
					bool terminated = false;

					Time frameTime = Time::Zero();

					FrameMetadata nextFrame;
					while (!terminated)
					{
						UInt8 tag;
						m_byteStream >> tag;

						switch (tag)
						{
							case 0: //< empty block?
								break;

							case 0x2C: //< image descriptor tag
							{
								nextFrame.streamOffset = m_byteStream.GetStream()->GetCursorPos();

								m_frames.push_back(nextFrame);
								nextFrame = {};

								UInt16 left;
								UInt16 top;
								UInt16 width;
								UInt16 height;
								UInt8 flag;

								m_byteStream >> left >> top >> width >> height >> flag;

								if (left + width > m_header.width)
								{
									NazaraError("corrupt gif (out of range)");
									return Err(ResourceLoadingError::DecodingError);
								}

								if (top + height > m_header.height)
								{
									NazaraError("corrupt gif (out of range)");
									return Err(ResourceLoadingError::DecodingError);
								}

								if (left != 0 || top != 0 || width < m_header.width || height < m_header.height)
									hasPartialRendering = true;

								if (flag & 0b1000'0000)
								{
									// has local color table
									UInt16 colorTableSize = 2ULL << (flag & 0b0000'0111);
									m_byteStream.Read(nullptr, colorTableSize * 3);
								}
								else if (!hasGlobalColorTable)
								{
									NazaraError("corrupt gif (no color table for image #{0}", m_frames.size() - 1);
									return Err(ResourceLoadingError::DecodingError);
								}

								UInt8 minimumCodeSize;
								m_byteStream >> minimumCodeSize;
								if (minimumCodeSize > 12)
								{
									NazaraError("unexpected LZW Minimum Code Size ({0})", minimumCodeSize);
									return Err(ResourceLoadingError::DecodingError);
								}

								SkipUntilTerminationBlock();
								break;
							}

							case 0x3B: //< end of file
								terminated = true;
								break;

							case 0x21: //< extension tag
							{
								UInt8 label;
								m_byteStream >> label;
								switch (label)
								{
									case 0xF9: //< graphic control extension
									{
										UInt8 blockSize;
										UInt8 flags;
										UInt16 delay;

										m_byteStream >> blockSize >> flags >> delay;

										if (delay == 0)
											delay = 10;

										if (blockSize != 4)
										{
											NazaraError("corrupt gif (invalid block size for graphic control extension)");
											return Err(ResourceLoadingError::DecodingError);
										}

										nextFrame.disposalMethod = (flags & 0b0001'1100) >> 2;
										nextFrame.time = frameTime;
										frameTime += Time::Milliseconds(delay * 10);

										if (flags & 0b0000'0001)
										{
											UInt8 transparentIndex;
											m_byteStream >> transparentIndex;

											nextFrame.transparentIndex = transparentIndex;
										}

										if (nextFrame.disposalMethod == DisposeToPrevious)
											hasDisposeToPrevious = true;

										break;
									}

									case 0xFE: //< comment extension
										break;

									case 0x01: //< plain text extension
										break;

									case 0xFF: //< application extension
										break;

									default:
										NazaraWarning("unrecognized extension label (unknown tag {0:#x}", label);
										break;
								}

								SkipUntilTerminationBlock();
								break;
							}

							default:
								NazaraError("corrupt gif (unknown tag {0:#x}", tag);
								return Err(ResourceLoadingError::DecodingError);
						}
					}

					if (hasDisposeToPrevious || hasPartialRendering)
						m_requiresFrameHistory = true;

					m_endFrameTime = frameTime;

					m_affectedPixels.Resize(m_header.width * m_header.height);
					if (m_requiresFrameHistory)
						m_previousFrame = std::make_unique_for_overwrite<UInt8[]>(m_header.width * m_header.height * 4);
					else
						m_previousFrame.reset();

					if (hasDisposeToPrevious)
						m_disposedRendering = std::make_unique_for_overwrite<UInt8[]>(m_header.width * m_header.height * 4);
					else
						m_disposedRendering.reset();

					m_currentFrame = 0;

					return Ok();
				}

				bool SetFile(const std::filesystem::path& filePath)
				{
					std::unique_ptr<File> file = std::make_unique<File>();
					if (!file->Open(filePath, OpenMode::Read))
					{
						NazaraError("failed to open stream from file: {0}", Error::GetLastError());
						return false;
					}
					m_ownedStream = std::move(file);

					SetStream(*m_ownedStream);
					return true;
				}

				void SetMemory(const void* data, std::size_t size)
				{
					m_ownedStream = std::make_unique<MemoryView>(data, size);
					SetStream(*m_ownedStream);
				}

				void SetStream(Stream& stream)
				{
					m_byteStream.SetStream(&stream);
				}

			private:
				struct ImageDecodingData;

				bool DecodeImageDescriptor(UInt8 minimumCodeSize, ImageDecodingData& decodingData)
				{
					Int32 clear = 1 << minimumCodeSize;
					UInt32 first = 1;
					Int32 codeSize = minimumCodeSize + 1;
					Int32 codeMask = (1 << codeSize) - 1;
					Int32 bits = 0;
					Int32 validBits = 0;
					m_lzwEntries.clear();
					m_lzwEntries.resize(8192); //< ??
					for (Int32 i = 0; i < clear; ++i)
					{
						auto& entry = m_lzwEntries[i];
						entry.prefix = -1;
						entry.first = UInt8(i);
						entry.suffix = UInt8(i);
					}

					// support no starting clear code
					Int32 avail = clear + 2;

					Int32 oldcode = -1;
					UInt8 len = 0;

					m_affectedPixels.Reset();

					for (;;)
					{
						if (validBits < codeSize)
						{
							if (len == 0)
							{
								m_byteStream >> len; // start new block
								if (len == 0)
									break;
							}

							UInt8 data;
							m_byteStream >> data;

							--len;
							bits |= data << validBits;
							validBits += 8;
						}
						else
						{
							Int32 code = bits & codeMask;
							bits >>= codeSize;
							validBits -= codeSize;
							// @OPTIMIZE: is there some way we can accelerate the non-clear path?
							if (code == clear)
							{
								// clear code
								codeSize = minimumCodeSize + 1;
								codeMask = (1 << codeSize) - 1;
								avail = clear + 2;
								oldcode = -1;
								first = 0;
							}
							else if (code == clear + 1)
							{
								// end of stream code
								SkipUntilTerminationBlock();
								break;
							}
							else if (code <= avail)
							{
								if (first)
								{
									NazaraError("corrupt gif (no clear code)");
									return false;
								}

								if (oldcode >= 0)
								{
									auto& p = m_lzwEntries[avail++];
									if (avail > 8192)
									{
										NazaraError("corrupt gif (too many codes)");
										return false;
									}

									p.prefix = SafeCast<Int16>(oldcode);
									p.first = m_lzwEntries[oldcode].first;
									p.suffix = (code == avail) ? p.first : m_lzwEntries[code].first;
								}
								else if (code == avail)
								{
									NazaraError("corrupt gif (illegal code in raster)");
									return false;
								}

								DecodeGIF(SafeCast<UInt16>(code), decodingData);

								if ((avail & codeMask) == 0 && avail <= 0x0FFF)
								{
									codeSize++;
									codeMask = (1 << codeSize) - 1;
								}

								oldcode = code;
							}
							else
							{
								NazaraError("corrupt gif (illegal code in raster)");
								return false;
							}
						}
					}

					return true;
				}

				void DecodeGIF(UInt16 code, ImageDecodingData& decodingData)
				{
					// recurse to decode the prefixes, since the linked-list is backwards,
					// and working backwards through an interleaved image would be nasty
					if (m_lzwEntries[code].prefix >= 0)
						DecodeGIF(m_lzwEntries[code].prefix, decodingData);

					if (decodingData.currentY >= decodingData.maxY)
						return;

					std::size_t idx = decodingData.currentX + decodingData.currentY;
					UInt8* p = &decodingData.outputImage[idx];
					m_affectedPixels[idx / 4] = true;

					std::size_t colorIndex = m_lzwEntries[code].suffix;

					const Color* c = &decodingData.colorTable[colorIndex];

					// don't render transparent pixels
					if (colorIndex != decodingData.transparentColorIndex)
					{
						p[0] = c->r;
						p[1] = c->g;
						p[2] = c->b;
						p[3] = c->a;
					}

					decodingData.currentX += 4;

					if (decodingData.currentX >= decodingData.maxX)
					{
						decodingData.currentX = decodingData.startX;
						decodingData.currentY += decodingData.step;

						while (decodingData.currentY >= decodingData.maxY && decodingData.parseMode > 0)
						{
							decodingData.step = (1ULL << decodingData.parseMode) * decodingData.lineSize;
							decodingData.currentY = decodingData.startY + (decodingData.step >> 1);
							--decodingData.parseMode;
						}
					}
				}

				void SkipUntilTerminationBlock()
				{
					for (;;)
					{
						UInt8 blockSize;
						m_byteStream >> blockSize;

						if (blockSize == 0)
							return;

						m_byteStream.Read(nullptr, blockSize);
					}
				}

				struct Color
				{
					UInt8 r, g, b, a;
				};

				struct FrameMetadata
				{
					std::size_t transparentIndex = std::numeric_limits<std::size_t>::max();
					Time time;
					UInt64 streamOffset;
					UInt8 disposalMethod = 0;
				};

				struct ImageDecodingData
				{
					std::size_t currentX;
					std::size_t currentY;
					std::size_t lineSize;
					std::size_t maxX;
					std::size_t maxY;
					std::size_t parseMode;
					std::size_t startX;
					std::size_t startY;
					std::size_t step;
					std::size_t transparentColorIndex;
					Color* colorTable;
					UInt8* outputImage;
				};

				struct LogicalScreenDescriptor
				{
					UInt16 height;
					UInt16 width;
					UInt8 backgroundPaletteIndex;
					UInt8 flags;
					UInt8 packedFields;
					UInt8 ratio;
				};

				struct LZWEntry
				{
					Int16 prefix = 0;
					UInt8 first = 0;
					UInt8 suffix = 0;
				};

				std::size_t m_currentFrame;
				std::vector<Color> m_globalColorTable;
				std::vector<Color> m_localColorTable;
				std::vector<FrameMetadata> m_frames;
				std::vector<LZWEntry> m_lzwEntries;
				std::unique_ptr<Stream> m_ownedStream;
				std::unique_ptr<UInt8[]> m_disposedRendering;
				std::unique_ptr<UInt8[]> m_previousFrame;
				Bitset<UInt64> m_affectedPixels;
				ByteStream m_byteStream;
				LogicalScreenDescriptor m_header;
				Time m_endFrameTime;
				bool m_requiresFrameHistory;
		};

		bool CheckGIFExtension(std::string_view extension)
		{
			return extension == ".gif";
		}

		Result<std::shared_ptr<ImageStream>, ResourceLoadingError> LoadGIFFile(const std::filesystem::path& filePath, const ImageStreamParams& /*parameters*/)
		{
			std::shared_ptr<GIFImageStream> gifStream = std::make_shared<GIFImageStream>();
			if (!gifStream->SetFile(filePath))
				return Err(ResourceLoadingError::FailedToOpenFile);

			Result status = gifStream->Open();
			return status.Map([&] { return std::move(gifStream); });
		}

		Result<std::shared_ptr<ImageStream>, ResourceLoadingError> LoadGIFMemory(const void* ptr, std::size_t size, const ImageStreamParams& /*parameters*/)
		{
			std::shared_ptr<GIFImageStream> gifStream = std::make_shared<GIFImageStream>();
			gifStream->SetMemory(ptr, size);

			Result status = gifStream->Open();
			return status.Map([&] { return std::move(gifStream); });
		}

		Result<std::shared_ptr<ImageStream>, ResourceLoadingError> LoadGIFStream(Stream& stream, const ImageStreamParams& /*parameters*/)
		{
			std::shared_ptr<GIFImageStream> gifStream = std::make_shared<GIFImageStream>();
			gifStream->SetStream(stream);

			Result status = gifStream->Open();
			return status.Map([&] { return std::move(gifStream); });
		}
	}

	namespace Loaders
	{
		ImageStreamLoader::Entry GetImageStreamLoader_GIF()
		{
			ImageStreamLoader::Entry loaderEntry;
			loaderEntry.extensionSupport = CheckGIFExtension;
			loaderEntry.fileLoader = LoadGIFFile;
			loaderEntry.memoryLoader = LoadGIFMemory;
			loaderEntry.streamLoader = LoadGIFStream;
			loaderEntry.parameterFilter = [](const ImageStreamParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinGIFLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loaderEntry;
		}
	}
}
