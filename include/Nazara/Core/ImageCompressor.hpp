// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_IMAGECOMPRESSOR_HPP
#define NAZARA_CORE_IMAGECOMPRESSOR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <memory>
#include <span>

namespace Nz
{
	class Image;
}

namespace Nz::ImageCompressor
{
	enum class BCFormat
	{
		BC1,
		BC3,
		BC4,
		BC5,

		Max = BC5
	};

	// Blocks to BC
	NAZARA_CORE_API void RGBA8BlockToBC1(std::span<const UInt8, 16 * 4> input, std::span<UInt8, 8> output);
	NAZARA_CORE_API void RGBA8BlockToBC3(std::span<const UInt8, 16 * 4> input, std::span<UInt8, 16> output);
	NAZARA_CORE_API void R8BlockToBC4(std::span<const UInt8, 16 * 1> input, std::span<UInt8, 8> output);
	NAZARA_CORE_API void RG8BlockToBC5(std::span<const UInt8, 16 * 2> input, std::span<UInt8, 16> output);

	// Image to BC
	NAZARA_CORE_API Image RGB8ToBC1(const Image& sourceImage);
	NAZARA_CORE_API Image RGBA8ToBC1(const Image& sourceImage);
	NAZARA_CORE_API Image RGBA8ToBC3(const Image& sourceImage);
	NAZARA_CORE_API Image R8ToBC4(const Image& sourceImage);
	NAZARA_CORE_API Image RG8ToBC5(const Image& sourceImage);

	// BC Crunch
	class NAZARA_CORE_API BCCruncher
	{
		public:
			BCCruncher();
			BCCruncher(const BCCruncher&) = delete;
			BCCruncher(BCCruncher&&) noexcept = default;
			~BCCruncher() = default;

			std::size_t Crunch(const void* input, UInt32 width, UInt32 height, BCFormat format, void* output, std::size_t length);

			BCCruncher& operator=(const BCCruncher&) = delete;
			BCCruncher& operator=(BCCruncher&&) noexcept = default;

			static void Decrunch(const void* input, std::size_t length, UInt32 width, UInt32 height, BCFormat format, void* output);

		private:
			std::unique_ptr<UInt8[]> m_memory;
	};
}

#include <Nazara/Core/ImageCompressor.inl>

#endif // NAZARA_CORE_IMAGECOMPRESSOR_HPP
