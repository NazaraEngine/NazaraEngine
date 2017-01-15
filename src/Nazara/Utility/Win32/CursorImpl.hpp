// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSORIMPL_HPP
#define NAZARA_CURSORIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <array>
#include <windows.h>

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

			HCURSOR GetCursor();

		private:
			static bool Initialize();
			static void Uninitialize();

			HCURSOR m_cursor = nullptr;
			HICON m_icon = nullptr;

			static std::array<LPTSTR, SystemCursor_Max + 1> s_systemCursorIds;
	};
}

#endif // NAZARA_CURSORIMPL_HPP
