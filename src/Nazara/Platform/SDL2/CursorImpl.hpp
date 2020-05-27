// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <array>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Image.hpp>

#include <SDL2/SDL_mouse.h>

namespace Nz
{
	class Image;

	class CursorImpl
	{
		friend class Cursor;

		public:
			bool Create(const Image& image, int hotSpotX, int hotSpotY);
			bool Create(SystemCursor cursor);

			void Destroy();

			SDL_Cursor* GetCursor();

		private:
			static bool Initialize();
			static void Uninitialize();

			SDL_Cursor* m_cursor = nullptr;
			SDL_Surface* m_icon = nullptr;
			Image m_iconImage;

			static std::array<SDL_SystemCursor, SystemCursor_Max + 1> s_systemCursorIds;
	};
}

#endif // NAZARA_CURSORIMPL_HPP
