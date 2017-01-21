// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>

namespace Nz
{
	struct WindowEvent
	{
		// Utilisé par:
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

		// Utilisé par:
		// -WindowEventType_MouseButtonDoubleClicked
		// -WindowEventType_MouseButtonPressed
		struct MouseButtonEvent
		{
			Mouse::Button button;
			unsigned int x;
			unsigned int y;
		};

		// Utilisé par:
		// -WindowEventType_MouseMoved
		struct MouseMoveEvent
		{
			int deltaX;
			int deltaY;
			unsigned int x;
			unsigned int y;
		};

		// Utilisé par:
		// -WindowEventType_MouseWheelMoved
		struct MouseWheelEvent
		{
			float delta;
		};

		// Utilisé par:
		// -WindowEventType_Moved
		struct PositionEvent
		{
			int x;
			int y;
		};

		// Utilisé par:
		// -WindowEventType_Resized
		struct SizeEvent
		{
			unsigned int height;
			unsigned int width;
		};

		// Utilisé par:
		// -WindowEventType_TextEntered
		struct TextEvent
		{
			bool repeated;
			char32_t character;
		};

		WindowEventType type;

		union
		{
			// Utilisé par:
			// -WindowEventType_KeyPressed
			// -WindowEventType_KeyReleased
			KeyEvent key;

			// Utilisé par:
			// -WindowEventType_MouseButtonDoubleClicked
			// -WindowEventType_MouseButtonPressed
			MouseButtonEvent mouseButton;

			// Utilisé par:
			// -WindowEventType_MouseMoved
			MouseMoveEvent mouseMove;

			// Utilisé par:
			// -WindowEventType_MouseWheelMoved
			MouseWheelEvent mouseWheel;

			// Utilisé par:
			// -WindowEventType_Moved
			PositionEvent position;

			// Utilisé par:
			// -WindowEventType_Resized
			SizeEvent size;

			// Utilisé par:
			// -WindowEventType_TextEntered
			TextEvent text;
		};
	};
}

#endif // NAZARA_EVENT_HPP
