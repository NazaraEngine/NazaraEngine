// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL3_SDLHELPER_HPP
#define NAZARA_PLATFORM_SDL3_SDLHELPER_HPP

#include <Nazara/Platform/Keyboard.hpp>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_scancode.h>

namespace Nz
{
	Keyboard::Scancode FromSDL(SDL_Scancode scancode);
	Keyboard::VKey FromSDL(SDL_Keycode keycode);

	SDL_Scancode ToSDL(Keyboard::Scancode scancode);
	SDL_Keycode ToSDL(Keyboard::VKey keycode);
}

#endif // NAZARA_PLATFORM_SDL3_SDLHELPER_HPP
