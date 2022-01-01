// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Image.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

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

#include <Nazara/Utility/DebugOff.hpp>
