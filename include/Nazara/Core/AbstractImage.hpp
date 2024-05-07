// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ABSTRACTIMAGE_HPP
#define NAZARA_CORE_ABSTRACTIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
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
			virtual Vector3ui32 GetSize(UInt8 level = 0) const = 0;
			virtual ImageType GetType() const = 0;

			bool IsCompressed() const;
			bool IsCubemap() const;

			inline bool Update(const void* pixels, UInt32 srcWidth = 0, UInt32 srcHeight = 0, UInt8 level = 0);
			virtual bool Update(const void* pixels, const Boxui& box, UInt32 srcWidth = 0, UInt32 srcHeight = 0, UInt8 level = 0) = 0;
			inline bool Update(const void* pixels, const Rectui& rect, UInt32 z = 0, UInt32 srcWidth = 0, UInt32 srcHeight = 0, UInt8 level = 0);

			AbstractImage& operator=(const AbstractImage&) = default;
			AbstractImage& operator=(AbstractImage&&) noexcept = default;
	};
}

#include <Nazara/Core/AbstractImage.inl>

#endif // NAZARA_CORE_ABSTRACTIMAGE_HPP
