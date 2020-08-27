// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTIMPL_HPP
#define NAZARA_INPUTIMPL_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>

namespace Nz
{
	class EventImpl
	{
		public:
			static String GetKeyName(Keyboard::Scancode scancode);
			static String GetKeyName(Keyboard::VKey key);
			static Vector2i GetMousePosition();
			static Vector2i GetMousePosition(const Window& relativeTo);
			static bool IsKeyPressed(Keyboard::Scancode key);
			static bool IsKeyPressed(Keyboard::VKey key);
			static bool IsMouseButtonPressed(Mouse::Button button);
			static bool SetRelativeMouseMode(bool relativeMouseMode);
			static void SetMousePosition(int x, int y);
			static void SetMousePosition(int x, int y, const Window& relativeTo);
			static void StartTextInput();
			static void StopTextInput();
			static Keyboard::Scancode ToScanCode(Keyboard::VKey key);
			static Keyboard::VKey ToVirtualKey(Keyboard::Scancode scancode);
	};
}

#endif // NAZARA_INPUTIMPL_HPP
