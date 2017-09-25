// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	/*!
	* \class Nz::Window
	*/

	inline Window::Window(VideoMode mode, const String& title, WindowStyleFlags style) :
	Window()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(mode, title, style);
	}

	inline Window::Window(WindowHandle handle) :
	Window()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(handle);
	}

	/*!
	* \brief Constructs a Window object by moving another one
	*/
	inline Window::Window(Window&& window) noexcept :
	m_impl(window.m_impl),
	m_events(std::move(window.m_events)),
	m_pendingEvents(std::move(window.m_pendingEvents)),
	m_eventCondition(std::move(window.m_eventCondition)),
	m_eventHandler(std::move(window.m_eventHandler)),
	m_eventMutex(std::move(window.m_eventMutex)),
	m_eventConditionMutex(std::move(window.m_eventConditionMutex)),
	m_closed(window.m_closed),
	m_closeOnQuit(window.m_closeOnQuit),
	m_eventPolling(window.m_eventPolling),
	m_ownsWindow(window.m_ownsWindow),
	m_waitForEvent(window.m_waitForEvent)
	{
		window.m_impl = nullptr;
	}

	inline void Window::Close()
	{
		m_closed = true; // The window will be closed at the next non-const IsOpen() call
	}

	inline void Window::EnableCloseOnQuit(bool closeOnQuit)
	{
		m_closeOnQuit = closeOnQuit;
	}

	inline void Window::EnableEventPolling(bool enable)
	{
		m_eventPolling = enable;
		if (!m_eventPolling)
		{
			while (!m_events.empty())
				m_events.pop();
		}
	}

	inline const CursorRef& Window::GetCursor() const
	{
		return m_cursor;
	}

	inline CursorController& Nz::Window::GetCursorController()
	{
		return m_cursorController;
	}

	inline EventHandler& Nz::Window::GetEventHandler()
	{
		return m_eventHandler;
	}

	inline bool Window::IsOpen(bool checkClosed)
	{
		if (!m_impl)
			return false;

		if (checkClosed && m_closed)
		{
			Destroy();
			return false;
		}

		return true;
	}

	inline bool Window::IsOpen() const
	{
		return m_impl != nullptr;
	}

	inline bool Window::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline void Window::SetCursor(SystemCursor systemCursor)
	{
		SetCursor(Cursor::Get(systemCursor));
	}

	inline void Window::HandleEvent(const WindowEvent& event)
	{
		if (m_eventPolling)
			m_events.push(event);

		m_eventHandler.Dispatch(event);

		if (event.type == WindowEventType_Resized)
			OnWindowResized();

		if (event.type == WindowEventType_Quit && m_closeOnQuit)
			Close();
	}

	inline void Window::PushEvent(const WindowEvent& event)
	{
		if (!m_asyncWindow)
			HandleEvent(event);
		else
		{
			{
				LockGuard eventLock(m_eventMutex);

				m_pendingEvents.push_back(event);
			}

			if (m_waitForEvent)
			{
				m_eventConditionMutex.Lock();
				m_eventCondition.Signal();
				m_eventConditionMutex.Unlock();
			}
		}
	}

	/*!
	* \brief Moves a window to another window object
	* \return A reference to the object
	*/
	inline Window& Window::operator=(Window&& window)
	{
		Destroy();

		m_closed              = window.m_closed;
		m_closeOnQuit         = window.m_closeOnQuit;
		m_eventCondition      = std::move(window.m_eventCondition);
		m_eventConditionMutex = std::move(window.m_eventConditionMutex);
		m_eventHandler        = std::move(window.m_eventHandler);
		m_eventMutex          = std::move(window.m_eventMutex);
		m_eventPolling        = window.m_eventPolling;
		m_impl                = window.m_impl;
		m_events              = std::move(window.m_events);
		m_pendingEvents       = std::move(window.m_pendingEvents);
		m_ownsWindow          = window.m_ownsWindow;
		m_waitForEvent        = window.m_waitForEvent;

		window.m_impl = nullptr;

		return *this;
	}
}

#include <Nazara/Platform/DebugOff.hpp>
