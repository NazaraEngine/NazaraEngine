// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Un grand merci à Laurent Gomila pour la SFML qui m'aura bien aidé à réaliser cette implémentation

#include <cstdio>
#include <memory>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <Nazara/Utility/Image.hpp>
#include <SDL2/SDL.h>

namespace Nz
{
	namespace
	{
		WindowImpl* fullscreenWindow = nullptr;

		Mouse::Button SDLToNazaraButton(Uint8 sdlButton)
		{
			switch (sdlButton)
			{
				case SDL_BUTTON_LEFT:
					return Mouse::Left;
				case SDL_BUTTON_MIDDLE:
					return Mouse::Middle;
				case SDL_BUTTON_RIGHT:
					return Mouse::Right;
				case SDL_BUTTON_X1:
					return Mouse::XButton1;
				case SDL_BUTTON_X2:
					return Mouse::XButton2;
				default:
					NazaraAssert(false, "Unkown mouse button");
					return Mouse::Left;
			}
		}
	}

	WindowImpl::WindowImpl(Window* parent) :
		m_cursor(nullptr),
		m_handle(nullptr),
		//m_callback(0),
		m_style(0),
		m_maxSize(-1),
		m_minSize(-1),
		m_parent(parent),
		m_keyRepeat(true),
		m_mouseInside(false),
		m_smoothScrolling(false),
		m_scrolling(0)
	{
		m_cursor = SDL_GetDefaultCursor();
	}

	bool WindowImpl::Create(const VideoMode& mode, const String& title, WindowStyleFlags style)
	{
		bool async = (style & WindowStyle_Threaded) != 0;
		if (async)
		{
			NazaraError("SDL2 backend doesn't support asyn window for now");

			return false;
		}


		bool fullscreen = (style & WindowStyle_Fullscreen) != 0;

		Uint32 winStyle = SDL_WINDOW_OPENGL;

		unsigned int x, y;
		unsigned int width = mode.width;
		unsigned int height = mode.height;
		if (fullscreen)
			winStyle |= SDL_WINDOW_FULLSCREEN;

		// Testé une seconde fois car sa valeur peut changer
		if (fullscreen)
		{
			x = 0;
			y = 0;

			fullscreenWindow = this;
		}
		else
		{
			if (!(style & WindowStyle_Titlebar))
				winStyle |= SDL_WINDOW_BORDERLESS;

			x = SDL_WINDOWPOS_CENTERED;
			y = SDL_WINDOWPOS_CENTERED;
		}

		if (style & WindowStyle_Resizable)
			winStyle |= SDL_WINDOW_RESIZABLE;
		if (style & WindowStyle_Max)
			winStyle |= SDL_WINDOW_MAXIMIZED;

		m_eventListener = true;
		m_ownsWindow = true;
		m_sizemove = false;
		m_style = style;

		m_handle = SDL_CreateWindow(title.GetConstBuffer(), x, y, width, height, winStyle);

		if (!m_handle)
		{
			NazaraError("Failed to create window: " + Error::GetLastSystemError());
			return false;
		}

		PrepareWindow(fullscreen);

		SDL_AddEventWatch(HandleEvent, this);

		return true;
	}

	bool WindowImpl::Create(WindowHandle handle)
	{
		m_handle = static_cast<SDL_Window*>(handle);

		if (!m_handle || !SDL_GetWindowID(m_handle))
		{
			NazaraError("Invalid handle");
			return false;
		}

		m_eventListener = false;
		m_ownsWindow = false;
		m_sizemove = false;

		SDL_GetWindowPosition(m_handle, &m_position.x, &m_position.y);

		int width;
		int height;
		SDL_GetWindowSize(m_handle, &width, &height);

		m_size.Set(width, height);

		SDL_AddEventWatch(HandleEvent, this);

		return true;
	}

	void WindowImpl::Destroy()
	{
		if (m_ownsWindow && m_handle)
			SDL_DestroyWindow(m_handle);
		else
			SetEventListener(false);

		SDL_DelEventWatch(HandleEvent, this);
	}

	void WindowImpl::EnableKeyRepeat(bool enable)
	{
		m_keyRepeat = enable;
	}

	void WindowImpl::EnableSmoothScrolling(bool enable)
	{
		m_smoothScrolling = enable;
	}

	WindowHandle WindowImpl::GetHandle() const
	{
		return m_handle;
	}

	Vector2i WindowImpl::GetPosition() const
	{
		return m_position;
	}

	Vector2ui WindowImpl::GetSize() const
	{
		return m_size;
	}

	WindowStyleFlags WindowImpl::GetStyle() const
	{
		return m_style;
	}

	String WindowImpl::GetTitle() const
	{
		return String::Unicode(SDL_GetWindowTitle(m_handle));
	}

	bool WindowImpl::HasFocus() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_INPUT_FOCUS) != 0;
	}

	void WindowImpl::IgnoreNextMouseEvent(int mouseX, int mouseY)
	{
		m_ignoreNextMouseMove = true;
		// Petite astuce ... probablement foireuse dans certains cas :ahde:
		m_mousePos.x = mouseX;
		m_mousePos.y = mouseY;
	}

	bool WindowImpl::IsMinimized() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_MINIMIZED) != 0;
	}

	bool WindowImpl::IsVisible() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_SHOWN) != 0;
	}

	void WindowImpl::RefreshCursor()
	{
		if (!m_cursor)
		{
			if (SDL_ShowCursor(SDL_DISABLE) < 0)
				NazaraWarning(SDL_GetError());
		}
		else
		{
			if (SDL_ShowCursor(SDL_ENABLE) < 0)
				NazaraWarning(SDL_GetError());

			SDL_SetCursor(m_cursor);
		}
	}

	void WindowImpl::ProcessEvents(bool block)
	{
		SDL_PumpEvents();


		/*if (m_ownsWindow)
		   {
		    if (block)
		        WaitMessage();

		    MSG message;
		    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		    {
		        TranslateMessage(&message);
		        DispatchMessageW(&message);
		    }
		   }*/
	}

	int SDLCALL WindowImpl::HandleEvent(void *userdata, SDL_Event* event)
	{
		try {
			auto window = static_cast<WindowImpl*>(userdata);

			WindowEvent evt;
			evt.type = WindowEventType::WindowEventType_Max;

			switch (event->type)
			{
				case SDL_WINDOWEVENT:
					if (SDL_GetWindowID(window->m_handle) != event->window.windowID)
						return 0;

					switch (event->window.event)
					{
						case SDL_WINDOWEVENT_CLOSE:
							evt.type = Nz::WindowEventType::WindowEventType_Quit;
							break;
						case SDL_WINDOWEVENT_RESIZED:
							evt.type = Nz::WindowEventType::WindowEventType_Resized;

							evt.size.width = event->window.data1;
							evt.size.height = event->window.data2;

							window->m_size.Set(event->window.data1, event->window.data2);

							break;
						case SDL_WINDOWEVENT_MOVED:
							evt.type = Nz::WindowEventType::WindowEventType_Moved;

							evt.position.x = event->window.data1;
							evt.position.y = event->window.data2;

							window->m_position.Set(event->window.data1, event->window.data2);

							break;
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							evt.type = Nz::WindowEventType::WindowEventType_GainedFocus;

							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							evt.type = Nz::WindowEventType::WindowEventType_LostFocus;

							break;
						case SDL_WINDOWEVENT_ENTER:
							evt.type = Nz::WindowEventType::WindowEventType_MouseEntered;

							break;
						case SDL_WINDOWEVENT_LEAVE:
							evt.type = Nz::WindowEventType::WindowEventType_MouseLeft;

							break;
					}
					break;

				case SDL_MOUSEMOTION:
					if (SDL_GetWindowID(window->m_handle) != event->motion.windowID)
						return 0;

					if (window->m_ignoreNextMouseMove && event->motion.x == window->m_mousePos.x && event->motion.y == window->m_mousePos.y)
					{
						window->m_ignoreNextMouseMove = false;

						return 0;
					}

					evt.type = Nz::WindowEventType::WindowEventType_MouseMoved;

					evt.mouseMove.x = event->motion.x;
					evt.mouseMove.y = event->motion.y;
					evt.mouseMove.deltaX = event->motion.xrel;
					evt.mouseMove.deltaY = event->motion.yrel;

					break;

				case SDL_MOUSEBUTTONDOWN:
					if (SDL_GetWindowID(window->m_handle) != event->button.windowID)
						return 0;

					evt.mouseButton.button = SDLToNazaraButton(event->button.button);
					evt.mouseButton.x = event->button.x;
					evt.mouseButton.y = event->button.y;

					if (event->button.clicks % 2 == 0)
					{
						evt.type = Nz::WindowEventType::WindowEventType_MouseButtonDoubleClicked;

						window->m_parent->PushEvent(evt);
					}

					evt.type = Nz::WindowEventType::WindowEventType_MouseButtonPressed;

					break;

				case SDL_MOUSEBUTTONUP:
					if (SDL_GetWindowID(window->m_handle) != event->button.windowID)
						return 0;

					evt.mouseButton.button = SDLToNazaraButton(event->button.button);
					evt.mouseButton.x = event->button.x;
					evt.mouseButton.y = event->button.y;

					evt.type = Nz::WindowEventType::WindowEventType_MouseButtonReleased;

					break;

				case SDL_MOUSEWHEEL:
					if (SDL_GetWindowID(window->m_handle) != event->wheel.windowID)
						return 0;

					evt.type = Nz::WindowEventType::WindowEventType_MouseWheelMoved;

					evt.mouseWheel.delta = event->wheel.y;

					break;

				case SDL_KEYDOWN:
					if (SDL_GetWindowID(window->m_handle) != event->key.windowID)
						return 0;

					evt.type = WindowEventType_KeyPressed;

					evt.key.code = SDLKeySymToNazaraKey(event->key.keysym);
					evt.key.alt = (event->key.keysym.mod & KMOD_ALT) != 0;
					evt.key.control = (event->key.keysym.mod & KMOD_CTRL) != 0;
					evt.key.repeated = event->key.repeat != 0;
					evt.key.shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
					evt.key.system = (event->key.keysym.mod & KMOD_GUI) != 0;

					break;

				case SDL_KEYUP:
					if (SDL_GetWindowID(window->m_handle) != event->key.windowID)
						return 0;

					evt.type = WindowEventType_KeyReleased;

					evt.key.code = SDLKeySymToNazaraKey(event->key.keysym);
					evt.key.alt = (event->key.keysym.mod & KMOD_ALT) != 0;
					evt.key.control = (event->key.keysym.mod & KMOD_CTRL) != 0;
					evt.key.repeated = event->key.repeat != 0;
					evt.key.shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
					evt.key.system = (event->key.keysym.mod & KMOD_GUI) != 0;

					break;

				case SDL_TEXTINPUT:
					if (SDL_GetWindowID(window->m_handle) != event->text.windowID)
						return 0;

					evt.type = WindowEventType_TextEntered;

					for (decltype(evt.text.character)codepoint : String::Unicode(event->text.text).GetUtf32String())
					{
						evt.text.character = codepoint;

						window->m_parent->PushEvent(evt);
					}

					// prevent post switch event
					evt.type = WindowEventType::WindowEventType_Max;

					break;
			}

			if (evt.type != WindowEventType::WindowEventType_Max)
				window->m_parent->PushEvent(evt);
		}
		catch (std::exception e)
		{
			NazaraError(e.what());
		}
		catch (...)     // Don't let any exceptions go thru C calls
		{
			NazaraError("An unknown error happened");
		}

		return 0;
	}

	void WindowImpl::SetCursor(const Cursor& cursor)
	{
		m_cursor = cursor.m_impl->GetCursor();

		if (HasFocus())
			RefreshCursor();
	}

	void WindowImpl::SetEventListener(bool listener)
	{

	}

	void WindowImpl::SetFocus()
	{
		SDL_RaiseWindow(m_handle);
	}

	void WindowImpl::SetIcon(const Icon& icon)
	{
		SDL_SetWindowIcon(m_handle, icon.m_impl->GetIcon());
	}

	void WindowImpl::SetMaximumSize(int width, int height)
	{
		SDL_SetWindowMaximumSize(m_handle, width, height);
	}

	void WindowImpl::SetMinimumSize(int width, int height)
	{
		SDL_SetWindowMinimumSize(m_handle, width, height);
	}

	void WindowImpl::SetPosition(int x, int y)
	{
		SDL_SetWindowPosition(m_handle, x, y);
	}

	void WindowImpl::SetSize(unsigned int width, unsigned int height)
	{
		m_size.Set(width, height);
		SDL_SetWindowSize(m_handle, width, height);
	}

	void WindowImpl::SetStayOnTop(bool stayOnTop)
	{
		NazaraDebug("Stay on top isn't supported by SDL2 backend for now");
	}

	void WindowImpl::SetTitle(const String& title)
	{
		SDL_SetWindowTitle(m_handle, title.GetConstBuffer());
	}

	void WindowImpl::SetVisible(bool visible)
	{
		visible ? SDL_ShowWindow(m_handle) : SDL_HideWindow(m_handle);
	}

	void WindowImpl::PrepareWindow(bool fullscreen)
	{
		(void)fullscreen;     // ignore param warning

		SDL_GetWindowPosition(m_handle, &m_position.x, &m_position.y);

		int width, height;
		SDL_GetWindowSize(m_handle, &width, &height);

		m_size.Set(width, height);
	}

	bool WindowImpl::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			NazaraError(SDL_GetError());
			return false;
		}
		if (SDL_GL_LoadLibrary(nullptr) < 0)
		{
			NazaraError(SDL_GetError());

			SDL_Quit();
			return false;
		}

		if (SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, true) < 0)
			NazaraError("Couldn't set share OpenGL contexes");

		return true;
	}

	void WindowImpl::Uninitialize()
	{
		SDL_Quit();
	}

	Keyboard::Key WindowImpl::SDLKeySymToNazaraKey(SDL_Keysym& keysym)
	{
		auto key = keysym.scancode;

		switch (key)
		{
			case SDL_SCANCODE_LCTRL:           return Keyboard::LControl;
			case SDL_SCANCODE_RCTRL:           return Keyboard::RControl;
			case SDL_SCANCODE_LALT:            return Keyboard::LAlt;
			case SDL_SCANCODE_RALT:            return Keyboard::RAlt;
			case SDL_SCANCODE_LSHIFT:          return Keyboard::LShift;
			case SDL_SCANCODE_RSHIFT:          return Keyboard::RShift;

			case SDL_SCANCODE_0:               return Keyboard::Num0;
			case SDL_SCANCODE_1:               return Keyboard::Num1;
			case SDL_SCANCODE_2:               return Keyboard::Num2;
			case SDL_SCANCODE_3:               return Keyboard::Num3;
			case SDL_SCANCODE_4:               return Keyboard::Num4;
			case SDL_SCANCODE_5:               return Keyboard::Num5;
			case SDL_SCANCODE_6:               return Keyboard::Num6;
			case SDL_SCANCODE_7:               return Keyboard::Num7;
			case SDL_SCANCODE_8:               return Keyboard::Num8;
			case SDL_SCANCODE_9:               return Keyboard::Num9;
			case SDL_SCANCODE_A:               return Keyboard::A;
			case SDL_SCANCODE_B:               return Keyboard::B;
			case SDL_SCANCODE_C:               return Keyboard::C;
			case SDL_SCANCODE_D:               return Keyboard::D;
			case SDL_SCANCODE_E:               return Keyboard::E;
			case SDL_SCANCODE_F:               return Keyboard::F;
			case SDL_SCANCODE_G:               return Keyboard::G;
			case SDL_SCANCODE_H:               return Keyboard::H;
			case SDL_SCANCODE_I:               return Keyboard::I;
			case SDL_SCANCODE_J:               return Keyboard::J;
			case SDL_SCANCODE_K:               return Keyboard::K;
			case SDL_SCANCODE_L:               return Keyboard::L;
			case SDL_SCANCODE_M:               return Keyboard::M;
			case SDL_SCANCODE_N:               return Keyboard::N;
			case SDL_SCANCODE_O:               return Keyboard::O;
			case SDL_SCANCODE_P:               return Keyboard::P;
			case SDL_SCANCODE_Q:               return Keyboard::Q;
			case SDL_SCANCODE_R:               return Keyboard::R;
			case SDL_SCANCODE_S:               return Keyboard::S;
			case SDL_SCANCODE_T:               return Keyboard::T;
			case SDL_SCANCODE_U:               return Keyboard::U;
			case SDL_SCANCODE_V:               return Keyboard::V;
			case SDL_SCANCODE_W:               return Keyboard::W;
			case SDL_SCANCODE_X:               return Keyboard::X;
			case SDL_SCANCODE_Y:               return Keyboard::Y;
			case SDL_SCANCODE_Z:               return Keyboard::Z;
			case SDL_SCANCODE_KP_PLUS:         return Keyboard::Add;
			case SDL_SCANCODE_BACKSPACE:       return Keyboard::Backspace;
			case SDL_SCANCODE_AC_BACK:         return Keyboard::Browser_Back;
			case SDL_SCANCODE_AC_BOOKMARKS:    return Keyboard::Browser_Favorites;
			case SDL_SCANCODE_AC_FORWARD:      return Keyboard::Browser_Forward;
			case SDL_SCANCODE_AC_HOME:         return Keyboard::Browser_Home;
			case SDL_SCANCODE_AC_REFRESH:      return Keyboard::Browser_Refresh;
			case SDL_SCANCODE_AC_SEARCH:       return Keyboard::Browser_Search;
			case SDL_SCANCODE_AC_STOP:         return Keyboard::Browser_Stop;
			case SDL_SCANCODE_CAPSLOCK:        return Keyboard::CapsLock;
			case SDL_SCANCODE_CLEAR:             return Keyboard::Clear;
			case SDL_SCANCODE_KP_PERIOD:         return Keyboard::Decimal;
			case SDL_SCANCODE_DELETE:            return Keyboard::Delete;
			case SDL_SCANCODE_KP_DIVIDE:         return Keyboard::Divide;
			case SDL_SCANCODE_DOWN:              return Keyboard::Down;
			case SDL_SCANCODE_END:               return Keyboard::End;
			case SDL_SCANCODE_ESCAPE:            return Keyboard::Escape;
			case SDL_SCANCODE_F1:                return Keyboard::F1;
			case SDL_SCANCODE_F2:                return Keyboard::F2;
			case SDL_SCANCODE_F3:                return Keyboard::F3;
			case SDL_SCANCODE_F4:                return Keyboard::F4;
			case SDL_SCANCODE_F5:                return Keyboard::F5;
			case SDL_SCANCODE_F6:                return Keyboard::F6;
			case SDL_SCANCODE_F7:                return Keyboard::F7;
			case SDL_SCANCODE_F8:                return Keyboard::F8;
			case SDL_SCANCODE_F9:                return Keyboard::F9;
			case SDL_SCANCODE_F10:               return Keyboard::F10;
			case SDL_SCANCODE_F11:               return Keyboard::F11;
			case SDL_SCANCODE_F12:               return Keyboard::F12;
			case SDL_SCANCODE_F13:               return Keyboard::F13;
			case SDL_SCANCODE_F14:               return Keyboard::F14;
			case SDL_SCANCODE_F15:               return Keyboard::F15;
			case SDL_SCANCODE_HOME:              return Keyboard::Home;
			case SDL_SCANCODE_INSERT:            return Keyboard::Insert;
			case SDL_SCANCODE_LEFT:              return Keyboard::Left;
			case SDL_SCANCODE_LGUI:              return Keyboard::LSystem;
			case SDL_SCANCODE_AUDIONEXT:         return Keyboard::Media_Next;
			case SDL_SCANCODE_AUDIOPLAY:         return Keyboard::Media_Play;
			case SDL_SCANCODE_AUDIOPREV:         return Keyboard::Media_Previous;
			case SDL_SCANCODE_AUDIOSTOP:         return Keyboard::Media_Stop;
			case SDL_SCANCODE_KP_MULTIPLY:       return Keyboard::Multiply;
			case SDL_SCANCODE_PAGEDOWN:          return Keyboard::PageDown;
			case SDL_SCANCODE_KP_0:              return Keyboard::Numpad0;
			case SDL_SCANCODE_KP_1:              return Keyboard::Numpad1;
			case SDL_SCANCODE_KP_2:              return Keyboard::Numpad2;
			case SDL_SCANCODE_KP_3:              return Keyboard::Numpad3;
			case SDL_SCANCODE_KP_4:              return Keyboard::Numpad4;
			case SDL_SCANCODE_KP_5:              return Keyboard::Numpad5;
			case SDL_SCANCODE_KP_6:              return Keyboard::Numpad6;
			case SDL_SCANCODE_KP_7:              return Keyboard::Numpad7;
			case SDL_SCANCODE_KP_8:              return Keyboard::Numpad8;
			case SDL_SCANCODE_KP_9:              return Keyboard::Numpad9;
			case SDL_SCANCODE_NUMLOCKCLEAR:      return Keyboard::NumLock;
			case SDL_SCANCODE_SEMICOLON:         return Keyboard::Semicolon;
			case SDL_SCANCODE_SLASH:             return Keyboard::Slash;
			case SDL_SCANCODE_GRAVE:             return Keyboard::Tilde;
			case SDL_SCANCODE_APPLICATION:       return Keyboard::Menu;
			case SDL_SCANCODE_NONUSBACKSLASH:    return Keyboard::ISOBackslash102;
			case SDL_SCANCODE_LEFTBRACKET:       return Keyboard::LBracket;
			case SDL_SCANCODE_BACKSLASH:         return Keyboard::Backslash;
			case SDL_SCANCODE_RIGHTBRACKET:      return Keyboard::RBracket;
			case SDL_SCANCODE_APOSTROPHE:        return Keyboard::Quote;
			case SDL_SCANCODE_COMMA:             return Keyboard::Comma;
			case SDL_SCANCODE_MINUS:             return Keyboard::Dash;
			case SDL_SCANCODE_PERIOD:            return Keyboard::Period;
			case SDL_SCANCODE_EQUALS:            return Keyboard::Equal;
			case SDL_SCANCODE_RIGHT:             return Keyboard::Right;
			case SDL_SCANCODE_PAGEUP:            return Keyboard::PageUp;
			case SDL_SCANCODE_PAUSE:             return Keyboard::Pause;
			case SDL_SCANCODE_SYSREQ:            return Keyboard::Print;
			case SDL_SCANCODE_SCROLLLOCK:        return Keyboard::ScrollLock;
			case SDL_SCANCODE_PRINTSCREEN:       return Keyboard::PrintScreen;
			case SDL_SCANCODE_KP_MINUS:          return Keyboard::Subtract;
			case SDL_SCANCODE_RETURN:            return Keyboard::Return;
			case SDL_SCANCODE_KP_ENTER:          return Keyboard::NumpadReturn;
			case SDL_SCANCODE_RGUI:              return Keyboard::RSystem;
			case SDL_SCANCODE_SPACE:             return Keyboard::Space;
			case SDL_SCANCODE_TAB:               return Keyboard::Tab;
			case SDL_SCANCODE_UP:                return Keyboard::Up;
			case SDL_SCANCODE_VOLUMEDOWN:        return Keyboard::Volume_Down;
			case SDL_SCANCODE_MUTE:              return Keyboard::Volume_Mute;
			case SDL_SCANCODE_AUDIOMUTE:         return Keyboard::Volume_Mute;
			case SDL_SCANCODE_VOLUMEUP:          return Keyboard::Volume_Up;

			default:
				return Keyboard::Undefined;
		}
	}

	// not implemented for now, wait for mainloop friendly input
	//void WindowImpl::WindowThread(SDL_Window* handle, /*DWORD styleEx,*/ const String& title, /*DWORD style,*/ bool fullscreen, const Rectui& dimensions, WindowImpl* window, Mutex* mutex, ConditionVariable* condition)
	//{
	//	SDL_Window& winHandle = *handle;
	/*winHandle = CreateWindowExW(styleEx, className, title.GetWideString().data(), style, dimensions.x, dimensions.y, dimensions.width, dimensions.height, nullptr, nullptr, GetModuleHandle(nullptr), window);

	   if (winHandle)
	    window->PrepareWindow(fullscreen);

	   mutex->Lock();
	   condition->Signal();
	   mutex->Unlock(); // mutex and condition may be destroyed after this line

	   if (!winHandle)
	    return;

	   while (window->m_threadActive)
	    window->ProcessEvents(true);

	   DestroyWindow(winHandle);*/
	//}
}
