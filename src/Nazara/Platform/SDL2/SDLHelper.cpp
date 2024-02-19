// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL2/SDLHelper.hpp>

namespace Nz
{
	namespace
	{
		SDL_Scancode nzScancodeToSDLScanCode[static_cast<std::size_t>(Keyboard::VKey::Max) + 1] = {
			// Lettres
			SDL_SCANCODE_A, // Key::A
			SDL_SCANCODE_B, // Key::B
			SDL_SCANCODE_C, // Key::C
			SDL_SCANCODE_D, // Key::D
			SDL_SCANCODE_E, // Key::E
			SDL_SCANCODE_F, // Key::F
			SDL_SCANCODE_G, // Key::G
			SDL_SCANCODE_H, // Key::H
			SDL_SCANCODE_I, // Key::I
			SDL_SCANCODE_J, // Key::J
			SDL_SCANCODE_K, // Key::K
			SDL_SCANCODE_L, // Key::L
			SDL_SCANCODE_M, // Key::M
			SDL_SCANCODE_N, // Key::N
			SDL_SCANCODE_O, // Key::O
			SDL_SCANCODE_P, // Key::P
			SDL_SCANCODE_Q, // Key::Q
			SDL_SCANCODE_R, // Key::R
			SDL_SCANCODE_S, // Key::S
			SDL_SCANCODE_T, // Key::T
			SDL_SCANCODE_U, // Key::U
			SDL_SCANCODE_V, // Key::V
			SDL_SCANCODE_W, // Key::W
			SDL_SCANCODE_X, // Key::X
			SDL_SCANCODE_Y, // Key::Y
			SDL_SCANCODE_Z, // Key::Z

			// Touches de fonction
			SDL_SCANCODE_F1,    // Key::F1
			SDL_SCANCODE_F2,    // Key::F2
			SDL_SCANCODE_F3,    // Key::F3
			SDL_SCANCODE_F4,    // Key::F4
			SDL_SCANCODE_F5,    // Key::F5
			SDL_SCANCODE_F6,    // Key::F6
			SDL_SCANCODE_F7,    // Key::F7
			SDL_SCANCODE_F8,    // Key::F8
			SDL_SCANCODE_F9,    // Key::F9
			SDL_SCANCODE_F10,   // Key::F10
			SDL_SCANCODE_F11,   // Key::F11
			SDL_SCANCODE_F12,   // Key::F12
			SDL_SCANCODE_F13,   // Key::F13
			SDL_SCANCODE_F14,   // Key::F14
			SDL_SCANCODE_F15,   // Key::F15

			// Flèches directionnelles
			SDL_SCANCODE_DOWN,  // Key::Down
			SDL_SCANCODE_LEFT,  // Key::Left
			SDL_SCANCODE_RIGHT, // Key::Right
			SDL_SCANCODE_UP,    // Key::Up

			// Pavé numérique
			SDL_SCANCODE_KP_PLUS,       // Key::Add
			SDL_SCANCODE_KP_PERIOD,     // Key::Decimal
			SDL_SCANCODE_KP_DIVIDE,     // Key::Divide
			SDL_SCANCODE_KP_MULTIPLY,   // Key::Multiply
			SDL_SCANCODE_KP_ENTER,      // Key::NumpadReturn
			SDL_SCANCODE_KP_0,          // Key::Numpad0
			SDL_SCANCODE_KP_1,          // Key::Numpad1
			SDL_SCANCODE_KP_2,          // Key::Numpad2
			SDL_SCANCODE_KP_3,          // Key::Numpad3
			SDL_SCANCODE_KP_4,          // Key::Numpad4
			SDL_SCANCODE_KP_5,          // Key::Numpad5
			SDL_SCANCODE_KP_6,          // Key::Numpad6
			SDL_SCANCODE_KP_7,          // Key::Numpad7
			SDL_SCANCODE_KP_8,          // Key::Numpad8
			SDL_SCANCODE_KP_9,          // Key::Numpad9
			SDL_SCANCODE_KP_MINUS,      // Key::Subtract

			// Divers
			SDL_SCANCODE_BACKSLASH,     // Key::Backslash
			SDL_SCANCODE_BACKSPACE,     // Key::Backspace
			SDL_SCANCODE_CLEAR,         // Key::Clear
			SDL_SCANCODE_COMMA,         // Key::Comma,
			SDL_SCANCODE_MINUS,         // Key::Dash
			SDL_SCANCODE_DELETE,        // Key::Delete
			SDL_SCANCODE_END,           // Key::End
			SDL_SCANCODE_EQUALS,        // Key::Equal
			SDL_SCANCODE_ESCAPE,        // Key::Escape
			SDL_SCANCODE_HOME,          // Key::Home
			SDL_SCANCODE_INSERT,        // Key::Insert
			SDL_SCANCODE_LALT,          // Key::LAlt
			SDL_SCANCODE_LEFTBRACKET,   // Key::LBracket
			SDL_SCANCODE_LCTRL,         // Key::LControl
			SDL_SCANCODE_LSHIFT,        // Key::LShift
			SDL_SCANCODE_LGUI,          // Key::LSystem
			SDL_SCANCODE_0,             // Key::Num0
			SDL_SCANCODE_1,             // Key::Num1
			SDL_SCANCODE_2,             // Key::Num2
			SDL_SCANCODE_3,             // Key::Num3
			SDL_SCANCODE_4,             // Key::Num4
			SDL_SCANCODE_5,             // Key::Num5
			SDL_SCANCODE_6,             // Key::Num6
			SDL_SCANCODE_7,             // Key::Num7
			SDL_SCANCODE_8,             // Key::Num8
			SDL_SCANCODE_9,             // Key::Num9
			SDL_SCANCODE_PAGEDOWN,      // Key::PageDown
			SDL_SCANCODE_PAGEUP,        // Key::PageUp
			SDL_SCANCODE_PAUSE,         // Key::Pause
			SDL_SCANCODE_PERIOD,        // Key::Period
			SDL_SCANCODE_SYSREQ,        // Key::Print
			SDL_SCANCODE_PRINTSCREEN,   // Key::PrintScreen
			SDL_SCANCODE_APOSTROPHE,    // Key::Quote
			SDL_SCANCODE_RALT,          // Key::RAlt
			SDL_SCANCODE_RIGHTBRACKET,  // Key::RBracket
			SDL_SCANCODE_RCTRL,         // Key::RControl
			SDL_SCANCODE_RETURN,        // Key::Return
			SDL_SCANCODE_RSHIFT,        // Key::RShift
			SDL_SCANCODE_RGUI,          // Key::RSystem
			SDL_SCANCODE_SEMICOLON,     // Key::Semicolon
			SDL_SCANCODE_SLASH,         // Key::Slash
			SDL_SCANCODE_SPACE,         // Key::Space
			SDL_SCANCODE_TAB,           // Key::Tab
			SDL_SCANCODE_GRAVE,         // Key::Tilde
			SDL_SCANCODE_APPLICATION,   // Key::Menu
			SDL_SCANCODE_NONUSBACKSLASH,// Key::ISOBackslash102

			// Touches navigateur
			SDL_SCANCODE_AC_BACK,       // Key::Browser_Back
			SDL_SCANCODE_AC_BOOKMARKS,  // Key::Browser_Favorites
			SDL_SCANCODE_AC_FORWARD,    // Key::Browser_Forward
			SDL_SCANCODE_AC_HOME,       // Key::Browser_Home
			SDL_SCANCODE_AC_REFRESH,    // Key::Browser_Refresh
			SDL_SCANCODE_AC_SEARCH,     // Key::Browser_Search
			SDL_SCANCODE_AC_STOP,       // Key::Browser_Stop

			// Touches de contrôle
			SDL_SCANCODE_AUDIONEXT,     // Key::Media_Next,
			SDL_SCANCODE_AUDIOPLAY,     // Key::Media_PlayPause,
			SDL_SCANCODE_AUDIOPREV,     // Key::Media_Previous,
			SDL_SCANCODE_AUDIOSTOP,     // Key::Media_Stop,

			// Touches de contrôle du volume
			SDL_SCANCODE_VOLUMEDOWN,    // Key::Volume_Down
			SDL_SCANCODE_MUTE,          // Key::Volume_Mute
			SDL_SCANCODE_VOLUMEUP,      // Key::Volume_Up

			// Touches à verrouillage
			SDL_SCANCODE_CAPSLOCK,      // Key::CapsLock
			SDL_SCANCODE_NUMLOCKCLEAR,  // Key::NumLock
			SDL_SCANCODE_SCROLLLOCK     // Key::ScrollLock
		};
		SDL_Keycode nzVKeyToSDLVKey[static_cast<std::size_t>(Keyboard::VKey::Max) + 1] = {
			// Keys
			SDLK_a,             // VKey::A
			SDLK_b,             // VKey::B
			SDLK_c,             // VKey::C
			SDLK_d,             // VKey::D
			SDLK_e,             // VKey::E
			SDLK_f,             // VKey::F
			SDLK_g,             // VKey::G
			SDLK_h,             // VKey::H
			SDLK_i,             // VKey::I
			SDLK_j,             // VKey::J
			SDLK_k,             // VKey::K
			SDLK_l,             // VKey::L
			SDLK_m,             // VKey::M
			SDLK_n,             // VKey::N
			SDLK_o,             // VKey::O
			SDLK_p,             // VKey::P
			SDLK_q,             // VKey::Q
			SDLK_r,             // VKey::R
			SDLK_s,             // VKey::S
			SDLK_t,             // VKey::T
			SDLK_u,             // VKey::U
			SDLK_v,             // VKey::V
			SDLK_w,             // VKey::W
			SDLK_x,             // VKey::X
			SDLK_y,             // VKey::Y
			SDLK_z,             // VKey::Z

			// Function keys
			SDLK_F1,            // VKey::F1
			SDLK_F2,            // VKey::F2
			SDLK_F3,            // VKey::F3
			SDLK_F4,            // VKey::F4
			SDLK_F5,            // VKey::F5
			SDLK_F6,            // VKey::F6
			SDLK_F7,            // VKey::F7
			SDLK_F8,            // VKey::F8
			SDLK_F9,            // VKey::F9
			SDLK_F10,           // VKey::F10
			SDLK_F11,           // VKey::F11
			SDLK_F12,           // VKey::F12
			SDLK_F13,           // VKey::F13
			SDLK_F14,           // VKey::F14
			SDLK_F15,           // VKey::F15

			// Arrows
			SDLK_DOWN,          // VKey::Down
			SDLK_LEFT,          // VKey::Left
			SDLK_RIGHT,         // VKey::Right
			SDLK_UP,            // VKey::Up

			// Keypad
			SDLK_KP_PLUS,       // VKey::Add
			SDLK_KP_PERIOD,     // VKey::Decimal
			SDLK_KP_DIVIDE,     // VKey::Divide
			SDLK_KP_MULTIPLY,   // VKey::Multiply
			SDLK_KP_ENTER,      // VKey::NumpadReturn
			SDLK_KP_0,          // VKey::Numpad0
			SDLK_KP_1,          // VKey::Numpad1
			SDLK_KP_2,          // VKey::Numpad2
			SDLK_KP_3,          // VKey::Numpad3
			SDLK_KP_4,          // VKey::Numpad4
			SDLK_KP_5,          // VKey::Numpad5
			SDLK_KP_6,          // VKey::Numpad6
			SDLK_KP_7,          // VKey::Numpad7
			SDLK_KP_8,          // VKey::Numpad8
			SDLK_KP_9,          // VKey::Numpad9
			SDLK_KP_MINUS,      // VKey::Subtract

			// Divers
			SDLK_BACKSLASH,     // VKey::Backslash
			SDLK_BACKSPACE,     // VKey::Backspace
			SDLK_CLEAR,         // VKey::Clear
			SDLK_COMMA,         // VKey::Comma,
			SDLK_MINUS,         // VKey::Dash
			SDLK_DELETE,        // VKey::Delete
			SDLK_END,           // VKey::End
			SDLK_EQUALS,        // VKey::Equal
			SDLK_ESCAPE,        // VKey::Escape
			SDLK_HOME,          // VKey::Home
			SDLK_INSERT,        // VKey::Insert
			SDLK_LALT,          // VKey::LAlt
			SDLK_LEFTBRACKET,   // VKey::LBracket
			SDLK_LCTRL,         // VKey::LControl
			SDLK_LSHIFT,        // VKey::LShift
			SDLK_LGUI,          // VKey::LSystem
			SDLK_0,             // VKey::Num0
			SDLK_1,             // VKey::Num1
			SDLK_2,             // VKey::Num2
			SDLK_3,             // VKey::Num3
			SDLK_4,             // VKey::Num4
			SDLK_5,             // VKey::Num5
			SDLK_6,             // VKey::Num6
			SDLK_7,             // VKey::Num7
			SDLK_8,             // VKey::Num8
			SDLK_9,             // VKey::Num9
			SDLK_PAGEDOWN,      // VKey::PageDown
			SDLK_PAGEUP,        // VKey::PageUp
			SDLK_PAUSE,         // VKey::Pause
			SDLK_PERIOD,        // VKey::Period
			SDLK_SYSREQ,        // VKey::Print
			SDLK_PRINTSCREEN,   // VKey::PrintScreen
			SDLK_QUOTE,         // VKey::Quote
			SDLK_RALT,          // VKey::RAlt
			SDLK_RIGHTBRACKET,  // VKey::RBracket
			SDLK_RCTRL,         // VKey::RControl
			SDLK_RETURN,        // VKey::Return
			SDLK_RSHIFT,        // VKey::RShift
			SDLK_RGUI,          // VKey::RSystem
			SDLK_SEMICOLON,     // VKey::Semicolon
			SDLK_SLASH,         // VKey::Slash
			SDLK_SPACE,         // VKey::Space
			SDLK_TAB,           // VKey::Tab
			SDLK_BACKQUOTE,     // VKey::Tilde
			SDLK_APPLICATION,   // VKey::Menu
			SDLK_UNKNOWN,       // VKey::ISOBackslash102

			// Browser control
			SDLK_AC_BACK,       // VKey::Browser_Back
			SDLK_AC_BOOKMARKS,  // VKey::Browser_Favorites
			SDLK_AC_FORWARD,    // VKey::Browser_Forward
			SDLK_AC_HOME,       // VKey::Browser_Home
			SDLK_AC_REFRESH,    // VKey::Browser_Refresh
			SDLK_AC_SEARCH,     // VKey::Browser_Search
			SDLK_AC_STOP,       // VKey::Browser_Stop

			// Audio control
			SDLK_AUDIONEXT,     // VKey::Media_Next,
			SDLK_AUDIOPLAY,     // VKey::Media_PlayPause,
			SDLK_AUDIOPREV,     // VKey::Media_Previous,
			SDLK_AUDIOSTOP,     // VKey::Media_Stop,

			// Volume control
			SDLK_VOLUMEDOWN,    // VKey::Volume_Down
			SDLK_MUTE,          // VKey::Volume_Mute
			SDLK_VOLUMEUP,      // VKey::Volume_Up

			// Lock keys
			SDLK_CAPSLOCK,      // VKey::CapsLock
			SDLK_NUMLOCKCLEAR,  // VKey::NumLock
			SDLK_SCROLLLOCK     // VKey::ScrollLock
		};
	}

	Keyboard::Scancode SDLHelper::FromSDL(SDL_Scancode scancode)
	{
		switch (scancode)
		{
			case SDL_SCANCODE_LCTRL:             return Keyboard::Scancode::LControl;
			case SDL_SCANCODE_RCTRL:             return Keyboard::Scancode::RControl;
			case SDL_SCANCODE_LALT:              return Keyboard::Scancode::LAlt;
			case SDL_SCANCODE_RALT:              return Keyboard::Scancode::RAlt;
			case SDL_SCANCODE_LSHIFT:            return Keyboard::Scancode::LShift;
			case SDL_SCANCODE_RSHIFT:            return Keyboard::Scancode::RShift;

			case SDL_SCANCODE_0:                 return Keyboard::Scancode::Num0;
			case SDL_SCANCODE_1:                 return Keyboard::Scancode::Num1;
			case SDL_SCANCODE_2:                 return Keyboard::Scancode::Num2;
			case SDL_SCANCODE_3:                 return Keyboard::Scancode::Num3;
			case SDL_SCANCODE_4:                 return Keyboard::Scancode::Num4;
			case SDL_SCANCODE_5:                 return Keyboard::Scancode::Num5;
			case SDL_SCANCODE_6:                 return Keyboard::Scancode::Num6;
			case SDL_SCANCODE_7:                 return Keyboard::Scancode::Num7;
			case SDL_SCANCODE_8:                 return Keyboard::Scancode::Num8;
			case SDL_SCANCODE_9:                 return Keyboard::Scancode::Num9;
			case SDL_SCANCODE_A:                 return Keyboard::Scancode::A;
			case SDL_SCANCODE_B:                 return Keyboard::Scancode::B;
			case SDL_SCANCODE_C:                 return Keyboard::Scancode::C;
			case SDL_SCANCODE_D:                 return Keyboard::Scancode::D;
			case SDL_SCANCODE_E:                 return Keyboard::Scancode::E;
			case SDL_SCANCODE_F:                 return Keyboard::Scancode::F;
			case SDL_SCANCODE_G:                 return Keyboard::Scancode::G;
			case SDL_SCANCODE_H:                 return Keyboard::Scancode::H;
			case SDL_SCANCODE_I:                 return Keyboard::Scancode::I;
			case SDL_SCANCODE_J:                 return Keyboard::Scancode::J;
			case SDL_SCANCODE_K:                 return Keyboard::Scancode::K;
			case SDL_SCANCODE_L:                 return Keyboard::Scancode::L;
			case SDL_SCANCODE_M:                 return Keyboard::Scancode::M;
			case SDL_SCANCODE_N:                 return Keyboard::Scancode::N;
			case SDL_SCANCODE_O:                 return Keyboard::Scancode::O;
			case SDL_SCANCODE_P:                 return Keyboard::Scancode::P;
			case SDL_SCANCODE_Q:                 return Keyboard::Scancode::Q;
			case SDL_SCANCODE_R:                 return Keyboard::Scancode::R;
			case SDL_SCANCODE_S:                 return Keyboard::Scancode::S;
			case SDL_SCANCODE_T:                 return Keyboard::Scancode::T;
			case SDL_SCANCODE_U:                 return Keyboard::Scancode::U;
			case SDL_SCANCODE_V:                 return Keyboard::Scancode::V;
			case SDL_SCANCODE_W:                 return Keyboard::Scancode::W;
			case SDL_SCANCODE_X:                 return Keyboard::Scancode::X;
			case SDL_SCANCODE_Y:                 return Keyboard::Scancode::Y;
			case SDL_SCANCODE_Z:                 return Keyboard::Scancode::Z;
			case SDL_SCANCODE_KP_PLUS:           return Keyboard::Scancode::Add;
			case SDL_SCANCODE_BACKSPACE:         return Keyboard::Scancode::Backspace;
			case SDL_SCANCODE_AC_BACK:           return Keyboard::Scancode::Browser_Back;
			case SDL_SCANCODE_AC_BOOKMARKS:      return Keyboard::Scancode::Browser_Favorites;
			case SDL_SCANCODE_AC_FORWARD:        return Keyboard::Scancode::Browser_Forward;
			case SDL_SCANCODE_AC_HOME:           return Keyboard::Scancode::Browser_Home;
			case SDL_SCANCODE_AC_REFRESH:        return Keyboard::Scancode::Browser_Refresh;
			case SDL_SCANCODE_AC_SEARCH:         return Keyboard::Scancode::Browser_Search;
			case SDL_SCANCODE_AC_STOP:           return Keyboard::Scancode::Browser_Stop;
			case SDL_SCANCODE_CAPSLOCK:          return Keyboard::Scancode::CapsLock;
			case SDL_SCANCODE_CLEAR:             return Keyboard::Scancode::Clear;
			case SDL_SCANCODE_KP_PERIOD:         return Keyboard::Scancode::Decimal;
			case SDL_SCANCODE_DELETE:            return Keyboard::Scancode::Delete;
			case SDL_SCANCODE_KP_DIVIDE:         return Keyboard::Scancode::Divide;
			case SDL_SCANCODE_DOWN:              return Keyboard::Scancode::Down;
			case SDL_SCANCODE_END:               return Keyboard::Scancode::End;
			case SDL_SCANCODE_ESCAPE:            return Keyboard::Scancode::Escape;
			case SDL_SCANCODE_F1:                return Keyboard::Scancode::F1;
			case SDL_SCANCODE_F2:                return Keyboard::Scancode::F2;
			case SDL_SCANCODE_F3:                return Keyboard::Scancode::F3;
			case SDL_SCANCODE_F4:                return Keyboard::Scancode::F4;
			case SDL_SCANCODE_F5:                return Keyboard::Scancode::F5;
			case SDL_SCANCODE_F6:                return Keyboard::Scancode::F6;
			case SDL_SCANCODE_F7:                return Keyboard::Scancode::F7;
			case SDL_SCANCODE_F8:                return Keyboard::Scancode::F8;
			case SDL_SCANCODE_F9:                return Keyboard::Scancode::F9;
			case SDL_SCANCODE_F10:               return Keyboard::Scancode::F10;
			case SDL_SCANCODE_F11:               return Keyboard::Scancode::F11;
			case SDL_SCANCODE_F12:               return Keyboard::Scancode::F12;
			case SDL_SCANCODE_F13:               return Keyboard::Scancode::F13;
			case SDL_SCANCODE_F14:               return Keyboard::Scancode::F14;
			case SDL_SCANCODE_F15:               return Keyboard::Scancode::F15;
			case SDL_SCANCODE_HOME:              return Keyboard::Scancode::Home;
			case SDL_SCANCODE_INSERT:            return Keyboard::Scancode::Insert;
			case SDL_SCANCODE_LEFT:              return Keyboard::Scancode::Left;
			case SDL_SCANCODE_LGUI:              return Keyboard::Scancode::LSystem;
			case SDL_SCANCODE_AUDIONEXT:         return Keyboard::Scancode::Media_Next;
			case SDL_SCANCODE_AUDIOPLAY:         return Keyboard::Scancode::Media_Play;
			case SDL_SCANCODE_AUDIOPREV:         return Keyboard::Scancode::Media_Previous;
			case SDL_SCANCODE_AUDIOSTOP:         return Keyboard::Scancode::Media_Stop;
			case SDL_SCANCODE_KP_MULTIPLY:       return Keyboard::Scancode::Multiply;
			case SDL_SCANCODE_PAGEDOWN:          return Keyboard::Scancode::PageDown;
			case SDL_SCANCODE_KP_0:              return Keyboard::Scancode::Numpad0;
			case SDL_SCANCODE_KP_1:              return Keyboard::Scancode::Numpad1;
			case SDL_SCANCODE_KP_2:              return Keyboard::Scancode::Numpad2;
			case SDL_SCANCODE_KP_3:              return Keyboard::Scancode::Numpad3;
			case SDL_SCANCODE_KP_4:              return Keyboard::Scancode::Numpad4;
			case SDL_SCANCODE_KP_5:              return Keyboard::Scancode::Numpad5;
			case SDL_SCANCODE_KP_6:              return Keyboard::Scancode::Numpad6;
			case SDL_SCANCODE_KP_7:              return Keyboard::Scancode::Numpad7;
			case SDL_SCANCODE_KP_8:              return Keyboard::Scancode::Numpad8;
			case SDL_SCANCODE_KP_9:              return Keyboard::Scancode::Numpad9;
			case SDL_SCANCODE_NUMLOCKCLEAR:      return Keyboard::Scancode::NumLock;
			case SDL_SCANCODE_SEMICOLON:         return Keyboard::Scancode::Semicolon;
			case SDL_SCANCODE_SLASH:             return Keyboard::Scancode::Slash;
			case SDL_SCANCODE_GRAVE:             return Keyboard::Scancode::Tilde;
			case SDL_SCANCODE_APPLICATION:       return Keyboard::Scancode::Menu;
			case SDL_SCANCODE_NONUSBACKSLASH:    return Keyboard::Scancode::ISOBackslash102;
			case SDL_SCANCODE_LEFTBRACKET:       return Keyboard::Scancode::LBracket;
			case SDL_SCANCODE_BACKSLASH:         return Keyboard::Scancode::Backslash;
			case SDL_SCANCODE_RIGHTBRACKET:      return Keyboard::Scancode::RBracket;
			case SDL_SCANCODE_APOSTROPHE:        return Keyboard::Scancode::Quote;
			case SDL_SCANCODE_COMMA:             return Keyboard::Scancode::Comma;
			case SDL_SCANCODE_MINUS:             return Keyboard::Scancode::Dash;
			case SDL_SCANCODE_PERIOD:            return Keyboard::Scancode::Period;
			case SDL_SCANCODE_EQUALS:            return Keyboard::Scancode::Equal;
			case SDL_SCANCODE_RIGHT:             return Keyboard::Scancode::Right;
			case SDL_SCANCODE_PAGEUP:            return Keyboard::Scancode::PageUp;
			case SDL_SCANCODE_PAUSE:             return Keyboard::Scancode::Pause;
			case SDL_SCANCODE_SYSREQ:            return Keyboard::Scancode::Print;
			case SDL_SCANCODE_SCROLLLOCK:        return Keyboard::Scancode::ScrollLock;
			case SDL_SCANCODE_PRINTSCREEN:       return Keyboard::Scancode::PrintScreen;
			case SDL_SCANCODE_KP_MINUS:          return Keyboard::Scancode::Subtract;
			case SDL_SCANCODE_RETURN:            return Keyboard::Scancode::Return;
			case SDL_SCANCODE_KP_ENTER:          return Keyboard::Scancode::NumpadReturn;
			case SDL_SCANCODE_RGUI:              return Keyboard::Scancode::RSystem;
			case SDL_SCANCODE_SPACE:             return Keyboard::Scancode::Space;
			case SDL_SCANCODE_TAB:               return Keyboard::Scancode::Tab;
			case SDL_SCANCODE_UP:                return Keyboard::Scancode::Up;
			case SDL_SCANCODE_VOLUMEDOWN:        return Keyboard::Scancode::Volume_Down;
			case SDL_SCANCODE_MUTE:              return Keyboard::Scancode::Volume_Mute;
			case SDL_SCANCODE_AUDIOMUTE:         return Keyboard::Scancode::Volume_Mute;
			case SDL_SCANCODE_VOLUMEUP:          return Keyboard::Scancode::Volume_Up;

			default:
				return Keyboard::Scancode::Undefined;
		}
	}

	Keyboard::VKey SDLHelper::FromSDL(SDL_Keycode keycode)
	{
		switch (keycode)
		{
			case SDLK_LCTRL:             return Keyboard::VKey::LControl;
			case SDLK_RCTRL:             return Keyboard::VKey::RControl;
			case SDLK_LALT:              return Keyboard::VKey::LAlt;
			case SDLK_RALT:              return Keyboard::VKey::RAlt;
			case SDLK_LSHIFT:            return Keyboard::VKey::LShift;
			case SDLK_RSHIFT:            return Keyboard::VKey::RShift;

			case SDLK_0:                 return Keyboard::VKey::Num0;
			case SDLK_1:                 return Keyboard::VKey::Num1;
			case SDLK_2:                 return Keyboard::VKey::Num2;
			case SDLK_3:                 return Keyboard::VKey::Num3;
			case SDLK_4:                 return Keyboard::VKey::Num4;
			case SDLK_5:                 return Keyboard::VKey::Num5;
			case SDLK_6:                 return Keyboard::VKey::Num6;
			case SDLK_7:                 return Keyboard::VKey::Num7;
			case SDLK_8:                 return Keyboard::VKey::Num8;
			case SDLK_9:                 return Keyboard::VKey::Num9;
			case SDLK_a:                 return Keyboard::VKey::A;
			case SDLK_b:                 return Keyboard::VKey::B;
			case SDLK_c:                 return Keyboard::VKey::C;
			case SDLK_d:                 return Keyboard::VKey::D;
			case SDLK_e:                 return Keyboard::VKey::E;
			case SDLK_f:                 return Keyboard::VKey::F;
			case SDLK_g:                 return Keyboard::VKey::G;
			case SDLK_h:                 return Keyboard::VKey::H;
			case SDLK_i:                 return Keyboard::VKey::I;
			case SDLK_j:                 return Keyboard::VKey::J;
			case SDLK_k:                 return Keyboard::VKey::K;
			case SDLK_l:                 return Keyboard::VKey::L;
			case SDLK_m:                 return Keyboard::VKey::M;
			case SDLK_n:                 return Keyboard::VKey::N;
			case SDLK_o:                 return Keyboard::VKey::O;
			case SDLK_p:                 return Keyboard::VKey::P;
			case SDLK_q:                 return Keyboard::VKey::Q;
			case SDLK_r:                 return Keyboard::VKey::R;
			case SDLK_s:                 return Keyboard::VKey::S;
			case SDLK_t:                 return Keyboard::VKey::T;
			case SDLK_u:                 return Keyboard::VKey::U;
			case SDLK_v:                 return Keyboard::VKey::V;
			case SDLK_w:                 return Keyboard::VKey::W;
			case SDLK_x:                 return Keyboard::VKey::X;
			case SDLK_y:                 return Keyboard::VKey::Y;
			case SDLK_z:                 return Keyboard::VKey::Z;
			case SDLK_KP_PLUS:           return Keyboard::VKey::Add;
			case SDLK_BACKSPACE:         return Keyboard::VKey::Backspace;
			case SDLK_AC_BACK:           return Keyboard::VKey::Browser_Back;
			case SDLK_AC_BOOKMARKS:      return Keyboard::VKey::Browser_Favorites;
			case SDLK_AC_FORWARD:        return Keyboard::VKey::Browser_Forward;
			case SDLK_AC_HOME:           return Keyboard::VKey::Browser_Home;
			case SDLK_AC_REFRESH:        return Keyboard::VKey::Browser_Refresh;
			case SDLK_AC_SEARCH:         return Keyboard::VKey::Browser_Search;
			case SDLK_AC_STOP:           return Keyboard::VKey::Browser_Stop;
			case SDLK_CAPSLOCK:          return Keyboard::VKey::CapsLock;
			case SDLK_CLEAR:             return Keyboard::VKey::Clear;
			case SDLK_KP_PERIOD:         return Keyboard::VKey::Decimal;
			case SDLK_DELETE:            return Keyboard::VKey::Delete;
			case SDLK_KP_DIVIDE:         return Keyboard::VKey::Divide;
			case SDLK_DOWN:              return Keyboard::VKey::Down;
			case SDLK_END:               return Keyboard::VKey::End;
			case SDLK_ESCAPE:            return Keyboard::VKey::Escape;
			case SDLK_F1:                return Keyboard::VKey::F1;
			case SDLK_F2:                return Keyboard::VKey::F2;
			case SDLK_F3:                return Keyboard::VKey::F3;
			case SDLK_F4:                return Keyboard::VKey::F4;
			case SDLK_F5:                return Keyboard::VKey::F5;
			case SDLK_F6:                return Keyboard::VKey::F6;
			case SDLK_F7:                return Keyboard::VKey::F7;
			case SDLK_F8:                return Keyboard::VKey::F8;
			case SDLK_F9:                return Keyboard::VKey::F9;
			case SDLK_F10:               return Keyboard::VKey::F10;
			case SDLK_F11:               return Keyboard::VKey::F11;
			case SDLK_F12:               return Keyboard::VKey::F12;
			case SDLK_F13:               return Keyboard::VKey::F13;
			case SDLK_F14:               return Keyboard::VKey::F14;
			case SDLK_F15:               return Keyboard::VKey::F15;
			case SDLK_HOME:              return Keyboard::VKey::Home;
			case SDLK_INSERT:            return Keyboard::VKey::Insert;
			case SDLK_LEFT:              return Keyboard::VKey::Left;
			case SDLK_LGUI:              return Keyboard::VKey::LSystem;
			case SDLK_AUDIONEXT:         return Keyboard::VKey::Media_Next;
			case SDLK_AUDIOPLAY:         return Keyboard::VKey::Media_Play;
			case SDLK_AUDIOPREV:         return Keyboard::VKey::Media_Previous;
			case SDLK_AUDIOSTOP:         return Keyboard::VKey::Media_Stop;
			case SDLK_KP_MULTIPLY:       return Keyboard::VKey::Multiply;
			case SDLK_PAGEDOWN:          return Keyboard::VKey::PageDown;
			case SDLK_KP_0:              return Keyboard::VKey::Numpad0;
			case SDLK_KP_1:              return Keyboard::VKey::Numpad1;
			case SDLK_KP_2:              return Keyboard::VKey::Numpad2;
			case SDLK_KP_3:              return Keyboard::VKey::Numpad3;
			case SDLK_KP_4:              return Keyboard::VKey::Numpad4;
			case SDLK_KP_5:              return Keyboard::VKey::Numpad5;
			case SDLK_KP_6:              return Keyboard::VKey::Numpad6;
			case SDLK_KP_7:              return Keyboard::VKey::Numpad7;
			case SDLK_KP_8:              return Keyboard::VKey::Numpad8;
			case SDLK_KP_9:              return Keyboard::VKey::Numpad9;
			case SDLK_NUMLOCKCLEAR:      return Keyboard::VKey::NumLock;
			case SDLK_SEMICOLON:         return Keyboard::VKey::Semicolon;
			case SDLK_SLASH:             return Keyboard::VKey::Slash;
			case SDLK_BACKQUOTE:         return Keyboard::VKey::Tilde;
			case SDLK_APPLICATION:       return Keyboard::VKey::Menu;
			case SDLK_LEFTBRACKET:       return Keyboard::VKey::LBracket;
			case SDLK_BACKSLASH:         return Keyboard::VKey::Backslash;
			case SDLK_RIGHTBRACKET:      return Keyboard::VKey::RBracket;
			case SDLK_QUOTE:             return Keyboard::VKey::Quote;
			case SDLK_COMMA:             return Keyboard::VKey::Comma;
			case SDLK_MINUS:             return Keyboard::VKey::Dash;
			case SDLK_PERIOD:            return Keyboard::VKey::Period;
			case SDLK_EQUALS:            return Keyboard::VKey::Equal;
			case SDLK_RIGHT:             return Keyboard::VKey::Right;
			case SDLK_PAGEUP:            return Keyboard::VKey::PageUp;
			case SDLK_PAUSE:             return Keyboard::VKey::Pause;
			case SDLK_SYSREQ:            return Keyboard::VKey::Print;
			case SDLK_SCROLLLOCK:        return Keyboard::VKey::ScrollLock;
			case SDLK_PRINTSCREEN:       return Keyboard::VKey::PrintScreen;
			case SDLK_KP_MINUS:          return Keyboard::VKey::Subtract;
			case SDLK_RETURN:            return Keyboard::VKey::Return;
			case SDLK_KP_ENTER:          return Keyboard::VKey::NumpadReturn;
			case SDLK_RGUI:              return Keyboard::VKey::RSystem;
			case SDLK_SPACE:             return Keyboard::VKey::Space;
			case SDLK_TAB:               return Keyboard::VKey::Tab;
			case SDLK_UP:                return Keyboard::VKey::Up;
			case SDLK_VOLUMEDOWN:        return Keyboard::VKey::Volume_Down;
			case SDLK_MUTE:              return Keyboard::VKey::Volume_Mute;
			case SDLK_AUDIOMUTE:         return Keyboard::VKey::Volume_Mute;
			case SDLK_VOLUMEUP:          return Keyboard::VKey::Volume_Up;

			default:
				return Keyboard::VKey::Undefined;
		}
	}

	SDL_Scancode SDLHelper::ToSDL(Keyboard::Scancode scancode)
	{
		if (scancode == Keyboard::Scancode::Undefined)
			return SDL_SCANCODE_UNKNOWN;

		return nzScancodeToSDLScanCode[static_cast<std::size_t>(scancode)];
	}

	SDL_Keycode SDLHelper::ToSDL(Keyboard::VKey keycode)
	{
		if (keycode == Keyboard::VKey::Undefined)
			return SDLK_UNKNOWN;

		return nzVKeyToSDLVKey[static_cast<std::size_t>(keycode)];
	}

}
