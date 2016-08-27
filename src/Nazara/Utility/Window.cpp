// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Window.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Icon.hpp>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/WindowImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Utility/X11/WindowImpl.hpp>
#else
	#error Lack of implementation: Window
#endif

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		Window* fullscreenWindow = nullptr;
	}

	bool Window::Create(VideoMode mode, const String& title, UInt32 style)
	{
		// Si la fenêtre est déjà ouverte, nous conservons sa position
		bool opened = IsOpen();
		Vector2i position;
		if (opened)
			position = m_impl->GetPosition();

		Destroy();

		// Inspiré du code de la SFML par Laurent Gomila
		if (style & WindowStyle_Fullscreen)
		{
			if (fullscreenWindow)
			{
				NazaraError("Window " + String::Pointer(fullscreenWindow) + " already in fullscreen mode");
				style &= ~WindowStyle_Fullscreen;
			}
			else
			{
				if (!mode.IsFullscreenValid())
				{
					NazaraWarning("Video mode is not fullscreen valid");
					mode = VideoMode::GetFullscreenModes()[0];
				}

				fullscreenWindow = this;
			}
		}
		else if (style & WindowStyle_Closable || style & WindowStyle_Resizable)
			style |= WindowStyle_Titlebar;

		m_impl = new WindowImpl(this);
		if (!m_impl->Create(mode, title, style))
		{
			NazaraError("Failed to create window implementation");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		m_closed = false;
		m_ownsWindow = true;

		if (!OnWindowCreated())
		{
			NazaraError("Failed to initialize window extension");
			delete m_impl;
			m_impl = nullptr;

			return false;
		}

		// Paramètres par défaut
		m_impl->EnableKeyRepeat(true);
		m_impl->EnableSmoothScrolling(false);
		m_impl->SetCursor(WindowCursor_Default);
		m_impl->SetMaximumSize(-1, -1);
		m_impl->SetMinimumSize(-1, -1);
		m_impl->SetVisible(true);

		if (opened)
			m_impl->SetPosition(position.x, position.y);

		return true;
	}

	bool Window::Create(WindowHandle handle)
	{
		Destroy();

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
		if (m_impl)
		{
			OnWindowDestroy();

			m_impl->Destroy();
			delete m_impl;
			m_impl = nullptr;

			if (fullscreenWindow == this)
				fullscreenWindow = nullptr;
		}
	}

	void Window::EnableKeyRepeat(bool enable)
	{
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->EnableSmoothScrolling(enable);
	}

	WindowHandle Window::GetHandle() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return static_cast<WindowHandle>(0);
		}
		#endif

		return m_impl->GetHandle();
	}

	unsigned int Window::GetHeight() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return 0;
		}
		#endif

		return m_impl->GetHeight();
	}

	Vector2i Window::GetPosition() const
	{
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return Vector2ui::Zero();
		}
		#endif

		return m_impl->GetSize();
	}

	UInt32 Window::GetStyle() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return 0;
		}
		#endif

		return m_impl->GetStyle();
	}

	String Window::GetTitle() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return String();
		}
		#endif

		return m_impl->GetTitle();
	}

	unsigned int Window::GetWidth() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return 0;
		}
		#endif

		return m_impl->GetWidth();
	}

	bool Window::HasFocus() const
	{
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		#if NAZARA_UTILITY_THREADED_WINDOW
		LockGuard lock(m_eventMutex);
		#else
		m_impl->ProcessEvents(false);
		#endif

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

		#if !NAZARA_UTILITY_THREADED_WINDOW
		m_impl->ProcessEvents(block);
		#endif
	}

	void Window::SetCursor(WindowCursor cursor)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->SetCursor(cursor);
	}

	void Window::SetCursor(const Cursor& cursor)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}

		if (!cursor.IsValid())
		{
			NazaraError("Cursor is not valid");
			return;
		}
		#endif

		m_impl->SetCursor(cursor);
	}

	void Window::SetEventListener(bool listener)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		#if NAZARA_UTILITY_THREADED_WINDOW
		m_impl->SetEventListener(listener);
		if (!listener)
		{
			// On vide la pile des évènements
			LockGuard lock(m_eventMutex);
			while (!m_events.empty())
				m_events.pop();
		}
		#else
		if (m_ownsWindow)
		{
			// Inutile de transmettre l'ordre dans ce cas-là
			if (!listener)
				NazaraError("A non-threaded window needs to listen to events");
		}
		else
			m_impl->SetEventListener(listener);
		#endif
	}

	void Window::SetFocus()
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->SetFocus();
	}

	void Window::SetIcon(const Icon& icon)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}

		if (!icon.IsValid())
		{
			NazaraError("Icon is not valid");
			return;
		}
		#endif

		m_impl->SetIcon(icon);
	}

	void Window::SetMaximumSize(const Vector2i& maxSize)
	{
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return;
		}
		#endif

		m_impl->SetStayOnTop(stayOnTop);
	}

	void Window::SetTitle(const String& title)
	{
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
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
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Window not created");
			return false;
		}
		#endif

		#if NAZARA_UTILITY_THREADED_WINDOW
		LockGuard lock(m_eventMutex);

		if (m_events.empty())
		{
			m_waitForEvent = true;
			m_eventConditionMutex.Lock();
			m_eventMutex.Unlock();
			m_eventCondition.Wait(&m_eventConditionMutex);
			m_eventMutex.Lock();
			m_eventConditionMutex.Unlock();
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
		#else
		while (m_events.empty())
			m_impl->ProcessEvents(true);

		if (event)
			*event = m_events.front();

		m_events.pop();

		return true;
		#endif
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

	void Window::IgnoreNextMouseEvent(int mouseX, int mouseY) const
	{
		#if NAZARA_UTILITY_SAFE
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
