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
			inline Cursor(SystemCursor systemCursor); //< implicit conversion intended
			Cursor(const Cursor&) = delete;
			inline Cursor(Cursor&& cursor) noexcept;
			inline ~Cursor();

			bool Create(const Image& cursor, const Vector2i& hotSpot);
			bool Create(SystemCursor cursor);

			void Destroy();

			inline const Image& GetImage() const;
			inline SystemCursor GetSystemCursor() const;

			inline bool IsValid() const;

			Cursor& operator=(const Cursor&) = delete;
			inline Cursor& operator=(Cursor&& cursor);

		private:
			static bool Initialize();
			static void Uninitialize();

			Image m_cursorImage;
			SystemCursor m_systemCursor;
			CursorImpl* m_impl;
			bool m_usesSystemCursor;
	};
}

#include <Nazara/Utility/Cursor.inl>

#endif // NAZARA_CURSOR_HPP
