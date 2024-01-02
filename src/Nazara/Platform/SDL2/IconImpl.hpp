// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL2_ICONIMPL_HPP
#define NAZARA_PLATFORM_SDL2_ICONIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Utility/Image.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <SDL_surface.h>

namespace Nz
{
	class IconImpl
	{
		public:
			IconImpl(const Image& image);
			IconImpl(const IconImpl&) = delete;
			IconImpl(IconImpl&&) noexcept = default;
			~IconImpl();

			SDL_Surface* GetIcon();

			IconImpl& operator=(const IconImpl&) = default;
			IconImpl& operator=(IconImpl&&) noexcept = default;

		private:
			Image m_iconImage;
			MovablePtr<SDL_Surface> m_icon;
	};
}

#endif // NAZARA_PLATFORM_SDL2_ICONIMPL_HPP
