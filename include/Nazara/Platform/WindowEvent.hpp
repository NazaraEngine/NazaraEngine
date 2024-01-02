// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspired by the SFML of Laurent Gomila (and its team)

#pragma once

#ifndef NAZARA_PLATFORM_WINDOWEVENT_HPP
#define NAZARA_PLATFORM_WINDOWEVENT_HPP

#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <array>

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
		// -WindowEventType::MouseButtonPressed
		// -WindowEventType::MouseButtonReleased
		struct MouseButtonEvent
		{
			Mouse::Button button;
			int x;
			int y;
			unsigned int clickCount; //< 1 for simple click, 2 for double click, 3 for triple click. (always 1 on release event)
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
			// -WindowEventType::MouseButtonPressed
			// -WindowEventType::MouseButtonReleased
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

#endif // NAZARA_PLATFORM_WINDOWEVENT_HPP
