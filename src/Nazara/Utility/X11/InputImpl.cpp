// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/X11/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Window.hpp>
#include <Nazara/Utility/X11/Display.hpp>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/Xlib.h>
#include <xcb/xcb_keysyms.h>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		KeySym GetKeySym(Keyboard::Key key)
		{
			// X11 keysym correspondant
			KeySym keysym = 0;
			switch (key)
			{
				// Lettres
				case Keyboard::A: keysym = XK_a; break;
				case Keyboard::B: keysym = XK_b; break;
				case Keyboard::C: keysym = XK_c; break;
				case Keyboard::D: keysym = XK_d; break;
				case Keyboard::E: keysym = XK_e; break;
				case Keyboard::F: keysym = XK_f; break;
				case Keyboard::G: keysym = XK_g; break;
				case Keyboard::H: keysym = XK_h; break;
				case Keyboard::I: keysym = XK_i; break;
				case Keyboard::J: keysym = XK_j; break;
				case Keyboard::K: keysym = XK_k; break;
				case Keyboard::L: keysym = XK_l; break;
				case Keyboard::M: keysym = XK_m; break;
				case Keyboard::N: keysym = XK_n; break;
				case Keyboard::O: keysym = XK_o; break;
				case Keyboard::P: keysym = XK_p; break;
				case Keyboard::Q: keysym = XK_q; break;
				case Keyboard::R: keysym = XK_r; break;
				case Keyboard::S: keysym = XK_s; break;
				case Keyboard::T: keysym = XK_t; break;
				case Keyboard::U: keysym = XK_u; break;
				case Keyboard::V: keysym = XK_v; break;
				case Keyboard::W: keysym = XK_w; break;
				case Keyboard::X: keysym = XK_x; break;
				case Keyboard::Y: keysym = XK_y; break;
				case Keyboard::Z: keysym = XK_z; break;

				// Touches de fonction
				case Keyboard::F1: keysym = XK_F1; break;
				case Keyboard::F2: keysym = XK_F2; break;
				case Keyboard::F3: keysym = XK_F3; break;
				case Keyboard::F4: keysym = XK_F4; break;
				case Keyboard::F5: keysym = XK_F5; break;
				case Keyboard::F6: keysym = XK_F6; break;
				case Keyboard::F7: keysym = XK_F7; break;
				case Keyboard::F8: keysym = XK_F8; break;
				case Keyboard::F9: keysym = XK_F9; break;
				case Keyboard::F10: keysym = XK_F10; break;
				case Keyboard::F11: keysym = XK_F11; break;
				case Keyboard::F12: keysym = XK_F12; break;
				case Keyboard::F13: keysym = XK_F13; break;
				case Keyboard::F14: keysym = XK_F14; break;
				case Keyboard::F15: keysym = XK_F15; break;

				// Flèches directionnelles
				case Keyboard::Down: keysym = XK_Down; break;
				case Keyboard::Left: keysym = XK_Left; break;
				case Keyboard::Right: keysym = XK_Right; break;
				case Keyboard::Up: keysym = XK_Up; break;

				// Pavé numérique
				case Keyboard::Add: keysym = XK_KP_Add; break;
				case Keyboard::Decimal: keysym = XK_KP_Decimal; break;
				case Keyboard::Divide: keysym = XK_KP_Divide; break;
				case Keyboard::Multiply: keysym = XK_KP_Multiply; break;
				case Keyboard::Numpad0: keysym = XK_KP_0; break;
				case Keyboard::Numpad1: keysym = XK_KP_1; break;
				case Keyboard::Numpad2: keysym = XK_KP_2; break;
				case Keyboard::Numpad3: keysym = XK_KP_3; break;
				case Keyboard::Numpad4: keysym = XK_KP_4; break;
				case Keyboard::Numpad5: keysym = XK_KP_5; break;
				case Keyboard::Numpad6: keysym = XK_KP_6; break;
				case Keyboard::Numpad7: keysym = XK_KP_7; break;
				case Keyboard::Numpad8: keysym = XK_KP_8; break;
				case Keyboard::Numpad9: keysym = XK_KP_9; break;
				case Keyboard::Subtract: keysym = XK_KP_Subtract; break;

				// Divers
				case Keyboard::Backslash: keysym = XK_backslash; break;
				case Keyboard::Backspace: keysym = XK_BackSpace; break;
				case Keyboard::Clear: keysym = XK_Clear; break;
				case Keyboard::Comma: keysym = XK_comma; break;
				case Keyboard::Dash: keysym = XK_minus; break;
				case Keyboard::Delete: keysym = XK_Delete; break;
				case Keyboard::End: keysym = XK_End; break;
				case Keyboard::Equal: keysym = XK_equal; break;
				case Keyboard::Escape: keysym = XK_Escape; break;
				case Keyboard::Home: keysym = XK_Home; break;
				case Keyboard::Insert: keysym = XK_Insert; break;
				case Keyboard::LAlt: keysym = XK_Alt_L; break;
				case Keyboard::LBracket: keysym = XK_bracketleft; break;
				case Keyboard::LControl: keysym = XK_Control_L; break;
				case Keyboard::LShift: keysym = XK_Shift_L; break;
				case Keyboard::LSystem: keysym = XK_Super_L; break;
				case Keyboard::Num0: keysym = XK_0; break;
				case Keyboard::Num1: keysym = XK_1; break;
				case Keyboard::Num2: keysym = XK_2; break;
				case Keyboard::Num3: keysym = XK_3; break;
				case Keyboard::Num4: keysym = XK_4; break;
				case Keyboard::Num5: keysym = XK_5; break;
				case Keyboard::Num6: keysym = XK_6; break;
				case Keyboard::Num7: keysym = XK_7; break;
				case Keyboard::Num8: keysym = XK_8; break;
				case Keyboard::Num9: keysym = XK_9; break;
				case Keyboard::PageDown: keysym = XK_Page_Down; break;
				case Keyboard::PageUp: keysym = XK_Page_Up; break;
				case Keyboard::Pause: keysym = XK_Pause; break;
				case Keyboard::Period: keysym = XK_period; break;
				case Keyboard::Print: keysym = XK_Print; break;
				case Keyboard::PrintScreen: keysym = XK_Sys_Req; break;
				case Keyboard::Quote: keysym = XK_quotedbl; break;
				case Keyboard::RAlt: keysym = XK_Alt_R; break;
				case Keyboard::RBracket: keysym = XK_bracketright; break;
				case Keyboard::RControl: keysym = XK_Control_R; break;
				case Keyboard::Return: keysym = XK_Return; break;
				case Keyboard::RShift: keysym = XK_Shift_R; break;
				case Keyboard::RSystem: keysym = XK_Super_R; break;
				case Keyboard::Semicolon: keysym = XK_semicolon; break;
				case Keyboard::Slash: keysym = XK_slash; break;
				case Keyboard::Space: keysym = XK_space; break;
				case Keyboard::Tab: keysym = XK_Tab; break;
				case Keyboard::Tilde: keysym = XK_grave; break;

				// Touches navigateur
				case Keyboard::Browser_Back: keysym = XF86XK_Back; break;
				case Keyboard::Browser_Favorites: keysym = XF86XK_Favorites; break;
				case Keyboard::Browser_Forward: keysym = XF86XK_Forward; break;
				case Keyboard::Browser_Home: keysym = XF86XK_HomePage; break;
				case Keyboard::Browser_Refresh: keysym = XF86XK_Refresh; break;
				case Keyboard::Browser_Search: keysym = XF86XK_Search; break;
				case Keyboard::Browser_Stop: keysym = XF86XK_Stop; break;

				// Touches de contrôle
				case Keyboard::Media_Next: keysym = XF86XK_AudioNext; break;
				case Keyboard::Media_Play: keysym = XF86XK_AudioPlay; break;
				case Keyboard::Media_Previous: keysym = XF86XK_AudioPrev; break;
				case Keyboard::Media_Stop: keysym = XF86XK_AudioStop; break;

				// Touches de contrôle du volume
				case Keyboard::Volume_Down: keysym = XF86XK_AudioLowerVolume; break;
				case Keyboard::Volume_Mute: keysym = XF86XK_AudioMute; break;
				case Keyboard::Volume_Up: keysym = XF86XK_AudioRaiseVolume; break;

				// Touches à verrouillage
				case Keyboard::CapsLock: keysym = XK_Caps_Lock; break;
				case Keyboard::NumLock: keysym = XK_Num_Lock; break;
				case Keyboard::ScrollLock: keysym = XK_Scroll_Lock; break;

				default: break;
			}

			 // Sanity checks
			if (key < 0 || key >= Keyboard::Count || keysym == 0)
				NazaraWarning("Key " + String::Number(key) + " is not handled in Keyboard");

			return keysym;
		}
	}

	String EventImpl::GetKeyName(Keyboard::Key key)
	{
		KeySym keySym = GetKeySym(key);

		// XKeysymToString returns a static area.
		return XKeysymToString(keySym);
	}

	Vector2i EventImpl::GetMousePosition()
	{
		ScopedXCBConnection connection;

		ScopedXCB<xcb_generic_error_t> error(nullptr);

		ScopedXCB<xcb_query_pointer_reply_t> pointer(
			xcb_query_pointer_reply(
				connection,
				xcb_query_pointer(
					connection,
					X11::XCBDefaultRootWindow(connection)
				),
				&error
			)
		);

		if (error)
		{
			NazaraError("Failed to query pointer");
			return Vector2i(-1, -1);
		}

		return Vector2i(pointer->root_x, pointer->root_y);
	}

	Vector2i EventImpl::GetMousePosition(const Window& relativeTo)
	{
		WindowHandle handle = relativeTo.GetHandle();
		if (handle)
		{
			// Open a connection with the X server
			ScopedXCBConnection connection;

			ScopedXCB<xcb_generic_error_t> error(nullptr);

			ScopedXCB<xcb_query_pointer_reply_t> pointer(
				xcb_query_pointer_reply(
					connection,
					xcb_query_pointer(
						connection,
						handle
					),
					&error
				)
			);

			if (error)
			{
				NazaraError("Failed to query pointer");
				return Vector2i(-1, -1);
			}

			return Vector2i(pointer->win_x, pointer->win_y);
		}
		else
		{
			NazaraError("No window handle");
			return Vector2i(-1, -1);
		}
	}

	bool EventImpl::IsKeyPressed(Keyboard::Key key)
	{
		ScopedXCBConnection connection;

		xcb_keysym_t keySym = GetKeySym(key);

		xcb_keycode_t realKeyCode = XCB_NO_SYMBOL;

		xcb_key_symbols_t* keySymbols = X11::XCBKeySymbolsAlloc(connection);
		if (!keySymbols)
		{
			NazaraError("Failed to alloc key symbols");
			return false;
		}

		ScopedXCB<xcb_keycode_t> keyCode = xcb_key_symbols_get_keycode(keySymbols, keySym);
		if (!keyCode)
		{
			NazaraError("Failed to get key code");
			return false;
		}

		// One keysym is associated with multiple key codes, we have to find the matching one ...
		int i = 0;
		while (keyCode.get()[i] != XCB_NO_SYMBOL)
		{
			xcb_keycode_t toTry = keyCode.get()[i];
			if (keySym == xcb_key_symbols_get_keysym(keySymbols, toTry, 0))
			{
				realKeyCode = toTry;
				break;
			}
			++i;
		}

		X11::XCBKeySymbolsFree(keySymbols);

		ScopedXCB<xcb_generic_error_t> error(nullptr);

		// Get the whole keyboard state
		ScopedXCB<xcb_query_keymap_reply_t> keymap(
			xcb_query_keymap_reply(
				connection,
				xcb_query_keymap(connection),
				&error
			)
		);

		if (error)
		{
			NazaraError("Failed to query keymap");
			return false;
		}

		// Check our keycode
		return (keymap->keys[realKeyCode / 8] & (1 << (realKeyCode % 8))) != 0;
	}

	bool EventImpl::IsMouseButtonPressed(Mouse::Button button)
	{
		ScopedXCBConnection connection;

		ScopedXCB<xcb_generic_error_t> error(nullptr);

		// Get pointer mask
		ScopedXCB<xcb_query_pointer_reply_t> pointer(
			xcb_query_pointer_reply(
				connection,
				xcb_query_pointer(
					connection,
					X11::XCBDefaultRootWindow(connection)
				),
				&error
			)
		);

		if (error)
		{
			NazaraError("Failed to query pointer");
			return false;
		}

		uint16_t buttons = pointer->mask;

		switch (button)
		{
			case Mouse::Left:     return buttons & XCB_BUTTON_MASK_1;
			case Mouse::Right:    return buttons & XCB_BUTTON_MASK_3;
			case Mouse::Middle:   return buttons & XCB_BUTTON_MASK_2;
			case Mouse::XButton1: return false; // not supported by X
			case Mouse::XButton2: return false; // not supported by X
		}

		NazaraError("Mouse button not supported.");
		return false;
	}

	void EventImpl::SetMousePosition(int x, int y)
	{
		ScopedXCBConnection connection;

		xcb_window_t root = X11::XCBDefaultRootWindow(connection);

		if (!X11::CheckCookie(
			connection,
			xcb_warp_pointer(
				connection,
				None, // Source window
				root, // Destination window
				0, 0, // Source position
				0, 0, // Source size
				x, y  // Destination position
			))
		)
			NazaraError("Failed to set mouse position");

		xcb_flush(connection);
	}

	void EventImpl::SetMousePosition(int x, int y, const Window& relativeTo)
	{
		ScopedXCBConnection connection;

		WindowHandle handle = relativeTo.GetHandle();
		if (handle)
		{
			if (!X11::CheckCookie(
				connection,
				xcb_warp_pointer(
					connection,
					None,   // Source window
					handle, // Destination window
					0, 0,   // Source position
					0, 0,   // Source size
					x, y    // Destination position
				))
			)
				NazaraError("Failed to set mouse position relative to window");

			xcb_flush(connection);
		}
		else
			NazaraError("No window handle");
	}
}
