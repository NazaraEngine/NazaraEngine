// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
}
