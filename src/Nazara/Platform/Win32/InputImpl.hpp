// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTIMPL_HPP
#define NAZARA_INPUTIMPL_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>

namespace Nz
{
	class EventImpl
	{
		public:
			static String GetKeyName(Keyboard::Key key);
			static Vector2i GetMousePosition();
			static Vector2i GetMousePosition(const Window& relativeTo);
			static bool IsKeyPressed(Keyboard::Key key);
			static bool IsMouseButtonPressed(Mouse::Button button);
			static void SetMousePosition(int x, int y);
			static void SetMousePosition(int x, int y, const Window& relativeTo);
	};
}

#endif // NAZARA_INPUTIMPL_HPP
