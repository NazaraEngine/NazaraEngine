// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL3_ICONIMPL_HPP
#define NAZARA_PLATFORM_SDL3_ICONIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Image.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <SDL3/SDL_surface.h>

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

#endif // NAZARA_PLATFORM_SDL3_ICONIMPL_HPP
