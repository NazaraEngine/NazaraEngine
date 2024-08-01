// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Export.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Platform/SDL2/SDLHelper.hpp>
#include <SDL.h>

#if defined(NAZARA_PLATFORM_MACOS)
// I'm not sure why, but SDL_VIDEO_DRIVER_X11 is automatically defined here by SDL_config.h
// This is problematic as it requires X11/X.h which is not present (adding libxext/libx11/xorgproto packages didn't help)
#undef SDL_VIDEO_DRIVER_X11
#endif

#include <SDL_syswm.h>
#include <utf8cpp/utf8.h>
#include <cstdio>
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
					NazaraAssert(false, "Unkown mouse button");
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

		unsigned int x, y;
		unsigned int width = mode.width;
		unsigned int height = mode.height;
		if (fullscreen)
			winStyle |= SDL_WINDOW_FULLSCREEN;

		if (fullscreen)
		{
			x = 0;
			y = 0;
		}
		else
		{
			if (!(style & WindowStyle::Titlebar))
				winStyle |= SDL_WINDOW_BORDERLESS;

			x = SDL_WINDOWPOS_CENTERED;
			y = SDL_WINDOWPOS_CENTERED;
		}

		if (style & WindowStyle::Resizable)
			winStyle |= SDL_WINDOW_RESIZABLE;

		m_ownsWindow = true;

		m_handle = SDL_CreateWindow(title.c_str(), x, y, width, height, winStyle);
		if (!m_handle)
		{
			NazaraErrorFmt("failed to create window: {0}", Error::GetLastSystemError());
			return false;
		}

		m_windowId = SDL_GetWindowID(m_handle);
		SetEventListener(true);
		return true;
	}

	bool WindowImpl::Create(WindowHandle handle)
	{
		void* systemHandle = nullptr;
		switch (handle.type)
		{
			case WindowBackend::Invalid:
			{
				NazaraError("unsupported creation from a Wayland handle");
				return false;
			}

			case WindowBackend::Wayland:
			{
				NazaraError("unsupported creation from a Wayland handle");
				return false;
			}

			case WindowBackend::Web:
			{
				NazaraError("unsupported creation from a Web handle");
				return false;
			}

			case WindowBackend::Cocoa:   systemHandle = handle.cocoa.window;       break;
			case WindowBackend::X11:     systemHandle = IntegerToPointer<void*>(handle.x11.window); break;
			case WindowBackend::Windows: systemHandle = handle.windows.window;     break;
		}

		m_ownsWindow = false;

		m_handle = SDL_CreateWindowFrom(systemHandle);
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
		UInt32 windowFlags = SDL_GetWindowFlags(m_handle);

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
#ifdef NAZARA_PLATFORM_WEB
		WindowHandle handle;
		handle.type = WindowBackend::Web;

		return handle;
#else
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);

		if (SDL_GetWindowWMInfo(m_handle, &wmInfo) != SDL_TRUE)
		{
#ifndef NAZARA_PLATFORM_WEB
			ErrorFlags flags(ErrorMode::ThrowException);
			NazaraErrorFmt("failed to retrieve window manager info: {0}", SDL_GetError());
#endif
		}

		WindowHandle handle;

		switch (wmInfo.subsystem)
		{
#if defined(SDL_VIDEO_DRIVER_COCOA)
			case SDL_SYSWM_COCOA:
			{
				handle.type = WindowBackend::Cocoa;
				handle.cocoa.window = wmInfo.info.cocoa.window;
				break;
			}
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
			case SDL_SYSWM_X11:
			{
				handle.type = WindowBackend::X11;
				handle.x11.display = wmInfo.info.x11.display;
				handle.x11.window = wmInfo.info.x11.window;
				break;
			}
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
			case SDL_SYSWM_WAYLAND:
			{
				handle.type = WindowBackend::Wayland;
				handle.wayland.display = wmInfo.info.wl.display;
				handle.wayland.surface = wmInfo.info.wl.surface;
				handle.wayland.shellSurface = wmInfo.info.wl.shell_surface;
				break;
			}
#endif
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
			case SDL_SYSWM_WINDOWS:
			{
				handle.type = WindowBackend::Windows;
				handle.windows.window = wmInfo.info.win.window;
				break;
			}
#endif
			default:
			{
#if defined(NAZARA_PLATFORM_WEB)
				handle.type = WindowBackend::Web;
#else
				ErrorFlags flags(ErrorMode::ThrowException);
				NazaraError("unhandled window subsystem");
#endif
			}
		}

		return handle;
#endif
	}

	bool WindowImpl::HasFocus() const
	{
		return (SDL_GetWindowFlags(m_handle) & SDL_WINDOW_INPUT_FOCUS) != 0;
	}

	void WindowImpl::IgnoreNextMouseEvent(int /*mouseX*/, int /*mouseY*/)
	{
		m_ignoreNextMouseMove = true;
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

	int WindowImpl::HandleEvent(void* userdata, SDL_Event* event)
	{
		try
		{
			WindowImpl* window = static_cast<WindowImpl*>(userdata);

			switch (event->type)
			{
				case SDL_WINDOWEVENT:
				{
					if (window->m_windowId != event->window.windowID)
						return 0;

					WindowEvent windowEvent;
					switch (event->window.event)
					{
						case SDL_WINDOWEVENT_CLOSE:
							windowEvent.type = WindowEventType::Quit;
							break;

						case SDL_WINDOWEVENT_SIZE_CHANGED:
							windowEvent.type = WindowEventType::Resized;
							windowEvent.size.width = static_cast<unsigned int>(std::max(0, event->window.data1));
							windowEvent.size.height = static_cast<unsigned int>(std::max(0, event->window.data2));
							break;

						case SDL_WINDOWEVENT_MOVED:
							windowEvent.type = WindowEventType::Moved;
							windowEvent.position.x = event->window.data1;
							windowEvent.position.y = event->window.data2;
							break;

						case SDL_WINDOWEVENT_FOCUS_GAINED:
							windowEvent.type = WindowEventType::GainedFocus;
							window->RefreshCursor();
							break;

						case SDL_WINDOWEVENT_FOCUS_LOST:
							windowEvent.type = WindowEventType::LostFocus;
							break;

						case SDL_WINDOWEVENT_ENTER:
							windowEvent.type = WindowEventType::MouseEntered;
							window->RefreshCursor();
							break;

						case SDL_WINDOWEVENT_LEAVE:
							windowEvent.type = WindowEventType::MouseLeft;
							break;

						case SDL_WINDOWEVENT_MINIMIZED:
							windowEvent.type = WindowEventType::Minimized;
							break;

						case SDL_WINDOWEVENT_RESTORED:
							windowEvent.type = WindowEventType::Restored;
							break;

						default:
							return 0;
					}

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_MOUSEMOTION:
				{
					if (window->m_windowId != event->motion.windowID)
						return 0;

					if (window->m_ignoreNextMouseMove)
					{
						window->m_ignoreNextMouseMove = false;
						return 0;
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

				case SDL_MOUSEBUTTONDOWN:
				{
					if (window->m_windowId != event->button.windowID)
						return 0;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseButtonPressed;
					windowEvent.mouseButton.button = SDLToNazaraButton(event->button.button);
					windowEvent.mouseButton.x = event->button.x;
					windowEvent.mouseButton.y = event->button.y;
					windowEvent.mouseButton.clickCount = event->button.clicks;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					if (window->m_windowId != event->button.windowID)
						return 0;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseButtonReleased;
					windowEvent.mouseButton.button = SDLToNazaraButton(event->button.button);
					windowEvent.mouseButton.x = event->button.x;
					windowEvent.mouseButton.y = event->button.y;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_MOUSEWHEEL:
				{
					if (window->m_windowId != event->wheel.windowID)
						return 0;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseWheelMoved;
					windowEvent.mouseWheel.delta = event->wheel.preciseY;
					windowEvent.mouseWheel.x = event->wheel.mouseX;
					windowEvent.mouseWheel.y = event->wheel.mouseY;

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_KEYDOWN:
				{
					if (window->m_windowId != event->key.windowID)
						return 0;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::KeyPressed;
					windowEvent.key.alt = (event->key.keysym.mod & KMOD_ALT) != 0;
					windowEvent.key.control = (event->key.keysym.mod & KMOD_CTRL) != 0;
					windowEvent.key.repeated = event->key.repeat != 0;
					windowEvent.key.scancode = SDLHelper::FromSDL(event->key.keysym.scancode);
					windowEvent.key.shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
					windowEvent.key.system = (event->key.keysym.mod & KMOD_GUI) != 0;
					windowEvent.key.virtualKey = SDLHelper::FromSDL(event->key.keysym.sym);

					window->m_parent->HandleEvent(windowEvent);

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
							windowEvent.text.repeated = event->key.repeat != 0;

							window->m_parent->HandleEvent(windowEvent);
							break;
						}

						case Nz::Keyboard::VKey::Backspace:
							windowEvent.type = WindowEventType::TextEntered;
							windowEvent.text.character = U'\b';
							windowEvent.text.repeated = event->key.repeat != 0;

							window->m_parent->HandleEvent(windowEvent);
							break;

						default:
							break;
					}

					break;
				}

				case SDL_KEYUP:
				{
					if (window->m_windowId != event->key.windowID)
						return 0;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::KeyReleased;
					windowEvent.key.alt = (event->key.keysym.mod & KMOD_ALT) != 0;
					windowEvent.key.control = (event->key.keysym.mod & KMOD_CTRL) != 0;
					windowEvent.key.repeated = event->key.repeat != 0;
					windowEvent.key.scancode = SDLHelper::FromSDL(event->key.keysym.scancode);
					windowEvent.key.shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
					windowEvent.key.system = (event->key.keysym.mod & KMOD_GUI) != 0;
					windowEvent.key.virtualKey = SDLHelper::FromSDL(event->key.keysym.sym);

					window->m_parent->HandleEvent(windowEvent);
					break;
				}

				case SDL_TEXTINPUT:
				{
					if (window->m_windowId != event->text.windowID)
						return 0;

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

				case SDL_TEXTEDITING:
				{
					if (window->m_windowId != event->edit.windowID)
						return 0;

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
			NazaraError(e.what());
		}
		catch (...) // Don't let any exceptions go through C calls
		{
			NazaraError("an unknown error happened");
		}

		return 0;
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

	void WindowImpl::UpdateSize(unsigned int width, unsigned int height)
	{
		SDL_SetWindowSize(m_handle, width, height);
	}

	void WindowImpl::UpdateStayOnTop(bool stayOnTop)
	{
		SDL_SetWindowAlwaysOnTop(m_handle, (stayOnTop) ? SDL_TRUE : SDL_FALSE);
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

	void WindowImpl::SetEventListener(bool listener)
	{
		if (m_eventListener == listener)
			return;

		if (listener)
			SDL_AddEventWatch(HandleEvent, this);
		else
			SDL_DelEventWatch(HandleEvent, this);

		m_eventListener = listener;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_FREEBSD)
#include <Nazara/Core/AntiX11.hpp>
#endif
