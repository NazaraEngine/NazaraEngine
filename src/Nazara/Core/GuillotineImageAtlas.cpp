// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/GuillotineImageAtlas.hpp>

namespace Nz
{
	DataStoreFlags GuillotineImageAtlas::GetStorage() const
	{
		return DataStorage::Software;
	}

	std::shared_ptr<AbstractImage> GuillotineImageAtlas::ResizeImage(const AbstractImage* oldImage, const Vector2ui& size) const
	{
		std::shared_ptr<Image> newImage = std::make_shared<Image>(ImageType::E2D, GetPixelFormat(), size.x, size.y);
		if (oldImage)
		{
			const Image& srcImage = SafeCast<const Image&>(*oldImage);
			if (srcImage.IsLevelAllocated(0))
				newImage->Copy(srcImage, Boxui(Vector3ui::Zero(), oldImage->GetSize()), Vector2ui(0, 0));
		}

		return newImage;
	}

	void GuillotineImageAtlas::UpdateImage(AbstractImage& image, const void* ptr, const Rectui& rect, UInt32 srcWidth, UInt32 srcHeight) const
	{
		Image& dstImage = SafeCast<Image&>(image);
		dstImage.Update(ptr, rect, 0, srcWidth, srcHeight);
	}
}
