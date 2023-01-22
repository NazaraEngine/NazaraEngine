// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Window.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	Window::Window() :
	m_impl(nullptr),
	m_closeOnQuit(true),
	m_waitForEvent(false)
	{
		ConnectSlots();
	}

	Window::Window(Window&& window) noexcept :
	m_cursorController(std::move(window.m_cursorController)),
	m_cursor(std::move(window.m_cursor)),
	m_eventHandler(std::move(window.m_eventHandler)),
	m_icon(std::move(window.m_icon)),
	m_closed(window.m_closed),
	m_closeOnQuit(window.m_closeOnQuit),
	m_ownsWindow(window.m_ownsWindow),
	m_waitForEvent(window.m_waitForEvent)
	{
		window.DisconnectSlots();
		ConnectSlots();
	}

	Window::~Window()
	{
		Destroy();
	}

	bool Window::Create(VideoMode mode, const std::string& title, WindowStyleFlags style)
	{
		// If the window is already open, we keep its position
		bool opened = IsOpen();
		Vector2i position;
		if (opened)
			position = m_position;

		Destroy();

		if (style & WindowStyle::Closable || style & WindowStyle::Resizable)
			style |= WindowStyle::Titlebar;

		m_impl = std::make_unique<WindowImpl>(this);
		if (!m_impl->Create(mode, title, style))
		{
			NazaraError("Failed to create window implementation");
			return false;
		}

		CallOnExit destroyOnFailure([this] () { Destroy(); });

		m_closed = false;
		m_ownsWindow = true;

		m_position = m_impl->FetchPosition();
		m_size = m_impl->FetchSize();

		// Default parameters
		m_impl->UpdateMaximumSize(-1, -1);
		m_impl->UpdateMinimumSize(-1, -1);
		m_impl->Show(true);

		if (opened)
			m_impl->UpdatePosition(position.x, position.y);

		destroyOnFailure.Reset();

		m_eventHandler.Dispatch({ WindowEventType::Created });

		return true;
	}

	bool Window::Create(WindowHandle handle)
	{
		Destroy();

		m_impl = std::make_unique<WindowImpl>(this);
		if (!m_impl->Create(handle))
		{
			NazaraError("Unable to create window implementation");
			m_impl.reset();
			return false;
		}

		m_position = m_impl->FetchPosition();
		m_size = m_impl->FetchSize();

		m_closed = false;
		m_ownsWindow = false;

		m_eventHandler.Dispatch({ WindowEventType::Created });

		return true;
	}

	void Window::Destroy()
	{
		if (m_impl)
		{
			m_eventHandler.Dispatch({ WindowEventType::Destruction });

			m_impl->Destroy();
			m_impl.reset();
		}

		m_cursor.reset();
	}

	Vector2i Window::GetPosition() const
	{
		NazaraAssert(m_impl, "Window not created");
		return m_position;
	}

	Vector2ui Window::GetSize() const
	{
		NazaraAssert(m_impl, "Window not created");
		return m_size;
	}

	WindowStyleFlags Window::GetStyle() const
	{
		NazaraAssert(m_impl, "Window not created");
		return m_impl->FetchStyle();
	}

	std::string Window::GetTitle() const
	{
		NazaraAssert(m_impl, "Window not created");
		return m_impl->FetchTitle();
	}

	void Window::HandleEvent(const WindowEvent& event)
	{
		m_eventHandler.Dispatch(event);

		switch (event.type)
		{
			case WindowEventType::Moved:
			{
				m_position = { event.position.x, event.position.y };
				break;
			}

			case WindowEventType::Quit:
			{
				if (m_closeOnQuit)
					Close();

				break;
			}

			case WindowEventType::Resized:
			{
				m_size = { event.size.width, event.size.height };
				break;
			}

			default:
				break;
		}
	}

	bool Window::HasFocus() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		return m_impl->HasFocus();
	}

	bool Window::IsMinimized() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		return m_impl->IsMinimized();
	}

	bool Window::IsVisible() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		return m_impl->IsVisible();
	}

	void Window::SetCursor(std::shared_ptr<Cursor> cursor)
	{
		NazaraAssert(m_impl, "Window not created");
		NazaraAssert(cursor && cursor->IsValid(), "Invalid cursor");

		m_cursor = std::move(cursor);
		m_impl->UpdateCursor(*m_cursor);
	}

	void Window::SetFocus()
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->RaiseFocus();
	}

	void Window::SetIcon(std::shared_ptr<Icon> icon)
	{
		NazaraAssert(m_impl, "Window not created");
		NazaraAssert(icon, "Invalid icon");

		m_icon = std::move(icon);
		m_impl->UpdateIcon(*m_icon);
	}

	void Window::SetMaximumSize(const Vector2i& maxSize)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateMaximumSize(maxSize.x, maxSize.y);
	}

	void Window::SetMaximumSize(int width, int height)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateMaximumSize(width, height);
	}

	void Window::SetMinimumSize(const Vector2i& minSize)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateMinimumSize(minSize.x, minSize.y);
	}

	void Window::SetMinimumSize(int width, int height)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateMinimumSize(width, height);
	}

	void Window::SetPosition(const Vector2i& position)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdatePosition(position.x, position.y);
	}

	void Window::SetPosition(int x, int y)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdatePosition(x, y);
	}

	void Window::SetSize(const Vector2i& size)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateSize(size.x, size.y);
	}

	void Window::SetSize(unsigned int width, unsigned int height)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateSize(width, height);
	}

	void Window::SetStayOnTop(bool stayOnTop)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateStayOnTop(stayOnTop);
	}

	void Window::SetTitle(const std::string& title)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->UpdateTitle(title);
	}

	void Window::SetVisible(bool visible)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->Show(visible);
	}

	Window& Window::operator=(Window&& window) noexcept
	{
		m_cursorController = std::move(window.m_cursorController);
		m_cursor = std::move(window.m_cursor);
		m_eventHandler = std::move(window.m_eventHandler);
		m_icon = std::move(window.m_icon);
		m_closed = window.m_closed;
		m_closeOnQuit = window.m_closeOnQuit;
		m_ownsWindow = window.m_ownsWindow;
		m_waitForEvent = window.m_waitForEvent;

		window.DisconnectSlots();
		ConnectSlots();

		return *this;
	}

	void Window::ProcessEvents()
	{
		WindowImpl::ProcessEvents();
	}

	WindowHandle Window::GetHandle() const
	{
		return (m_impl) ? m_impl->GetSystemHandle() : WindowHandle{};
	}

	void Window::ConnectSlots()
	{
		m_cursorUpdateSlot.Connect(m_cursorController.OnCursorUpdated, [this](const CursorController*, const std::shared_ptr<Cursor>& cursor)
		{
			if (IsValid())
				SetCursor(cursor);
		});
	}

	void Window::DisconnectSlots()
	{
		m_cursorUpdateSlot.Disconnect();
	}

	void Window::IgnoreNextMouseEvent(int mouseX, int mouseY) const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->IgnoreNextMouseEvent(mouseX, mouseY);
	}

	bool Window::Initialize()
	{
		return WindowImpl::Initialize();
	}

	void Window::Uninitialize()
	{
		WindowImpl::Uninitialize();
	}
}
