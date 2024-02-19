// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/PixelFormat.hpp>

namespace Nz
{
	IconImpl::IconImpl(const Image& icon)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException);

		m_iconImage = icon;
		if (!m_iconImage.Convert(PixelFormat::BGRA8))
			NazaraError("failed to convert icon to BGRA8");

		m_icon = SDL_CreateRGBSurfaceWithFormatFrom(
			m_iconImage.GetPixels(),
			m_iconImage.GetWidth(),
			m_iconImage.GetHeight(),
			32,
			32 * m_iconImage.GetWidth(),
			SDL_PIXELFORMAT_BGRA8888
		);

		if (!m_icon)
			NazaraErrorFmt("failed to create SDL Surface for icon: {0}", SDL_GetError());
	}

	IconImpl::~IconImpl()
	{
		if (m_icon)
			SDL_FreeSurface(m_icon);
	}

	SDL_Surface* IconImpl::GetIcon()
	{
		return m_icon;
	}
}
