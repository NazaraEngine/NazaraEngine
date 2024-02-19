// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline Image::Image(Image&& image) noexcept :
	m_sharedImage(std::exchange(image.m_sharedImage, &emptyImage))
	{
	}

	inline Image& Image::operator=(Image&& image) noexcept
	{
		std::swap(m_sharedImage, image.m_sharedImage);

		return *this;
	}

	inline void Image::ArrayToRegion(ImageType type, unsigned int baseLayer, unsigned int layerCount, Boxui& region)
	{
		switch (type)
		{
			case ImageType::E1D_Array:
				region.y = baseLayer;
				region.height = layerCount;
				break;

			case ImageType::Cubemap:
			case ImageType::E2D_Array:
				region.z = baseLayer;
				region.depth = layerCount;
				break;

			case ImageType::E1D:
				NazaraAssert(baseLayer == 0, "out of bounds");
				NazaraAssert(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E2D:
				NazaraAssert(baseLayer == 0, "out of bounds");
				NazaraAssert(layerCount <= 1, "out of bounds");
				[[fallthrough]];
			case ImageType::E3D:
				region.z = 0;
				region.depth = 1;
				break;
		}
	}

	inline Boxui Image::RegionToArray(ImageType type, Boxui region, unsigned int& baseLayer, unsigned int& layerCount)
	{
		switch (type)
		{
			case ImageType::E1D_Array:
				baseLayer = region.y;
				layerCount = region.height;

				region.y = 0;
				region.height = 1;
				break;

			case ImageType::Cubemap:
			case ImageType::E2D_Array:
				baseLayer = region.z;
				layerCount = region.depth;

				region.z = 0;
				region.depth = 1;
				break;

			case ImageType::E1D:
				NazaraAssert(region.y == 0, "out of bounds");
				NazaraAssert(region.height <= 1, "out of bounds");
			case ImageType::E2D:
				NazaraAssert(region.z == 0, "out of bounds");
				NazaraAssert(region.depth <= 1, "out of bounds");
			case ImageType::E3D:
				baseLayer = 0;
				layerCount = 1;
				break;
		}

		return region;
	}
}

