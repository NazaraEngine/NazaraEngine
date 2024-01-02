// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspired by the SFML of Laurent Gomila (and its team)

#pragma once

#ifndef NAZARA_PLATFORM_KEYBOARD_HPP
#define NAZARA_PLATFORM_KEYBOARD_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>
#include <string>

namespace Nz
{
	class NAZARA_PLATFORM_API Keyboard
	{
		public:
			enum class Scancode
			{
				Undefined = -1,

				// Letters
				A,
				B,
				C,
				D,
				E,
				F,
				G,
				H,
				I,
				J,
				K,
				L,
				M,
				N,
				O,
				P,
				Q,
				R,
				S,
				T,
				U,
				V,
				W,
				X,
				Y,
				Z,

				// Functional keys
				F1,
				F2,
				F3,
				F4,
				F5,
				F6,
				F7,
				F8,
				F9,
				F10,
				F11,
				F12,
				F13,
				F14,
				F15,

				// Directional keys
				Down,
				Left,
				Right,
				Up,

				// Numerical pad
				Add,
				Decimal,
				Divide,
				Multiply,
				NumpadReturn,
				Numpad0,
				Numpad1,
				Numpad2,
				Numpad3,
				Numpad4,
				Numpad5,
				Numpad6,
				Numpad7,
				Numpad8,
				Numpad9,
				Subtract,

				// Various
				Backslash,
				Backspace,
				Clear,
				Comma,
				Dash,
				Delete,
				End,
				Equal,
				Escape,
				Home,
				Insert,
				LAlt,
				LBracket,
				LControl,
				LShift,
				LSystem,
				Num0,
				Num1,
				Num2,
				Num3,
				Num4,
				Num5,
				Num6,
				Num7,
				Num8,
				Num9,
				PageDown,
				PageUp,
				Pause,
				Period,
				Print,
				PrintScreen,
				Quote,
				RAlt,
				RBracket,
				RControl,
				Return,
				RShift,
				RSystem,
				Semicolon,
				Slash,
				Space,
				Tab,
				Tilde,
				Menu,
				ISOBackslash102,

				// Navigator keys
				Browser_Back,
				Browser_Favorites,
				Browser_Forward,
				Browser_Home,
				Browser_Refresh,
				Browser_Search,
				Browser_Stop,

				// Lecture control keys
				Media_Next,
				Media_Play,
				Media_Previous,
				Media_Stop,

				// Volume control keys
				Volume_Down,
				Volume_Mute,
				Volume_Up,

				// Locking keys
				CapsLock,
				NumLock,
				ScrollLock,

				Max = ScrollLock
			};

			enum class VKey
			{
				Undefined = -1,

				// Letters
				A,
				B,
				C,
				D,
				E,
				F,
				G,
				H,
				I,
				J,
				K,
				L,
				M,
				N,
				O,
				P,
				Q,
				R,
				S,
				T,
				U,
				V,
				W,
				X,
				Y,
				Z,

				// Functional keys
				F1,
				F2,
				F3,
				F4,
				F5,
				F6,
				F7,
				F8,
				F9,
				F10,
				F11,
				F12,
				F13,
				F14,
				F15,

				// Directional keys
				Down,
				Left,
				Right,
				Up,

				// Numerical pad
				Add,
				Decimal,
				Divide,
				Multiply,
				NumpadReturn,
				Numpad0,
				Numpad1,
				Numpad2,
				Numpad3,
				Numpad4,
				Numpad5,
				Numpad6,
				Numpad7,
				Numpad8,
				Numpad9,
				Subtract,

				// Various
				Backslash,
				Backspace,
				Clear,
				Comma,
				Dash,
				Delete,
				End,
				Equal,
				Escape,
				Home,
				Insert,
				LAlt,
				LBracket,
				LControl,
				LShift,
				LSystem,
				Num0,
				Num1,
				Num2,
				Num3,
				Num4,
				Num5,
				Num6,
				Num7,
				Num8,
				Num9,
				PageDown,
				PageUp,
				Pause,
				Period,
				Print,
				PrintScreen,
				Quote,
				RAlt,
				RBracket,
				RControl,
				Return,
				RShift,
				RSystem,
				Semicolon,
				Slash,
				Space,
				Tab,
				Tilde,
				Menu,
				ISOBackslash102,

				// Navigator keys
				Browser_Back,
				Browser_Favorites,
				Browser_Forward,
				Browser_Home,
				Browser_Refresh,
				Browser_Search,
				Browser_Stop,

				// Lecture control keys
				Media_Next,
				Media_Play,
				Media_Previous,
				Media_Stop,

				// Volume control keys
				Volume_Down,
				Volume_Mute,
				Volume_Up,

				// Locking keys
				CapsLock,
				NumLock,
				ScrollLock,

				Max = ScrollLock
			};

			Keyboard() = delete;
			~Keyboard() = delete;

			static std::string GetKeyName(Scancode scancode);
			static std::string GetKeyName(VKey key);
			static bool IsKeyPressed(Scancode scancode);
			static bool IsKeyPressed(VKey key);
			static void StartTextInput();
			static void StopTextInput();
			static Scancode ToScanCode(VKey key);
			static VKey ToVirtualKey(Scancode key);
	};
}

#endif // NAZARA_PLATFORM_KEYBOARD_HPP
