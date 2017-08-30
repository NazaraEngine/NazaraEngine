// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspired by the SFML of Laurent Gomila (and its team)

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>

namespace Nz
{
	struct WindowEvent
	{
		// Used by:
		// -WindowEventType_KeyPressed
		// -WindowEventType_KeyReleased
		struct KeyEvent
		{
			Keyboard::Key code;
			bool alt;
			bool control;
			bool repeated;
			bool shift;
			bool system;
		};

		// Used by:
		// -WindowEventType_MouseButtonDoubleClicked
		// -WindowEventType_MouseButtonPressed
		struct MouseButtonEvent
		{
			Mouse::Button button;
			unsigned int x;
			unsigned int y;
		};

		// Used by:
		// -WindowEventType_MouseMoved
		struct MouseMoveEvent
		{
			int deltaX;
			int deltaY;
			unsigned int x;
			unsigned int y;
		};

		// Used by:
		// -WindowEventType_MouseWheelMoved
		struct MouseWheelEvent
		{
			float delta;
		};

		// Used by:
		// -WindowEventType_Moved
		struct PositionEvent
		{
			int x;
			int y;
		};

		// Used by:
		// -WindowEventType_Resized
		struct SizeEvent
		{
			unsigned int height;
			unsigned int width;
		};

		// Used by:
		// -WindowEventType_TextEntered
		struct TextEvent
		{
			bool repeated;
			char32_t character;
		};

		WindowEventType type;

		union
		{
			// Used by:
			// -WindowEventType_KeyPressed
			// -WindowEventType_KeyReleased
			KeyEvent key;

			// Used by:
			// -WindowEventType_MouseButtonDoubleClicked
			// -WindowEventType_MouseButtonPressed
			MouseButtonEvent mouseButton;

			// Used by:
			// -WindowEventType_MouseMoved
			MouseMoveEvent mouseMove;

			// Used by:
			// -WindowEventType_MouseWheelMoved
			MouseWheelEvent mouseWheel;

			// Used by:
			// -WindowEventType_Moved
			PositionEvent position;

			// Used by:
			// -WindowEventType_Resized
			SizeEvent size;

			// Used by:
			// -WindowEventType_TextEntered
			TextEvent text;
		};
	};
}

#endif // NAZARA_EVENT_HPP
