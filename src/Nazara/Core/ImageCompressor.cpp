// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ImageCompressor.hpp>
#include <Nazara/Core/Image.hpp>
#include <NazaraUtils/MathUtils.hpp>

#define STB_DXT_IMPLEMENTATION
#include <stb/stb_dxt.h>

namespace Nz::ImageCompressor
{
	namespace
	{
		template<PixelFormat SourceFormat>
		struct BlockExtractor;

		template<>
		struct BlockExtractor<PixelFormat::R8>
		{
			static constexpr std::size_t TargetBpp = 1;

			template<UInt32 BlockSize>
			static void Extract(const UInt8* input, std::size_t sourcePitch, std::span<UInt8, BlockSize * BlockSize * 1> output)
			{
				for (std::size_t y = 0; y < BlockSize; ++y)
					std::memcpy(&output[y * BlockSize * 1], &input[y * sourcePitch], BlockSize * 1);
			}
		};

		template<>
		struct BlockExtractor<PixelFormat::RG8>
		{
			static constexpr std::size_t TargetBpp = 2;

			template<UInt32 BlockSize>
			static void Extract(const UInt8* input, std::size_t sourcePitch, std::span<UInt8, BlockSize * BlockSize * 2> output)
			{
				for (std::size_t y = 0; y < BlockSize; ++y)
					std::memcpy(&output[y * BlockSize * 2], &input[y * sourcePitch], BlockSize * 2);
			}
		};

		template<>
		struct BlockExtractor<PixelFormat::RGB8>
		{
			static constexpr std::size_t TargetBpp = 4;

			template<UInt32 BlockSize>
			static void Extract(const UInt8* input, std::size_t sourcePitch, std::span<UInt8, BlockSize * BlockSize * 4> output)
			{
				UInt8* outputPtr = output.data();
				for (std::size_t y = 0; y < BlockSize; ++y)
				{
					for (std::size_t x = 0; x < BlockSize; ++x)
					{
						outputPtr[0] = input[0];
						outputPtr[1] = input[1];
						outputPtr[2] = input[2];
						outputPtr[3] = 0xFF;

						input += 4;
						outputPtr += 4;
					}

					input += sourcePitch;
					outputPtr += BlockSize;
				}
			}
		};

		template<>
		struct BlockExtractor<PixelFormat::RGBA8>
		{
			static constexpr std::size_t TargetBpp = 4;

			template<UInt32 BlockSize>
			static void Extract(const UInt8* input, std::size_t sourcePitch, std::span<UInt8, BlockSize * BlockSize * 4> output)
			{
				for (std::size_t y = 0; y < BlockSize; ++y)
					std::memcpy(&output[y * BlockSize * 4], &input[y * sourcePitch], BlockSize * 4);
			}
		};
	}

	void RGBA8BlockToBC1(std::span<const UInt8, 16 * 4> input, std::span<UInt8, 8> output)
	{
		stb_compress_dxt_block(output.data(), input.data(), 0, STB_DXT_HIGHQUAL);
	}

	void RGBA8BlockToBC3(std::span<const UInt8, 16 * 4> input, std::span<UInt8, 16> output)
	{
		stb_compress_dxt_block(output.data(), input.data(), 1, STB_DXT_HIGHQUAL);
	}

	void R8BlockToBC4(std::span<const UInt8, 16> input, std::span<UInt8, 8> output)
	{
		stb_compress_bc4_block(output.data(), input.data());
	}

	void RG8BlockToBC5(std::span<const UInt8, 16 * 2> input, std::span<UInt8, 16> output)
	{
		stb_compress_bc5_block(output.data(), input.data());
	}

	template<PixelFormat SourceFormat, PixelFormat DestFormat, UInt32 BlockSize, std::size_t CompressedBlockSize>
	Image CompressImage(const Image& sourceImage, auto BlockCompressor)
	{
		NazaraAssert(sourceImage.IsValid());
		NazaraAssert(sourceImage.GetFormat() == SourceFormat);

		UInt8 levelCount = std::min(sourceImage.GetLevelCount(), ImageUtils::GetMaxLevelCount(sourceImage.GetType(), DestFormat, sourceImage.GetWidth(), sourceImage.GetHeight(), sourceImage.GetDepth()));

		Image compressedImage(sourceImage.GetType(), DestFormat, AlignPow2(sourceImage.GetWidth(), BlockSize), AlignPow2(sourceImage.GetHeight(), BlockSize), sourceImage.GetDepth(), levelCount);
		for (UInt8 level = 0; level < levelCount; ++level)
		{
			if (!sourceImage.IsLevelAllocated(level))
				continue;

			UInt32 width = sourceImage.GetWidth(level);
			UInt32 height = sourceImage.GetHeight(level);
			UInt32 depth = sourceImage.GetDepth(level);
			if (sourceImage.GetType() == ImageType::Cubemap)
				depth *= 6;

			UInt32 blockCountX = width / BlockSize;
			UInt32 blockCountY = height / BlockSize;
			std::size_t bpp = PixelFormatInfo::GetBytesPerPixel(SourceFormat);
			std::size_t sourcePitch = sourceImage.GetWidth(level) * bpp;

			std::size_t bytesPerLayer = PixelFormatInfo::ComputeSize(DestFormat, compressedImage.GetWidth(level), compressedImage.GetHeight(level), 1u);

			for (UInt32 z = 0; z < depth; ++z)
			{
				const UInt8* sourcePixels = sourceImage.GetConstPixels(0, 0, z, level);
				UInt8* targetPixels = compressedImage.GetPixels(level) + bytesPerLayer * z;

				std::size_t blockIndex = 0;
				for (UInt32 blockY = 0; blockY < blockCountY; ++blockY)
				{
					const UInt8* lineStart = &sourcePixels[blockY * BlockSize * sourcePitch];
					for (UInt32 blockX = 0; blockX < blockCountX; ++blockX)
					{
						constexpr std::size_t TargetBpp = BlockExtractor<SourceFormat>::TargetBpp;

						std::array<UInt8, BlockSize * BlockSize * TargetBpp> blockData;
						BlockExtractor<SourceFormat>::template Extract<BlockSize>(&lineStart[blockX * BlockSize * bpp], sourcePitch, blockData);
						BlockCompressor(blockData, std::span<UInt8, CompressedBlockSize>(&targetPixels[blockIndex * CompressedBlockSize], CompressedBlockSize));
						blockIndex++;
					}

					// TODO: Handle overflow blocks
				}

				// TODO: Handle overflow blocks
			}
		}

		return compressedImage;
	}

	Image RGB8ToBC1(const Image& sourceImage)
	{
		return CompressImage<PixelFormat::RGB8, PixelFormat::BC1_RGB_Unorm, 4, 8>(sourceImage, RGBA8BlockToBC1);
	}

	Image RGBA8ToBC1(const Image& sourceImage)
	{
		return CompressImage<PixelFormat::RGBA8, PixelFormat::BC1_RGBA_Unorm, 4, 8>(sourceImage, RGBA8BlockToBC1);
	}

	Image RGBA8ToBC3(const Image& sourceImage)
	{
		return CompressImage<PixelFormat::RGBA8, PixelFormat::BC3_Unorm, 4, 16>(sourceImage, RGBA8BlockToBC3);
	}

	Image R8ToBC4(const Image& sourceImage)
	{
		return CompressImage<PixelFormat::R8, PixelFormat::BC4_Unorm, 4, 8>(sourceImage, R8BlockToBC4);
	}

	Image RG8ToBC5(const Image& sourceImage)
	{
		return CompressImage<PixelFormat::RG8, PixelFormat::BC5_Unorm, 4, 16>(sourceImage, RG8BlockToBC5);
	}
}
