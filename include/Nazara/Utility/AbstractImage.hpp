// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTIMAGE_HPP
#define NAZARA_ABSTRACTIMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class AbstractImage;

	using AbstractImageConstRef = ObjectRef<const AbstractImage>;
	using AbstractImageRef = ObjectRef<AbstractImage>;

	class NAZARA_UTILITY_API AbstractImage : public RefCounted
	{
		public:
			AbstractImage() = default;
			inline AbstractImage(const AbstractImage& image);
			virtual ~AbstractImage();

			UInt8 GetBytesPerPixel() const;
			virtual unsigned int GetDepth(UInt8 level = 0) const = 0;
			virtual PixelFormatType GetFormat() const = 0;
			virtual unsigned int GetHeight(UInt8 level = 0) const = 0;
			virtual UInt8 GetLevelCount() const = 0;
			virtual UInt8 GetMaxLevel() const = 0;
			virtual std::size_t GetMemoryUsage() const = 0;
			virtual std::size_t GetMemoryUsage(UInt8 level) const = 0;
			virtual Vector3ui GetSize(UInt8 level = 0) const = 0;
			virtual ImageType GetType() const = 0;
			virtual unsigned int GetWidth(UInt8 level = 0) const = 0;

			bool IsCompressed() const;
			bool IsCubemap() const;

			virtual bool Update(const UInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) = 0;
			virtual bool Update(const UInt8* pixels, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) = 0;
			virtual bool Update(const UInt8* pixels, const Rectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) = 0;
	};
}

#include <Nazara/Utility/AbstractImage.inl>

#endif // NAZARA_IMAGE_HPP
