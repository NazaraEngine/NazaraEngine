// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_KEYBOARD_HPP
#define NAZARA_KEYBOARD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API Keyboard
	{
		public:
			enum Key
			{
				Undefined = -1,

				// Lettres
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

				// Touches de fonction
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

				// Flèches directionnelles
				Down,
				Left,
				Right,
				Up,

				// Pavé numérique
				Add,
				Decimal,
				Divide,
				Multiply,
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

				// Divers
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

				// Touches navigateur
				Browser_Back,
				Browser_Favorites,
				Browser_Forward,
				Browser_Home,
				Browser_Refresh,
				Browser_Search,
				Browser_Stop,

				// Touches de contrôle de lecture
				Media_Next,
				Media_Play,
				Media_Previous,
				Media_Stop,

				// Touches de contrôle du volume
				Volume_Down,
				Volume_Mute,
				Volume_Up,

				// Touches à verrouillage
				CapsLock,
				NumLock,
				ScrollLock,

				Count
			};

			Keyboard() = delete;
			~Keyboard() = delete;

			static String GetKeyName(Key key);
			static bool IsKeyPressed(Key key);
	};
}

#endif // NAZARA_KEYBOARD_HPP
