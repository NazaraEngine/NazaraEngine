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

namespace
{

	KeySym GetKeySym(NzKeyboard::Key key)
	{
		// X11 keysym correspondant
		KeySym keysym = 0;
		switch (key)
		{
			// Lettres
			case NzKeyboard::A: keysym = XK_A; break;
			case NzKeyboard::B: keysym = XK_B; break;
			case NzKeyboard::C: keysym = XK_C; break;
			case NzKeyboard::D: keysym = XK_D; break;
			case NzKeyboard::E: keysym = XK_E; break;
			case NzKeyboard::F: keysym = XK_F; break;
			case NzKeyboard::G: keysym = XK_G; break;
			case NzKeyboard::H: keysym = XK_H; break;
			case NzKeyboard::I: keysym = XK_I; break;
			case NzKeyboard::J: keysym = XK_J; break;
			case NzKeyboard::K: keysym = XK_K; break;
			case NzKeyboard::L: keysym = XK_L; break;
			case NzKeyboard::M: keysym = XK_M; break;
			case NzKeyboard::N: keysym = XK_N; break;
			case NzKeyboard::O: keysym = XK_O; break;
			case NzKeyboard::P: keysym = XK_P; break;
			case NzKeyboard::Q: keysym = XK_Q; break;
			case NzKeyboard::R: keysym = XK_R; break;
			case NzKeyboard::S: keysym = XK_S; break;
			case NzKeyboard::T: keysym = XK_T; break;
			case NzKeyboard::U: keysym = XK_U; break;
			case NzKeyboard::V: keysym = XK_V; break;
			case NzKeyboard::W: keysym = XK_W; break;
			case NzKeyboard::X: keysym = XK_X; break;
			case NzKeyboard::Y: keysym = XK_Y; break;
			case NzKeyboard::Z: keysym = XK_Z; break;

			// Touches de fonction
			case NzKeyboard::F1: keysym = XK_F1; break;
			case NzKeyboard::F2: keysym = XK_F2; break;
			case NzKeyboard::F3: keysym = XK_F3; break;
			case NzKeyboard::F4: keysym = XK_F4; break;
			case NzKeyboard::F5: keysym = XK_F5; break;
			case NzKeyboard::F6: keysym = XK_F6; break;
			case NzKeyboard::F7: keysym = XK_F7; break;
			case NzKeyboard::F8: keysym = XK_F8; break;
			case NzKeyboard::F9: keysym = XK_F9; break;
			case NzKeyboard::F10: keysym = XK_F10; break;
			case NzKeyboard::F11: keysym = XK_F11; break;
			case NzKeyboard::F12: keysym = XK_F12; break;
			case NzKeyboard::F13: keysym = XK_F13; break;
			case NzKeyboard::F14: keysym = XK_F14; break;
			case NzKeyboard::F15: keysym = XK_F15; break;

			// Flèches directionnelles
			case NzKeyboard::Down: keysym = XK_Down; break;
			case NzKeyboard::Left: keysym = XK_Left; break;
			case NzKeyboard::Right: keysym = XK_Right; break;
			case NzKeyboard::Up: keysym = XK_Up; break;

			// Pavé numérique
			case NzKeyboard::Add: keysym = XK_KP_Add; break;
			case NzKeyboard::Decimal: keysym = XK_KP_Decimal; break;
			case NzKeyboard::Divide: keysym = XK_KP_Divide; break;
			case NzKeyboard::Multiply: keysym = XK_KP_Multiply; break;
			case NzKeyboard::Numpad0: keysym = XK_KP_0; break;
			case NzKeyboard::Numpad1: keysym = XK_KP_1; break;
			case NzKeyboard::Numpad2: keysym = XK_KP_2; break;
			case NzKeyboard::Numpad3: keysym = XK_KP_3; break;
			case NzKeyboard::Numpad4: keysym = XK_KP_4; break;
			case NzKeyboard::Numpad5: keysym = XK_KP_5; break;
			case NzKeyboard::Numpad6: keysym = XK_KP_6; break;
			case NzKeyboard::Numpad7: keysym = XK_KP_7; break;
			case NzKeyboard::Numpad8: keysym = XK_KP_8; break;
			case NzKeyboard::Numpad9: keysym = XK_KP_9; break;
			case NzKeyboard::Subtract: keysym = XK_KP_Subtract; break;

			// Divers
			case NzKeyboard::Backslash: keysym = XK_backslash; break;
			case NzKeyboard::Backspace: keysym = XK_BackSpace; break;
			case NzKeyboard::Clear: keysym = XK_Clear; break;
			case NzKeyboard::Comma: keysym = XK_comma; break;
			case NzKeyboard::Dash: keysym = XK_minus; break;
			case NzKeyboard::Delete: keysym = XK_Delete; break;
			case NzKeyboard::End: keysym = XK_End; break;
			case NzKeyboard::Equal: keysym = XK_equal; break;
			case NzKeyboard::Escape: keysym = XK_Escape; break;
			case NzKeyboard::Home: keysym = XK_Home; break;
			case NzKeyboard::Insert: keysym = XK_Insert; break;
			case NzKeyboard::LAlt: keysym = XK_Alt_L; break;
			case NzKeyboard::LBracket: keysym = XK_bracketleft; break;
			case NzKeyboard::LControl: keysym = XK_Control_L; break;
			case NzKeyboard::LShift: keysym = XK_Shift_L; break;
			case NzKeyboard::LSystem: keysym = XK_Super_L; break;
			case NzKeyboard::Num0: keysym = XK_0; break;
			case NzKeyboard::Num1: keysym = XK_1; break;
			case NzKeyboard::Num2: keysym = XK_2; break;
			case NzKeyboard::Num3: keysym = XK_3; break;
			case NzKeyboard::Num4: keysym = XK_4; break;
			case NzKeyboard::Num5: keysym = XK_5; break;
			case NzKeyboard::Num6: keysym = XK_6; break;
			case NzKeyboard::Num7: keysym = XK_7; break;
			case NzKeyboard::Num8: keysym = XK_8; break;
			case NzKeyboard::Num9: keysym = XK_9; break;
			case NzKeyboard::PageDown: keysym = XK_Page_Down; break;
			case NzKeyboard::PageUp: keysym = XK_Page_Up; break;
			case NzKeyboard::Pause: keysym = XK_Pause; break;
			case NzKeyboard::Period: keysym = XK_period; break;
			case NzKeyboard::Print: keysym = XK_Print; break;
			case NzKeyboard::PrintScreen: keysym = XK_Sys_Req; break;
			case NzKeyboard::Quote: keysym = XK_quotedbl; break;
			case NzKeyboard::RAlt: keysym = XK_Alt_R; break;
			case NzKeyboard::RBracket: keysym = XK_bracketright; break;
			case NzKeyboard::RControl: keysym = XK_Control_R; break;
			case NzKeyboard::Return: keysym = XK_Return; break;
			case NzKeyboard::RShift: keysym = XK_Shift_R; break;
			case NzKeyboard::RSystem: keysym = XK_Super_R; break;
			case NzKeyboard::Semicolon: keysym = XK_semicolon; break;
			case NzKeyboard::Slash: keysym = XK_slash; break;
			case NzKeyboard::Space: keysym = XK_space; break;
			case NzKeyboard::Tab: keysym = XK_Tab; break;
			case NzKeyboard::Tilde: keysym = XK_grave; break;

			// Touches navigateur
			case NzKeyboard::Browser_Back: keysym = XF86XK_Back; break;
			case NzKeyboard::Browser_Favorites: keysym = XF86XK_Favorites; break;
			case NzKeyboard::Browser_Forward: keysym = XF86XK_Forward; break;
			case NzKeyboard::Browser_Home: keysym = XF86XK_HomePage; break;
			case NzKeyboard::Browser_Refresh: keysym = XF86XK_Refresh; break;
			case NzKeyboard::Browser_Search: keysym = XF86XK_Search; break;
			case NzKeyboard::Browser_Stop: keysym = XF86XK_Stop; break;

			// Touches de contrôle
			case NzKeyboard::Media_Next: keysym = XF86XK_AudioNext; break;
			case NzKeyboard::Media_Play: keysym = XF86XK_AudioPlay; break;
			case NzKeyboard::Media_Previous: keysym = XF86XK_AudioPrev; break;
			case NzKeyboard::Media_Stop: keysym = XF86XK_AudioStop; break;

			// Touches de contrôle du volume
			case NzKeyboard::Volume_Down: keysym = XF86XK_AudioLowerVolume; break;
			case NzKeyboard::Volume_Mute: keysym = XF86XK_AudioMute; break;
			case NzKeyboard::Volume_Up: keysym = XF86XK_AudioRaiseVolume; break;

			// Touches à verrouillage
			case NzKeyboard::CapsLock: keysym = XK_Caps_Lock; break;
			case NzKeyboard::NumLock: keysym = XK_Num_Lock; break;
			case NzKeyboard::ScrollLock: keysym = XK_Scroll_Lock; break;

			default: break;
		}

		 // Sanity checks
		if (key < 0 || key >= NzKeyboard::Count || keysym == 0)
			NazaraWarning("Key " + NzString::Number(key) + " is not handled in NzKeyboard");

		return keysym;
	}
}

NzString NzEventImpl::GetKeyName(NzKeyboard::Key key)
{
	KeySym keySym = GetKeySym(key);

	// XKeysymToString returns a static area.
	return XKeysymToString(keySym);
}

NzVector2i NzEventImpl::GetMousePosition()
{
	NzScopedXCBConnection connection;

	NzScopedXCB<xcb_generic_error_t> error(nullptr);

	NzScopedXCB<xcb_query_pointer_reply_t> pointer(
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
		return NzVector2i(-1, -1);
	}

	return NzVector2i(pointer->root_x, pointer->root_y);
}

NzVector2i NzEventImpl::GetMousePosition(const NzWindow& relativeTo)
{
	NzWindowHandle handle = relativeTo.GetHandle();
	if (handle)
	{
		// Open a connection with the X server
		NzScopedXCBConnection connection;

		NzScopedXCB<xcb_generic_error_t> error(nullptr);

		NzScopedXCB<xcb_query_pointer_reply_t> pointer(
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
			return NzVector2i(-1, -1);
		}

		return NzVector2i(pointer->win_x, pointer->win_y);
	}
	else
	{
		NazaraError("No window handle");
		return NzVector2i(-1, -1);
	}
}

bool NzEventImpl::IsKeyPressed(NzKeyboard::Key key)
{
	NzScopedXCBConnection connection;

	xcb_keysym_t keySym = GetKeySym(key);

	xcb_key_symbols_t* keySymbols = X11::XCBKeySymbolsAlloc(connection);
	if (!keySymbols)
	{
		NazaraError("Failed to alloc key symbols");
		return false;
	}

	NzScopedXCB<xcb_keycode_t> keyCode = xcb_key_symbols_get_keycode(keySymbols, keySym);
	if (!keyCode)
	{
		NazaraError("Failed to get key code");
		return false;
	}
	X11::XCBKeySymbolsFree(keySymbols);

	NzScopedXCB<xcb_generic_error_t> error(nullptr);

	// Get the whole keyboard state
	NzScopedXCB<xcb_query_keymap_reply_t> keymap(
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
	return (keymap->keys[*keyCode.get() / 8] & (1 << (*keyCode.get() % 8))) != 0;
}

bool NzEventImpl::IsMouseButtonPressed(NzMouse::Button button)
{
	NzScopedXCBConnection connection;

	NzScopedXCB<xcb_generic_error_t> error(nullptr);

	// Get pointer mask
	NzScopedXCB<xcb_query_pointer_reply_t> pointer(
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
		case NzMouse::Left:     return buttons & XCB_BUTTON_MASK_1;
		case NzMouse::Right:    return buttons & XCB_BUTTON_MASK_3;
		case NzMouse::Middle:   return buttons & XCB_BUTTON_MASK_2;
		case NzMouse::XButton1: return false; // not supported by X
		case NzMouse::XButton2: return false; // not supported by X
	}

	NazaraError("Mouse button not supported.");
	return false;
}

void NzEventImpl::SetMousePosition(int x, int y)
{
	NzScopedXCBConnection connection;

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

void NzEventImpl::SetMousePosition(int x, int y, const NzWindow& relativeTo)
{
	NzScopedXCBConnection connection;

	NzWindowHandle handle = relativeTo.GetHandle();
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
