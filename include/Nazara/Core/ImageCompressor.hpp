// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_IMAGECOMPRESSOR_HPP
#define NAZARA_CORE_IMAGECOMPRESSOR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <span>

namespace Nz
{
	class Image;
}

namespace Nz::ImageCompressor
{
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
}

#include <Nazara/Core/ImageCompressor.inl>

#endif // NAZARA_CORE_IMAGECOMPRESSOR_HPP
