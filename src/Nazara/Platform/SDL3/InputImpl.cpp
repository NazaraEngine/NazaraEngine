// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL3/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Platform/SDL3/SDLHelper.hpp>
#include <Nazara/Platform/SDL3/WindowImpl.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

namespace Nz
{
	ClipboardContentType InputImpl::GetClipboardContentType()
	{
		if (SDL_HasClipboardText())
			return ClipboardContentType::Text;

		return ClipboardContentType::Unknown;
	}

	std::string InputImpl::GetClipboardString()
	{
		char* str = SDL_GetClipboardText();
		if (!str)
			return {};

		NAZARA_DEFER({ SDL_free(str); });

		return std::string(str);
	}

	std::string_view InputImpl::GetKeyName(Keyboard::Scancode key)
	{
		SDL_Scancode scancode = ToSDL(key);

		std::string_view name;
		if (scancode != SDL_SCANCODE_UNKNOWN)
			name = SDL_GetScancodeName(scancode);
		else
			name = "unknown";

		return name;
	}

	std::string_view InputImpl::GetKeyName(Keyboard::VKey key)
	{
		SDL_Keycode vkey = ToSDL(key);

		std::string_view name;
		if (vkey != SDLK_UNKNOWN)
			name = SDL_GetKeyName(vkey);
		else
			name = "unknown";

		return name;
	}

	Vector2f InputImpl::GetMousePosition()
	{
		Vector2f pos;
		SDL_GetGlobalMouseState(&pos.x, &pos.y);

		return pos;
	}

	Vector2f InputImpl::GetMousePosition(const Window& relativeTo)
	{
		Vector2f windowPos = Vector2f(relativeTo.GetPosition());
		Vector2f mousePos = GetMousePosition();

		return mousePos - windowPos;
	}

	bool InputImpl::IsKeyPressed(Keyboard::Scancode key)
	{
		return SDL_GetKeyboardState(nullptr)[ToSDL(key)];
	}

	bool InputImpl::IsKeyPressed(Keyboard::VKey key)
	{
		return IsKeyPressed(ToScanCode(key));
	}

	bool InputImpl::IsMouseButtonPressed(Mouse::Button button)
	{
		static int vButtons[Mouse::Max + 1] = {
			SDL_BUTTON_LMASK,    // Button::Left
			SDL_BUTTON_MMASK,    // Button::Middle
			SDL_BUTTON_RMASK,    // Button::Right
			SDL_BUTTON_X1MASK,   // Button::XButton1
			SDL_BUTTON_X2MASK    // Button::XButton2
		};

		return (SDL_GetGlobalMouseState(nullptr, nullptr) & vButtons[button]) != 0;
	}

	void InputImpl::SetClipboardString(const std::string& str)
	{
		SDL_SetClipboardText(str.c_str());
	}

	void InputImpl::SetMousePosition(float x, float y)
	{
		if (SDL_WarpMouseGlobal(x, y) != 0)
			NazaraWarning("SDL error: {}", SDL_GetError());
	}

	void InputImpl::SetMousePosition(float x, float y, const Window& relativeTo)
	{
		SDL_Window* handle = static_cast<const WindowImpl*>(relativeTo.GetImpl())->GetHandle();
		if (handle)
			SDL_WarpMouseInWindow(handle, x, y);
		else
			NazaraError("invalid window handle");
	}

	Keyboard::Scancode InputImpl::ToScanCode(Keyboard::VKey key)
	{
		return FromSDL(SDL_GetScancodeFromKey(ToSDL(key), nullptr));
	}

	Keyboard::VKey InputImpl::ToVirtualKey(Keyboard::Scancode scancode)
	{
		return FromSDL(SDL_GetKeyFromScancode(ToSDL(scancode), SDL_KMOD_NONE, false));
	}
}
