// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL3_INPUTIMPL_HPP
#define NAZARA_PLATFORM_SDL3_INPUTIMPL_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <string>

namespace Nz
{
	class InputImpl
	{
		public:
			static ClipboardContentType GetClipboardContentType();
			static std::string GetClipboardString();
			static std::string_view GetKeyName(Keyboard::Scancode scancode);
			static std::string_view GetKeyName(Keyboard::VKey key);
			static Vector2f GetMousePosition();
			static Vector2f GetMousePosition(const Window& relativeTo);
			static bool IsKeyPressed(Keyboard::Scancode key);
			static bool IsKeyPressed(Keyboard::VKey key);
			static bool IsMouseButtonPressed(Mouse::Button button);
			static void SetClipboardString(const std::string& str);
			static void SetMousePosition(float x, float y);
			static void SetMousePosition(float x, float y, const Window& relativeTo);
			static Keyboard::Scancode ToScanCode(Keyboard::VKey key);
			static Keyboard::VKey ToVirtualKey(Keyboard::Scancode scancode);
	};
}

#endif // NAZARA_PLATFORM_SDL3_INPUTIMPL_HPP
