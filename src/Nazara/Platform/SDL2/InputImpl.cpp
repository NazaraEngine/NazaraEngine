// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/SDL2/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Platform/SDL2/SDLHelper.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <SDL_clipboard.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <Nazara/Platform/Debug.hpp>

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

		CallOnExit freeStr([=] { SDL_free(str); });

		return std::string(str);
	}

	std::string InputImpl::GetKeyName(Keyboard::Scancode key)
	{
		SDL_Scancode scancode = SDLHelper::ToSDL(key);

		std::string name;
		if (scancode != SDL_SCANCODE_UNKNOWN)
			name = SDL_GetScancodeName(scancode);
		else
			name = "unknown";

		return name;
	}

	std::string InputImpl::GetKeyName(Keyboard::VKey key)
	{
		SDL_Keycode vkey = SDLHelper::ToSDL(key);

		std::string name;
		if (vkey != SDLK_UNKNOWN)
			name = SDL_GetKeyName(vkey);
		else
			name = "unknown";

		return name;
	}

	Vector2i InputImpl::GetMousePosition()
	{
		Vector2i pos;
		SDL_GetGlobalMouseState(&pos.x, &pos.y);

		return pos;
	}

	Vector2i InputImpl::GetMousePosition(const Window& relativeTo)
	{
		auto windowPos = relativeTo.GetPosition();
		auto mousePos = GetMousePosition();

		return mousePos - windowPos;
	}

	bool InputImpl::IsKeyPressed(Keyboard::Scancode key)
	{
		return SDL_GetKeyboardState(nullptr)[SDLHelper::ToSDL(key)];
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

	bool InputImpl::SetRelativeMouseMode(bool relativeMouseMode)
	{
		return SDL_SetRelativeMouseMode((relativeMouseMode) ? SDL_TRUE : SDL_FALSE) == 0;
	}

	void InputImpl::SetMousePosition(int x, int y)
	{
		if (SDL_WarpMouseGlobal(x, y) != 0)
			NazaraWarning(SDL_GetError());
	}

	void InputImpl::SetMousePosition(int x, int y, const Window& relativeTo)
	{
		SDL_Window* handle = static_cast<const WindowImpl*>(relativeTo.GetImpl())->GetHandle();
		if (handle)
			SDL_WarpMouseInWindow(handle, x, y);
		else
			NazaraError("invalid window handle");
	}

	void InputImpl::StartTextInput()
	{
		SDL_StartTextInput();
	}

	void InputImpl::StopTextInput()
	{
		SDL_StopTextInput();
	}

	Keyboard::Scancode InputImpl::ToScanCode(Keyboard::VKey key)
	{
		return SDLHelper::FromSDL(SDL_GetScancodeFromKey(SDLHelper::ToSDL(key)));
	}

	Keyboard::VKey InputImpl::ToVirtualKey(Keyboard::Scancode scancode)
	{
		return SDLHelper::FromSDL(SDL_GetKeyFromScancode(SDLHelper::ToSDL(scancode)));
	}
}
