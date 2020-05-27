// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Debug.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

namespace Nz
{
	bool IconImpl::Create(const Image& icon)
	{
		m_iconImage = icon;
		if (!m_iconImage.Convert(PixelFormat_BGRA8))
		{
			NazaraError("Failed to convert icon to BGRA8");
			return false;
		}

		m_icon = SDL_CreateRGBSurfaceWithFormatFrom(
			m_iconImage.GetPixels(),
			m_iconImage.GetWidth(),
			m_iconImage.GetHeight(),
			32,
			32 * m_iconImage.GetWidth(),
			SDL_PIXELFORMAT_BGRA8888
			);

		if (!m_icon)
		{
			NazaraError(SDL_GetError());
			return false;
		}

		return true;
	}

	void IconImpl::Destroy()
	{
		SDL_FreeSurface(m_icon);
		m_iconImage.Destroy();
	}

	SDL_Surface* IconImpl::GetIcon()
	{
		return m_icon;
	}
}
