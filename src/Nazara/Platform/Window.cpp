// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Window.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		Window* s_fullscreenWindow = nullptr;
	}

	Window::Window() :
	m_impl(nullptr),
	m_asyncWindow(false),
	m_closeOnQuit(true),
	m_eventPolling(false),
	m_waitForEvent(false)
	{
		ConnectSlots();
	}

	Window::Window(Window&& window) :
	m_events(std::move(window.m_events)),
	m_pendingEvents(std::move(window.m_pendingEvents)),
	m_cursorController(std::move(window.m_cursorController)),
	m_cursor(std::move(window.m_cursor)),
	m_eventHandler(std::move(window.m_eventHandler)),
	m_icon(std::move(window.m_icon)),
	m_asyncWindow(window.m_asyncWindow),
	m_closed(window.m_asyncWindow),
	m_closeOnQuit(window.m_closeOnQuit),
	m_eventPolling(window.m_eventPolling),
	m_ownsWindow(window.m_asyncWindow),
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
		NAZARA_USE_ANONYMOUS_NAMESPACE

		// If the window is already open, we keep its position
		bool opened = IsOpen();
		Vector2i position;
		if (opened)
			position = m_impl->GetPosition();

		Destroy();

		// Inspired by the code of the SFML by Laurent Gomila (and its team)
		if (style & WindowStyle::Fullscreen)
		{
			if (s_fullscreenWindow)
			{
				NazaraError("Window " + PointerToString(s_fullscreenWindow) + " already in fullscreen mode");
				style &= ~WindowStyle::Fullscreen;
			}
			else
			{
				if (!mode.IsFullscreenValid())
				{
					NazaraWarning("Video mode is not fullscreen valid");
					mode = VideoMode::GetFullscreenModes()[0];
				}

				s_fullscreenWindow = this;
			}
		}
		else if (style & WindowStyle::Closable || style & WindowStyle::Resizable)
			style |= WindowStyle::Titlebar;

		m_asyncWindow = (style & WindowStyle::Threaded) != 0;

		std::unique_ptr<WindowImpl> impl = std::make_unique<WindowImpl>(this);
		if (!impl->Create(mode, title, style))
		{
			NazaraError("Failed to create window implementation");
			return false;
		}

		m_impl = impl.release();
		CallOnExit destroyOnFailure([this] () { Destroy(); });

		m_closed = false;
		m_ownsWindow = true;

		if (!OnWindowCreated())
		{
			NazaraError("Failed to initialize window extension");
			return false;
		}

		// Default parameters
		m_impl->EnableKeyRepeat(true);
		m_impl->EnableSmoothScrolling(false);
		m_impl->SetMaximumSize(-1, -1);
		m_impl->SetMinimumSize(-1, -1);
		m_impl->SetVisible(true);

		if (opened)
			m_impl->SetPosition(position.x, position.y);

		OnWindowResized();

		destroyOnFailure.Reset();

		return true;
	}

	bool Window::Create(void* handle)
	{
		Destroy();

		m_asyncWindow = false;
		m_impl = new WindowImpl(this);
		if (!m_impl->Create(handle))
		{
			NazaraError("Unable to create window implementation");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		m_closed = false;
		m_ownsWindow = false;

		if (!OnWindowCreated())
		{
			NazaraError("Failed to initialize window's derivate");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		return true;
	}

	void Window::Destroy()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		m_cursor.reset();

		if (m_impl)
		{
			OnWindowDestroy();

			m_impl->Destroy();
			delete m_impl;
			m_impl = nullptr;

			if (s_fullscreenWindow == this)
				s_fullscreenWindow = nullptr;
		}
	}

	void Window::EnableKeyRepeat(bool enable)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->EnableKeyRepeat(enable);
	}

	void Window::EnableSmoothScrolling(bool enable)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->EnableSmoothScrolling(enable);
	}

	Vector2i Window::GetPosition() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return Vector2i::Zero();
		}
		#endif

		return m_impl->GetPosition();
	}

	Vector2ui Window::GetSize() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return Vector2ui::Zero();
		}
		#endif

		return m_impl->GetSize();
	}

	WindowStyleFlags Window::GetStyle() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return 0;
		}
		#endif

		return m_impl->GetStyle();
	}

	WindowHandle Window::GetSystemHandle() const
	{
#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return {};
		}
#endif

		return m_impl->GetSystemHandle();
	}

	std::string Window::GetTitle() const
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return {};
		}
		#endif

		return m_impl->GetTitle();
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

	bool Window::PollEvent(WindowEvent* event)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		if (!m_asyncWindow)
			m_impl->ProcessEvents(false);

		if (!m_events.empty())
		{
			if (event)
				*event = m_events.front();

			m_events.pop();

			return true;
		}

		return false;
	}

	void Window::ProcessEvents(bool block)
	{
		NazaraAssert(m_impl, "Window not created");
		NazaraUnused(block);

		if (!m_asyncWindow)
			m_impl->ProcessEvents(block);
		else
		{
			std::lock_guard<std::mutex> eventLock(m_eventMutex);

			for (const WindowEvent& event : m_pendingEvents)
				HandleEvent(event);

			m_pendingEvents.clear();
		}
	}

	void Window::SetCursor(std::shared_ptr<Cursor> cursor)
	{
		NazaraAssert(m_impl, "Window not created");
		NazaraAssert(cursor && cursor->IsValid(), "Invalid cursor");

		m_cursor = std::move(cursor);
		m_impl->SetCursor(*m_cursor);
	}

	void Window::SetEventListener(bool listener)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->SetEventListener(listener);
		if (!listener)
		{
			// Empty the event queue
			while (!m_events.empty())
				m_events.pop();
		}
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

		m_impl->SetFocus();
	}

	void Window::SetIcon(std::shared_ptr<Icon> icon)
	{
		NazaraAssert(m_impl, "Window not created");
		NazaraAssert(icon, "Invalid icon");

		m_icon = std::move(icon);
		m_impl->SetIcon(*m_icon);
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

		m_impl->SetMaximumSize(maxSize.x, maxSize.y);
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

		m_impl->SetMaximumSize(width, height);
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

		m_impl->SetMinimumSize(minSize.x, minSize.y);
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

		m_impl->SetMinimumSize(width, height);
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

		m_impl->SetPosition(position.x, position.y);
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

		m_impl->SetPosition(x, y);
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

		m_impl->SetSize(size.x, size.y);
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

		m_impl->SetSize(width, height);
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

		m_impl->SetStayOnTop(stayOnTop);
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

		m_impl->SetTitle(title);
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

		m_impl->SetVisible(visible);
	}

	bool Window::WaitEvent(WindowEvent* event)
	{
		#if NAZARA_PLATFORM_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		if (!m_asyncWindow)
		{
			while (m_events.empty())
				m_impl->ProcessEvents(true);

			if (event)
				*event = m_events.front();

			m_events.pop();

			return true;
		}
		else
		{
			std::lock_guard<std::mutex> lock(m_eventMutex);

			if (m_events.empty())
			{
				m_waitForEvent = true;
				{
					m_eventMutex.unlock();

					std::unique_lock<std::mutex> eventConditionLock(m_eventConditionMutex);
					m_eventCondition.wait(eventConditionLock);

					m_eventMutex.lock();
				}
				m_waitForEvent = false;
			}

			if (!m_events.empty())
			{
				if (event)
					*event = m_events.front();

				m_events.pop();

				return true;
			}

			return false;
		}
	}

	Window& Window::operator=(Window&& window)
	{
		m_events = std::move(window.m_events);
		m_pendingEvents = std::move(window.m_pendingEvents);
		m_cursorController = std::move(window.m_cursorController);
		m_cursor = std::move(window.m_cursor);
		m_eventHandler = std::move(window.m_eventHandler);
		m_icon = std::move(window.m_icon);
		m_asyncWindow = window.m_asyncWindow;
		m_closed = window.m_asyncWindow;
		m_closeOnQuit = window.m_closeOnQuit;
		m_eventPolling = window.m_eventPolling;
		m_ownsWindow = window.m_asyncWindow;
		m_waitForEvent = window.m_waitForEvent;

		window.DisconnectSlots();
		ConnectSlots();

		return *this;
	}

	void* Window::GetHandle()
	{
		return (m_impl) ? m_impl->GetHandle() : nullptr;
	}

	bool Window::OnWindowCreated()
	{
		return true;
	}

	void Window::OnWindowDestroy()
	{
	}

	void Window::OnWindowResized()
	{
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

	void Window::HandleEvent(const WindowEvent& event)
	{
		if (m_eventPolling)
			m_events.push(event);

		m_eventHandler.Dispatch(event);

		switch (event.type)
		{
			case WindowEventType::MouseEntered:
				m_impl->RefreshCursor();
				break;

			case WindowEventType::Resized:
				OnWindowResized();
				break;

			case WindowEventType::Quit:
				if (m_closeOnQuit)
					Close();

				break;

			default:
				break;
		}
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
