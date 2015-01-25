// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTIMAGE_HPP
#define NAZARA_ABSTRACTIMAGE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>

class NAZARA_API NzAbstractImage
{
	public:
		NzAbstractImage() = default;
		virtual ~NzAbstractImage();

		nzUInt8 GetBytesPerPixel() const;
		virtual unsigned int GetDepth(nzUInt8 level = 0) const = 0;
		virtual nzPixelFormat GetFormat() const = 0;
		virtual unsigned int GetHeight(nzUInt8 level = 0) const = 0;
		virtual nzUInt8 GetLevelCount() const = 0;
		virtual nzUInt8 GetMaxLevel() const = 0;
		virtual unsigned int GetMemoryUsage() const = 0;
		virtual unsigned int GetMemoryUsage(nzUInt8 level) const = 0;
		virtual NzVector3ui GetSize(nzUInt8 level = 0) const = 0;
		virtual nzImageType GetType() const = 0;
		virtual unsigned int GetWidth(nzUInt8 level = 0) const = 0;

		bool IsCompressed() const;
		bool IsCubemap() const;

		virtual bool Update(const nzUInt8* pixels, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0) = 0;
		virtual bool Update(const nzUInt8* pixels, const NzBoxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0) = 0;
		virtual bool Update(const nzUInt8* pixels, const NzRectui& rect, unsigned int z = 0, unsigned int srcWidth = 0, unsigned int srcHeight = 0, nzUInt8 level = 0) = 0;
};

#endif // NAZARA_IMAGE_HPP
