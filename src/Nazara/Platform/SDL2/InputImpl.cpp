// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/SDL2/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Debug.hpp>
#include <Nazara/Platform/SDL2/SDLHelper.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <Nazara/Platform/Window.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>

namespace Nz
{
	std::string EventImpl::GetKeyName(Keyboard::Scancode key)
	{
		SDL_Scancode scancode = SDLHelper::ToSDL(key);

		std::string name;
		if (scancode != SDL_SCANCODE_UNKNOWN)
			name = SDL_GetScancodeName(scancode);
		else
			name = "Unknown";

		return name;
	}

	std::string EventImpl::GetKeyName(Keyboard::VKey key)
	{
		SDL_Keycode vkey = SDLHelper::ToSDL(key);

		std::string name;
		if (vkey != SDLK_UNKNOWN)
			name = SDL_GetKeyName(vkey);
		else
			name = "Unknown";

		return name;
	}

	Vector2i EventImpl::GetMousePosition()
	{
		Vector2i pos;
		SDL_GetGlobalMouseState(&pos.x, &pos.y);

		return pos;
	}

	Vector2i EventImpl::GetMousePosition(const Window& relativeTo)
	{
		auto windowPos = relativeTo.GetPosition();
		auto mousePos = GetMousePosition();

		return mousePos - windowPos;
	}

	bool EventImpl::IsKeyPressed(Keyboard::Scancode key)
	{
		return SDL_GetKeyboardState(nullptr)[SDLHelper::ToSDL(key)];
	}

	bool EventImpl::IsKeyPressed(Keyboard::VKey key)
	{
		return IsKeyPressed(ToScanCode(key));
	}

	bool EventImpl::IsMouseButtonPressed(Mouse::Button button)
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

	bool EventImpl::SetRelativeMouseMode(bool relativeMouseMode)
	{
		return SDL_SetRelativeMouseMode((relativeMouseMode) ? SDL_TRUE : SDL_FALSE) == 0;
	}

	void EventImpl::SetMousePosition(int x, int y)
	{
		if (SDL_WarpMouseGlobal(x, y) != 0)
			NazaraWarning(SDL_GetError());
	}

	void EventImpl::SetMousePosition(int x, int y, const Window& relativeTo)
	{
		SDL_Window* handle = static_cast<const WindowImpl*>(relativeTo.GetImpl())->GetHandle();
		if (handle)
			SDL_WarpMouseInWindow(handle, x, y);
		else
			NazaraError("Invalid window handle");
	}

	void EventImpl::StartTextInput()
	{
		SDL_StartTextInput();
	}

	void EventImpl::StopTextInput()
	{
		SDL_StopTextInput();
	}

	Keyboard::Scancode EventImpl::ToScanCode(Keyboard::VKey key)
	{
		return SDLHelper::FromSDL(SDL_GetScancodeFromKey(SDLHelper::ToSDL(key)));
	}

	Keyboard::VKey EventImpl::ToVirtualKey(Keyboard::Scancode scancode)
	{
		return SDLHelper::FromSDL(SDL_GetKeyFromScancode(SDLHelper::ToSDL(scancode)));
	}
}
