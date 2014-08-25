// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>

struct NzEvent
{
	// Utilisé par:
	// -nzEventType_KeyPressed
	// -nzEventType_KeyReleased
	struct KeyEvent
	{
		NzKeyboard::Key code;
		bool alt;
		bool control;
		bool repeated;
		bool shift;
		bool system;
	};

	// Utilisé par:
	// -nzEventType_MouseButtonDoubleClicked
	// -nzEventType_MouseButtonPressed
	struct MouseButtonEvent
	{
		NzMouse::Button button;
		unsigned int x;
		unsigned int y;
	};

	// Utilisé par:
	// -nzEventType_MouseMoved
	struct MouseMoveEvent
	{
		int deltaX;
		int deltaY;
		unsigned int x;
		unsigned int y;
	};

	// Utilisé par:
	// -nzEventType_MouseWheelMoved
	struct MouseWheelEvent
	{
		float delta;
	};

	// Utilisé par:
	// -nzEventType_Moved
	struct PositionEvent
	{
		int x;
		int y;
	};

	// Utilisé par:
	// -nzEventType_Resized
	struct SizeEvent
	{
		unsigned int height;
		unsigned int width;
	};

	// Utilisé par:
	// -nzEventType_TextEntered
	struct TextEvent
	{
		bool repeated;
		char32_t character;
	};

	nzEventType type;

	union
	{
		// Utilisé par:
		// -nzEventType_KeyPressed
		// -nzEventType_KeyReleased
		KeyEvent key;

		// Utilisé par:
		// -nzEventType_MouseButtonDoubleClicked
		// -nzEventType_MouseButtonPressed
		MouseButtonEvent mouseButton;

		// Utilisé par:
		// -nzEventType_MouseMoved
		MouseMoveEvent mouseMove;

		// Utilisé par:
		// -nzEventType_MouseWheelMoved
		MouseWheelEvent mouseWheel;

		// Utilisé par:
		// -nzEventType_Moved
		PositionEvent position;

		// Utilisé par:
		// -nzEventType_Resized
		SizeEvent size;

		// Utilisé par:
		// -nzEventType_TextEntered
		TextEvent text;
	};
};

#endif // NAZARA_EVENT_HPP
