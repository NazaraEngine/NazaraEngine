// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>

struct NzEvent
{
	struct KeyEvent
	{
		NzKeyboard::Key code;
		bool alt;
		bool control;
		bool shift;
		bool system;
	};

	struct MouseButtonEvent
	{
		NzMouse::Button button;
		unsigned int x;
		unsigned int y;
	};

	struct MouseMoveEvent
	{
		int x;
		int y;
	};

	struct MouseWheelEvent
	{
		float delta;
	};

	struct PositionEvent
	{
		int x;
		int y;
	};

	struct SizeEvent
	{
		unsigned int height;
		unsigned int width;
	};

	struct TextEvent
	{
		char32_t character;
	};

	enum Type
	{
		GainedFocus,
		LostFocus,
		KeyPressed,
		KeyReleased,
		MouseButtonDoubleClicked,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseEntered,
		MouseLeft,
		MouseMoved,
		MouseWheelMoved,
		Moved,
		Quit,
		Resized,
		TextEntered
	};

	Type type;

	union
	{
		KeyEvent key;
		MouseButtonEvent mouseButton;
		MouseMoveEvent mouseMove;
		MouseWheelEvent mouseWheel;
		PositionEvent position;
		SizeEvent size;
		TextEvent text;
	};
};

#endif // NAZARA_EVENT_HPP
