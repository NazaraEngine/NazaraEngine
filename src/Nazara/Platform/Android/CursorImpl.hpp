// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_ANDROID_CURSORIMPL_HPP
#define NAZARA_PLATFORM_ANDROID_CURSORIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utils/MovablePtr.hpp>

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
