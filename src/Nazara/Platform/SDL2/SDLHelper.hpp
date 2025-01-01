// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL2_SDLHELPER_HPP
#define NAZARA_PLATFORM_SDL2_SDLHELPER_HPP

#include <Nazara/Platform/Keyboard.hpp>
#include <SDL_keycode.h>
#include <SDL_scancode.h>

namespace Nz
{
	class SDLHelper
	{
		public:
			static Keyboard::Scancode FromSDL(SDL_Scancode scancode);
			static Keyboard::VKey     FromSDL(SDL_Keycode keycode);
			static SDL_Scancode       ToSDL(Keyboard::Scancode scancode);
			static SDL_Keycode        ToSDL(Keyboard::VKey keycode);
	};
}

#endif // NAZARA_PLATFORM_SDL2_SDLHELPER_HPP
