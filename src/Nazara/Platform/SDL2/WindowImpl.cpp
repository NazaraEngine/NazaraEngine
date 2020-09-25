// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <cstdio>
#include <memory>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Platform/SDL2/SDLHelper.hpp>
#include <Nazara/Utility/Image.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

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

	bool WindowImpl::Create(const VideoMode& mode, const std::string& title, WindowStyleFlags style)
	{
		bool async = (style & WindowStyle_Threaded) != 0;
		if (async)
		{
			NazaraError("SDL2 backend doesn't support asyn window for now");

			return false;
		}


		bool fullscreen = (style & WindowStyle_Fullscreen) != 0;

		Uint32 winStyle = 0;

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

		m_handle = SDL_CreateWindow(title.c_str(), x, y, width, height, winStyle);

		if (!m_handle)
		{
			NazaraError("Failed to create window: " + Error::GetLastSystemError());
			return false;
		}

		PrepareWindow(fullscreen);

		SDL_AddEventWatch(HandleEvent, this);

		return true;
	}

	bool WindowImpl::Create(void* handle)
	{
		m_handle = SDL_CreateWindowFrom(handle);
		if (!m_handle)
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

		return true;
	}

	void WindowImpl::Destroy()
	{
		if (m_ownsWindow && m_handle)
		{
			SDL_DelEventWatch(HandleEvent, this);
			SDL_DestroyWindow(m_handle);
		}
		else
			SetEventListener(false);
	}

	void WindowImpl::EnableKeyRepeat(bool enable)
	{
		m_keyRepeat = enable;
	}

	void WindowImpl::EnableSmoothScrolling(bool enable)
	{
		m_smoothScrolling = enable;
	}

	SDL_Window* WindowImpl::GetHandle() const
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
	
	WindowHandle WindowImpl::GetSystemHandle() const
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);

		if (SDL_GetWindowWMInfo(m_handle, &wmInfo) != SDL_TRUE)
		{
			ErrorFlags flags(ErrorFlag_ThrowException, true);
			NazaraError(std::string("failed to retrieve window manager info: ") + SDL_GetError());
		}

		WindowHandle handle;

		switch (wmInfo.subsystem)
		{
#if defined(SDL_VIDEO_DRIVER_X11)
			case SDL_SYSWM_X11:
			{
				handle.type = WindowManager::X11;
				handle.x11.display = wmInfo.info.x11.display;
				handle.x11.window = wmInfo.info.x11.window;
				break;
			}
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
			case SDL_SYSWM_WAYLAND:
			{
				handle.type = WindowManager::Wayland;
				handle.wayland.display = wmInfo.info.wl.display;
				handle.wayland.surface = wmInfo.info.wl.surface;
				handle.wayland.shellSurface = wmInfo.info.wl.shell_surface;
				break;
			}
#endif
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
			case SDL_SYSWM_WINDOWS:
			{
				handle.type = WindowManager::Windows;
				handle.windows.window = wmInfo.info.win.window;
				break;
			}
#endif
			default:
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);
				NazaraError("unhandled window subsystem");
			}
		}

		return handle;
	}

	std::string WindowImpl::GetTitle() const
	{
		return SDL_GetWindowTitle(m_handle);
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
		if (m_ownsWindow)
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

							evt.size.width = static_cast<unsigned int>(std::max(0, event->window.data1));
							evt.size.height = static_cast<unsigned int>(std::max(0, event->window.data2));

							window->m_size.Set(evt.size.width, evt.size.height);

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

					if (window->m_ignoreNextMouseMove /*&& event->motion.x == window->m_mousePos.x && event->motion.y == window->m_mousePos.y*/)
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

					evt.key.scancode = SDLHelper::FromSDL(event->key.keysym.scancode);
					evt.key.virtualKey = SDLHelper::FromSDL(event->key.keysym.sym);
					evt.key.alt = (event->key.keysym.mod & KMOD_ALT) != 0;
					evt.key.control = (event->key.keysym.mod & KMOD_CTRL) != 0;
					evt.key.repeated = event->key.repeat != 0;
					evt.key.shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
					evt.key.system = (event->key.keysym.mod & KMOD_GUI) != 0;

					// implements X11/Win32 APIs behavior for Enter and Backspace
					switch (evt.key.virtualKey) {
						case Nz::Keyboard::VKey::NumpadReturn:
						case Nz::Keyboard::VKey::Return:
							if (window->m_lastEditEventLength != 0)
								break;
							window->m_parent->PushEvent(evt);

							evt.type = WindowEventType_TextEntered;

							evt.text.character = U'\n';
							evt.text.repeated = event->key.repeat != 0;

							window->m_parent->PushEvent(evt);

							break;
						case Nz::Keyboard::VKey::Backspace:
							window->m_parent->PushEvent(evt);

							evt.type = WindowEventType_TextEntered;

							evt.text.character = U'\b';
							evt.text.repeated = event->key.repeat != 0;

							window->m_parent->PushEvent(evt);

							break;
						default:
							break;
					}

					break;

				case SDL_KEYUP:
					if (SDL_GetWindowID(window->m_handle) != event->key.windowID)
						return 0;

					evt.type = WindowEventType_KeyReleased;

					evt.key.scancode = SDLHelper::FromSDL(event->key.keysym.scancode);
					evt.key.virtualKey = SDLHelper::FromSDL(event->key.keysym.sym);
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
					evt.text.repeated = false;

					for (decltype(evt.text.character) codepoint : ToUtf32String(event->text.text))
					{
						evt.text.character = codepoint;

						window->m_parent->PushEvent(evt);
					}

					// prevent post switch event
					evt.type = WindowEventType::WindowEventType_Max;

					break;

				case SDL_TEXTEDITING:
					if (SDL_GetWindowID(window->m_handle) != event->edit.windowID)
						return 0;

					evt.type = WindowEventType_TextEdited;
					evt.edit.length = event->edit.length;
					window->m_lastEditEventLength = evt.edit.length;

					for (std::size_t i = 0; i < 32; i++)
					{
						evt.edit.text[i] = event->edit.text[i];
					}

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
		if (m_ownsWindow)
			return;

		if (listener)
			SDL_AddEventWatch(HandleEvent, this);
		else
			SDL_DelEventWatch(HandleEvent, this);
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

	void WindowImpl::SetTitle(const std::string& title)
	{
		SDL_SetWindowTitle(m_handle, title.c_str());
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

		return true;
	}

	void WindowImpl::Uninitialize()
	{
		SDL_Quit();
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
