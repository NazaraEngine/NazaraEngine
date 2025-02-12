// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL3/WindowImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL3/CursorImpl.hpp>
#include <Nazara/Platform/SDL3/IconImpl.hpp>
#include <Nazara/Platform/SDL3/SDLHelper.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <SDL3/SDL.h>
#include <utf8cpp/utf8.h>
#include <memory>

namespace Nz
{
	namespace
	{
		Mouse::Button SDLToNazaraButton(Uint8 sdlButton)
		{
			switch (sdlButton)
			{
				case SDL_BUTTON_LEFT:   return Mouse::Left;
				case SDL_BUTTON_MIDDLE: return Mouse::Middle;
				case SDL_BUTTON_RIGHT:  return Mouse::Right;
				case SDL_BUTTON_X1:     return Mouse::XButton1;
				case SDL_BUTTON_X2:     return Mouse::XButton2;
				default:
					NazaraAssertMsg(false, "Unkown mouse button");
					return Mouse::Left;
			}
		}
	}

	WindowImpl::WindowImpl(Window* parent) :
	m_cursor(nullptr),
	m_handle(nullptr),
	m_parent(parent),
	m_eventListener(false),
	m_ignoreNextMouseMove(false),
	m_lastEditEventLength(0)
	{
		m_cursor = SDL_GetDefaultCursor();
	}

	bool WindowImpl::Create(const VideoMode& mode, const std::string& title, WindowStyleFlags style)
	{
		bool fullscreen = (style & WindowStyle::Fullscreen) != 0;

		Uint32 winStyle = 0;

		unsigned int width = mode.width;
		unsigned int height = mode.height;
		if (fullscreen)
			winStyle |= SDL_WINDOW_FULLSCREEN;

		if (!fullscreen)
		{
			if (!(style & WindowStyle::Titlebar))
				winStyle |= SDL_WINDOW_BORDERLESS;
		}

		if (style & WindowStyle::Resizable)
			winStyle |= SDL_WINDOW_RESIZABLE;

		m_ownsWindow = true;

		m_handle = SDL_CreateWindow(title.c_str(), width, height, winStyle);
		if (!m_handle)
		{
			NazaraError("failed to create window: {0}", Error::GetLastSystemError());
			return false;
		}

		m_windowId = SDL_GetWindowID(m_handle);
		SetEventListener(true);
		return true;
	}

	bool WindowImpl::Create(WindowHandle handle)
	{
		m_ownsWindow = false;

		SDL_PropertiesID properties = SDL_CreateProperties();
		switch (handle.type)
		{
			case WindowBackend::Invalid:
			{
				NazaraError("unsupported creation from an Invalid handle");
				return false;
			}

			case WindowBackend::Web:
			{
				NazaraError("unsupported creation from a Web handle");
				return false;
			}

			case WindowBackend::Cocoa:
				SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER, handle.cocoa.window);
				break;

			case WindowBackend::X11:
				SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER, Sint64(handle.x11.window));
				break;

			case WindowBackend::Wayland:
				SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER, handle.wayland.surface);
				break;

			case WindowBackend::Windows:
				SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, handle.windows.window);
				break;
		}

		NAZARA_DEFER({ SDL_DestroyProperties(properties); });

		m_handle = SDL_CreateWindowWithProperties(properties);
		if (!m_handle)
		{
			NazaraError("invalid handle");
			return false;
		}

		m_windowId = SDL_GetWindowID(m_handle);
		SetEventListener(true);
		return true;
	}

	void WindowImpl::Destroy()
	{
		SetEventListener(false);

		if (m_handle)
		{
			if (m_ownsWindow)
				SDL_DestroyWindow(m_handle);

			m_handle = nullptr;
		}
	}

	Vector2i WindowImpl::FetchPosition() const
	{
		int x, y;
		SDL_GetWindowPosition(m_handle, &x, &y);

		return { x, y };
	}

	Vector2ui WindowImpl::FetchSize() const
	{
		int width, height;
		SDL_GetWindowSize(m_handle, &width, &height);

		return { SafeCast<unsigned int>(width), SafeCast<unsigned int>(height) };
	}

	WindowStyleFlags WindowImpl::FetchStyle() const
	{
		SDL_WindowFlags windowFlags = SDL_GetWindowFlags(m_handle);

		WindowStyleFlags styleFlags;
		if (windowFlags & SDL_WINDOW_RESIZABLE)
			styleFlags |= WindowStyle::Resizable;

		if ((windowFlags & SDL_WINDOW_BORDERLESS) == 0)
			styleFlags |= WindowStyle::Titlebar | WindowStyle::Closable;

		if (windowFlags & SDL_WINDOW_FULLSCREEN)
			styleFlags |= WindowStyle::Fullscreen;

		return styleFlags;
	}

	std::string WindowImpl::FetchTitle() const
	{
		return SDL_GetWindowTitle(m_handle);
	}

	SDL_Window* WindowImpl::GetHandle() const
	{
		return m_handle;
	}

	WindowHandle WindowImpl::GetSystemHandle() const
	{
		WindowHandle handle;
		handle.type = WindowBackend::Invalid;

#if defined(NAZARA_PLATFORM_WEB)
		handle.type = WindowBackend::Web;
#elif defined(SDL_PLATFORM_WIN32)
		void* hwnd = SDL_GetPointerProperty(SDL_GetWindowProperties(m_handle), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
		if (hwnd)
		{
			handle.type = WindowBackend::Windows;
			handle.windows.window = hwnd;
		}

		return handle;
#elif defined(SDL_PLATFORM_MACOS)
		void* nswindow = SDL_GetPointerProperty(SDL_GetWindowProperties(m_handle), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
		if (nswindow)
		{
			handle.type = WindowBackend::Cocoa;
			handle.cocoa.window = nswindow;
		}
#elif defined(SDL_PLATFORM_LINUX)
		const char* videoDriver = SDL_GetCurrentVideoDriver();
		SDL_PropertiesID windowProperties = SDL_GetWindowProperties(m_handle);
		if (SDL_strcmp(videoDriver, "x11") == 0)
		{
			void* xdisplay = SDL_GetPointerProperty(windowProperties, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
			Sint64 xwindow = SDL_GetNumberProperty(windowProperties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
			if (xdisplay && xwindow)
			{
				handle.type = WindowBackend::X11;
				handle.x11.display = xdisplay;
				handle.x11.window = static_cast<unsigned long>(xwindow);
			}
		}
		else if (SDL_strcmp(videoDriver, "wayland") == 0)
		{
			void* display = SDL_GetPointerProperty(windowProperties, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
			void* surface = SDL_GetPointerProperty(windowProperties, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
			if (display && surface)
			{
				handle.type = WindowBackend::Wayland;
				handle.wayland.display = display;
				handle.wayland.surface = surface;
			}
		}
#endif

		return handle;
	}

	bool WindowImpl::HasFocus() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_INPUT_FOCUS) != 0;
	}

	void WindowImpl::IgnoreNextMouseEvent(float /*mouseX*/, float /*mouseY*/)
	{
		m_ignoreNextMouseMove = true;
	}

	bool WindowImpl::IsMinimized() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_MINIMIZED) != 0;
	}

	bool WindowImpl::IsVisible() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_HIDDEN) == 0;
	}

	void WindowImpl::RefreshCursor()
	{
		if (!m_cursor)
		{
			if (!SDL_HideCursor())
				NazaraWarning("SDL error: {}", SDL_GetError());
		}
		else
		{
			if (!SDL_ShowCursor())
				NazaraWarning("SDL error: {}", SDL_GetError());

			SDL_SetCursor(m_cursor);
		}
	}

	void WindowImpl::StartTextInput()
	{
		SDL_StartTextInput(m_handle);
	}

	void WindowImpl::StopTextInput()
	{
		SDL_StopTextInput(m_handle);
	}

	void WindowImpl::UpdateCursor(const Cursor& cursor)
	{
		m_cursor = cursor.m_impl->GetCursor();

		if (HasFocus())
			RefreshCursor();
	}

	void WindowImpl::RaiseFocus()
	{
		SDL_RaiseWindow(m_handle);
	}

	void WindowImpl::UpdateIcon(const Icon& icon)
	{
		SDL_SetWindowIcon(m_handle, icon.m_impl->GetIcon());
	}

	void WindowImpl::UpdateMaximumSize(int width, int height)
	{
		SDL_SetWindowMaximumSize(m_handle, width, height);
	}

	void WindowImpl::UpdateMinimumSize(int width, int height)
	{
		SDL_SetWindowMinimumSize(m_handle, width, height);
	}

	void WindowImpl::UpdatePosition(int x, int y)
	{
		SDL_SetWindowPosition(m_handle, x, y);
	}

	void WindowImpl::UpdateRelativeMouseMode(bool relativeMouseMode)
	{
		SDL_SetWindowRelativeMouseMode(m_handle, relativeMouseMode);
	}

	void WindowImpl::UpdateSize(unsigned int width, unsigned int height)
	{
		SDL_SetWindowSize(m_handle, width, height);
	}

	void WindowImpl::UpdateStayOnTop(bool stayOnTop)
	{
		SDL_SetWindowAlwaysOnTop(m_handle, stayOnTop);
	}

	void WindowImpl::UpdateTitle(const std::string& title)
	{
		SDL_SetWindowTitle(m_handle, title.c_str());
	}

	void WindowImpl::Show(bool visible)
	{
		if (visible)
			SDL_ShowWindow(m_handle);
		else
			SDL_HideWindow(m_handle);
	}

	void WindowImpl::ProcessEvents()
	{
		SDL_PumpEvents();
	}

	bool WindowImpl::Initialize()
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			NazaraError("SDL error: {}", SDL_GetError());
			return false;
		}

		return true;
	}

	void WindowImpl::Uninitialize()
	{
		SDL_Quit();
	}

	void WindowImpl::SetEventListener(bool listener)
	{
		if (m_eventListener == listener)
			return;

		if (listener)
			SDL_AddEventWatch(HandleEvent, this);
		else
			SDL_RemoveEventWatch(HandleEvent, this);

		m_eventListener = listener;
	}

	bool WindowImpl::HandleEvent(void* userdata, SDL_Event* event)
	{
		try
		{
			WindowImpl* window = static_cast<WindowImpl*>(userdata);

			switch (event->type)
			{
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Quit;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_MOVED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Moved;
					windowEvent.position.x = static_cast<unsigned int>(std::max(0, event->window.data1));
					windowEvent.position.y = static_cast<unsigned int>(std::max(0, event->window.data2));

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_RESIZED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Resized;
					windowEvent.size.width = static_cast<unsigned int>(std::max(0, event->window.data1));
					windowEvent.size.height = static_cast<unsigned int>(std::max(0, event->window.data2));

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::GainedFocus;
					window->RefreshCursor();

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::LostFocus;
					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseEntered;
					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseLeft;
					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_MINIMIZED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Minimized;
					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_WINDOW_RESTORED:
				{
					if (window->m_windowId != event->window.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Restored;
					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_MOUSE_MOTION:
				{
					if (window->m_windowId != event->motion.windowID)
						return false;

					if (window->m_ignoreNextMouseMove)
					{
						window->m_ignoreNextMouseMove = false;
						return false;
					}

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseMoved;
					windowEvent.mouseMove.x = event->motion.x;
					windowEvent.mouseMove.y = event->motion.y;
					windowEvent.mouseMove.deltaX = event->motion.xrel;
					windowEvent.mouseMove.deltaY = event->motion.yrel;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					if (window->m_windowId != event->button.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? WindowEventType::MouseButtonPressed : WindowEventType::MouseButtonReleased;
					windowEvent.mouseButton.button = SDLToNazaraButton(event->button.button);
					windowEvent.mouseButton.x = event->button.x;
					windowEvent.mouseButton.y = event->button.y;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_MOUSE_WHEEL:
				{
					if (window->m_windowId != event->wheel.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseWheelMoved;
					windowEvent.mouseWheel.delta = event->wheel.y;
					windowEvent.mouseWheel.x = event->wheel.mouse_x;
					windowEvent.mouseWheel.y = event->wheel.mouse_y;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
				{
					if (window->m_windowId != event->key.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = (event->type == SDL_EVENT_KEY_DOWN) ? WindowEventType::KeyPressed : WindowEventType::KeyReleased;
					windowEvent.key.alt = (event->key.mod & SDL_KMOD_ALT) != 0;
					windowEvent.key.control = (event->key.mod & SDL_KMOD_CTRL) != 0;
					windowEvent.key.repeated = event->key.repeat;
					windowEvent.key.scancode = FromSDL(event->key.scancode);
					windowEvent.key.shift = (event->key.mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) != 0;
					windowEvent.key.system = (event->key.mod & (SDL_KMOD_LGUI | SDL_KMOD_RGUI)) != 0;
					windowEvent.key.virtualKey = FromSDL(event->key.key);

					window->m_parent->HandleEvent(windowEvent);

					if (event->type == SDL_EVENT_KEY_DOWN)
					{
						// implements X11/Win32 APIs behavior for Enter and Backspace
						switch (windowEvent.key.virtualKey)
						{
							case Nz::Keyboard::VKey::NumpadReturn:
							case Nz::Keyboard::VKey::Return:
							{
								if (window->m_lastEditEventLength != 0)
									break;

								windowEvent.type = WindowEventType::TextEntered;
								windowEvent.text.character = U'\n';
								windowEvent.text.repeated = event->key.repeat;

								window->m_parent->HandleEvent(windowEvent);
								break;
							}

							case Nz::Keyboard::VKey::Backspace:
								windowEvent.type = WindowEventType::TextEntered;
								windowEvent.text.character = U'\b';
								windowEvent.text.repeated = event->key.repeat;

								window->m_parent->HandleEvent(windowEvent);
								break;

							default:
								break;
						}
					}
					break;
				}

				case SDL_EVENT_TEXT_INPUT:
				{
					if (window->m_windowId != event->text.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::TextEntered;
					windowEvent.text.repeated = false;

					utf8::unchecked::iterator<const char*> it(event->text.text);
					do
					{
						windowEvent.text.character = *it;

						window->m_parent->HandleEvent(windowEvent);
					}
					while (*it++);

					break;
				}

				case SDL_EVENT_TEXT_EDITING:
				{
					if (window->m_windowId != event->edit.windowID)
						return false;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::TextEdited;
					windowEvent.edit.length = event->edit.length;
					window->m_lastEditEventLength = windowEvent.edit.length;

					for (std::size_t i = 0; i < 32; i++)
					{
						windowEvent.edit.text[i] = event->edit.text[i];
					}

					window->m_parent->HandleEvent(windowEvent);
					break;
				}
			}
		}
		catch (const std::exception& e)
		{
			NazaraError("{}", e.what());
		}
		catch (...) // Don't let any exceptions go through C calls
		{
			NazaraError("an unknown error happened");
		}

		return false;
	}
}
