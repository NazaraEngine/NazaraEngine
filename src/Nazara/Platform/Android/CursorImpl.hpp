// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_ANDROID_CURSORIMPL_HPP
#define NAZARA_PLATFORM_ANDROID_CURSORIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	class CursorImpl
	{
		public:
			CursorImpl(const Image& cursor, const Vector2i& hotSpot);
			CursorImpl(SystemCursor cursor);
			CursorImpl(const CursorImpl&) = delete;
			CursorImpl(CursorImpl&&) noexcept = default;
			~CursorImpl() = default;

			CursorImpl& operator=(const CursorImpl&) = delete;
			CursorImpl& operator=(CursorImpl&&) noexcept = default;
	};
}

#endif // NAZARA_PLATFORM_ANDROID_CURSORIMPL_HPP
