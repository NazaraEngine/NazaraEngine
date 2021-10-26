// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SDL2_HELPER_HPP
#define NAZARA_SDL2_HELPER_HPP

#include <Nazara/Platform/Keyboard.hpp>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

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

#endif // NAZARA_SDL2_HELPER_HPP
