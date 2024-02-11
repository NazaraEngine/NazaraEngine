// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ABSTRACTIMAGE_HPP
#define NAZARA_CORE_ABSTRACTIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class AbstractImage;

	class NAZARA_CORE_API AbstractImage
	{
		public:
			AbstractImage() = default;
			AbstractImage(const AbstractImage&) = default;
			AbstractImage(AbstractImage&&) noexcept = default;
			virtual ~AbstractImage();

			UInt8 GetBytesPerPixel() const;
			virtual PixelFormat GetFormat() const = 0;
			virtual UInt8 GetLevelCount() const = 0;
			virtual Vector3ui GetSize(UInt8 level = 0) const = 0;
			virtual ImageType GetType() const = 0;

			bool IsCompressed() const;
			bool IsCubemap() const;

			inline bool Update(const void* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0);
			virtual bool Update(const void* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) = 0;
			inline bool Update(const void* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0);

			AbstractImage& operator=(const AbstractImage&) = default;
			AbstractImage& operator=(AbstractImage&&) noexcept = default;
	};
}

#include <Nazara/Core/AbstractImage.inl>

#endif // NAZARA_CORE_ABSTRACTIMAGE_HPP
