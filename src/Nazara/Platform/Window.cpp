// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Window.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL3/WindowImpl.hpp>
#include <NazaraUtils/CallOnExit.hpp>

namespace Nz
{
	Window::Window() :
	m_closeOnQuit(true),
	m_waitForEvent(false)
	{
		ConnectSlots();
	}

	Window::Window(Window&& window) noexcept :
	m_cursor(std::move(window.m_cursor)),
	m_icon(std::move(window.m_icon)),
	m_impl(std::move(window.m_impl)),
	m_cursorController(std::move(window.m_cursorController)),
	m_eventHandler(std::move(window.m_eventHandler)),
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
NAZARA_WARNING_PUSH()
NAZARA_WARNING_MSVC_DISABLE(4701) //< uninitialized variable maybe used (position)

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
			NazaraError("failed to create window implementation");
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

		m_eventHandler.Dispatch({ { WindowEventType::Created } });

NAZARA_WARNING_POP()

		return true;
	}

	bool Window::Create(WindowHandle handle)
	{
		Destroy();

		m_impl = std::make_unique<WindowImpl>(this);
		if (!m_impl->Create(handle))
		{
			NazaraError("unable to create window implementation");
			m_impl.reset();
			return false;
		}

		m_position = m_impl->FetchPosition();
		m_size = m_impl->FetchSize();

		m_closed = false;
		m_ownsWindow = false;

		m_eventHandler.Dispatch({ { WindowEventType::Created } });

		return true;
	}

	void Window::Destroy()
	{
		if (m_impl)
		{
			m_eventHandler.Dispatch({ { WindowEventType::Destruction } });

			m_impl->Destroy();
			m_impl.reset();
		}

		m_cursor.reset();
	}

	WindowStyleFlags Window::GetStyle() const
	{
		NazaraAssertMsg(m_impl, "Window not created");
		return m_impl->FetchStyle();
	}

	std::string Window::GetTitle() const
	{
		NazaraAssertMsg(m_impl, "Window not created");
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
		NazaraAssertMsg(m_impl, "window not created");
		return m_impl->HasFocus();
	}

	bool Window::IsMinimized() const
	{
		NazaraAssertMsg(m_impl, "window not created");
		return m_impl->IsMinimized();
	}

	bool Window::IsVisible() const
	{
		NazaraAssertMsg(m_impl, "window not created");
		return m_impl->IsVisible();
	}

	void Window::SetCursor(std::shared_ptr<Cursor> cursor)
	{
		NazaraAssertMsg(m_impl, "window not created");
		NazaraAssertMsg(cursor && cursor->IsValid(), "Invalid cursor");

		m_cursor = std::move(cursor);
		m_impl->UpdateCursor(*m_cursor);
	}

	void Window::SetFocus()
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->RaiseFocus();
	}

	void Window::SetIcon(std::shared_ptr<Icon> icon)
	{
		NazaraAssertMsg(m_impl, "Window not created");
		NazaraAssertMsg(icon, "Invalid icon");

		m_icon = std::move(icon);
		m_impl->UpdateIcon(*m_icon);
	}

	void Window::SetMaximumSize(const Vector2i& maxSize)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateMaximumSize(maxSize.x, maxSize.y);
	}

	void Window::SetMinimumSize(const Vector2i& minSize)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateMinimumSize(minSize.x, minSize.y);
	}

	void Window::SetPosition(const Vector2i& position)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdatePosition(position.x, position.y);
	}

	void Window::SetRelativeMouseMode(bool relativeMouseMode)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateRelativeMouseMode(relativeMouseMode);
	}

	void Window::SetSize(const Vector2i& size)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateSize(size.x, size.y);
	}

	void Window::SetStayOnTop(bool stayOnTop)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateStayOnTop(stayOnTop);
	}

	void Window::SetTitle(const std::string& title)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->UpdateTitle(title);
	}

	void Window::SetVisible(bool visible)
	{
		NazaraAssertMsg(m_impl, "window not created");

		m_impl->Show(visible);
	}

	void Window::StartTextInput()
	{
		m_impl->StartTextInput();
	}

	void Window::StopTextInput()
	{
		m_impl->StopTextInput();
	}

	Window& Window::operator=(Window&& window) noexcept
	{
		m_cursorController = std::move(window.m_cursorController);
		m_cursor = std::move(window.m_cursor);
		m_eventHandler = std::move(window.m_eventHandler);
		m_icon = std::move(window.m_icon);
		m_impl = std::move(window.m_impl);
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

		m_textInputStarted.Connect(m_textInputController.OnTextInputStarted, [this](const TextInputController*)
		{
			if (IsValid())
				StartTextInput();
		});

		m_textInputStopped.Connect(m_textInputController.OnTextInputStopped, [this](const TextInputController*)
		{
			if (IsValid())
				StopTextInput();
		});
	}

	void Window::DisconnectSlots()
	{
		m_cursorUpdateSlot.Disconnect();
		m_textInputStarted.Disconnect();
		m_textInputStopped.Disconnect();
	}

	void Window::IgnoreNextMouseEvent(float mouseX, float mouseY) const
	{
		NazaraAssertMsg(m_impl, "window not created");

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
