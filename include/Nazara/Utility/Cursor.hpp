// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSOR_HPP
#define NAZARA_CURSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Image.hpp>

namespace Nz
{
	class CursorImpl;

	class NAZARA_UTILITY_API Cursor
	{
		friend class WindowImpl;

		public:
			inline Cursor();
			inline ~Cursor();

			bool Create(const Image& cursor, int hotSpotX = 0, int hotSpotY = 0);
			bool Create(const Image& cursor, const Vector2i& hotSpot);
			void Destroy();

			inline const Image& GetImage() const;
			inline bool IsValid() const;

		private:
			Image m_cursorImage;
			CursorImpl* m_impl;
	};
}

#include <Nazara/Utility/Cursor.inl>

#endif // NAZARA_CURSOR_HPP
