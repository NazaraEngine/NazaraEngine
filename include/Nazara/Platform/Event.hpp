// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspired by the SFML of Laurent Gomila (and its team)

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <array>

#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>

namespace Nz
{
	struct WindowEvent
	{
		// Used by:
		// -WindowEventType::KeyPressed
		// -WindowEventType::KeyReleased
		struct KeyEvent
		{
			Keyboard::Scancode scancode;
			Keyboard::VKey virtualKey;
			bool alt;
			bool control;
			bool repeated;
			bool shift;
			bool system;
		};

		// Used by:
		// -WindowEventType::MouseButtonDoubleClicked
		// -WindowEventType::MouseButtonPressed
		struct MouseButtonEvent
		{
			Mouse::Button button;
			int x;
			int y;
		};

		// Used by:
		// -WindowEventType::MouseMoved
		struct MouseMoveEvent
		{
			int deltaX;
			int deltaY;
			int x;
			int y;
		};

		// Used by:
		// -WindowEventType::MouseWheelMoved
		struct MouseWheelEvent
		{
			float delta;
			int x;
			int y;
		};

		// Used by:
		// -WindowEventType::Moved
		struct PositionEvent
		{
			int x;
			int y;
		};

		// Used by:
		// -WindowEventType::Resized
		struct SizeEvent
		{
			unsigned int height;
			unsigned int width;
		};

		// Used by:
		// -WindowEventType::TextEntered
		struct TextEvent
		{
			bool repeated;
			char32_t character;
		};

		// Used by:
		// -WindowEventType::TextEdited
		struct EditEvent
		{
			int length;
			std::array<char, 32> text;
		};

		WindowEventType type;

		union
		{
			// Used by:
			// -WindowEventType::KeyPressed
			// -WindowEventType::KeyReleased
			KeyEvent key;

			// Used by:
			// -WindowEventType::MouseButtonDoubleClicked
			// -WindowEventType::MouseButtonPressed
			MouseButtonEvent mouseButton;

			// Used by:
			// -WindowEventType::MouseMoved
			MouseMoveEvent mouseMove;

			// Used by:
			// -WindowEventType::MouseWheelMoved
			MouseWheelEvent mouseWheel;

			// Used by:
			// -WindowEventType::Moved
			PositionEvent position;

			// Used by:
			// -WindowEventType::Resized
			SizeEvent size;

			// Used by:
			// -WindowEventType::TextEntered
			TextEvent text;

			// Used by:
			// -WindowEventType::TextEntered
			EditEvent edit;
		};
	};
}

#endif // NAZARA_EVENT_HPP
