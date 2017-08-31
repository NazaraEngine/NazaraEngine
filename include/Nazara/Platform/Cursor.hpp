// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CURSOR_HPP
#define NAZARA_CURSOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Utility/Image.hpp>
#include <array>

namespace Nz
{
	class CursorImpl;

	class Cursor;

	using CursorConstRef = ObjectRef<const Cursor>;
	using CursorRef = ObjectRef<Cursor>;

	class NAZARA_PLATFORM_API Cursor : public RefCounted
	{
		friend class Platform;
		friend class WindowImpl;

		public:
			inline Cursor();
			inline Cursor(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder);
			Cursor(const Cursor&) = delete;
			Cursor(Cursor&&) = delete;
			inline ~Cursor();

			bool Create(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder);

			void Destroy();

			inline const Image& GetImage() const;
			inline SystemCursor GetSystemCursor() const;

			inline bool IsValid() const;

			Cursor& operator=(const Cursor&) = delete;
			Cursor& operator=(Cursor&&) = delete;

			static inline Cursor* Get(SystemCursor cursor);
			template<typename... Args> static CursorRef New(Args&&... args);

		private:
			inline explicit Cursor(SystemCursor systemCursor);

			bool Create(SystemCursor cursor);

			static bool Initialize();
			static void Uninitialize();

			Image m_cursorImage;
			SystemCursor m_systemCursor;
			CursorImpl* m_impl;

			static std::array<Cursor, SystemCursor_Max + 1> s_systemCursors;
	};
}

#include <Nazara/Platform/Cursor.inl>

#endif // NAZARA_CURSOR_HPP
