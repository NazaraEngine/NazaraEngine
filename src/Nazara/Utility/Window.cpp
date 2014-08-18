// Copyright (C) 2014 Jérôme Leclercq
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
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Utility/Linux/WindowImpl.hpp>
#else
	#error Lack of implementation: Window
#endif

#include <Nazara/Utility/Debug.hpp>

namespace
{
	NzWindow* fullscreenWindow = nullptr;
}

NzWindow::NzWindow() :
#if NAZARA_UTILITY_THREADED_WINDOW
m_impl(nullptr),
m_eventListener(true),
m_waitForEvent(false)
#else
m_impl(nullptr)
#endif
{
}

NzWindow::NzWindow(NzVideoMode mode, const NzString& title, nzUInt32 style) :
#if NAZARA_UTILITY_THREADED_WINDOW
m_impl(nullptr),
m_eventListener(true),
m_waitForEvent(false)
#else
m_impl(nullptr)
#endif
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);
	Create(mode, title, style);
}

NzWindow::NzWindow(NzWindowHandle handle) :
#if NAZARA_UTILITY_THREADED_WINDOW
m_impl(nullptr),
m_eventListener(true),
m_waitForEvent(false)
#else
m_impl(nullptr)
#endif
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);
	Create(handle);
}

NzWindow::~NzWindow()
{
	Destroy();
}

void NzWindow::Close()
{
	m_closed = true; // On retarde la fermeture jusqu'au prochain IsOpen
}

bool NzWindow::Create(NzVideoMode mode, const NzString& title, nzUInt32 style)
{
	// Si la fenêtre est déjà ouverte, nous conservons sa position
	bool opened = IsOpen();
	NzVector2i position;
	if (opened)
		position = m_impl->GetPosition();

	Destroy();

	// Inspiré du code de la SFML par Laurent Gomila
	if (style & nzWindowStyle_Fullscreen)
	{
		if (fullscreenWindow)
		{
			NazaraError("Window " + NzString::Pointer(fullscreenWindow) + " already in fullscreen mode");
			style &= ~nzWindowStyle_Fullscreen;
		}
		else
		{
			if (!mode.IsFullscreenValid())
			{
				NazaraWarning("Video mode is not fullscreen valid");
				mode = NzVideoMode::GetFullscreenModes()[0];
			}

			fullscreenWindow = this;
		}
	}
	else if (style & nzWindowStyle_Closable || style & nzWindowStyle_Resizable)
		style |= nzWindowStyle_Titlebar;

	m_impl = new NzWindowImpl(this);
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
	m_impl->SetCursor(nzWindowCursor_Default);
	m_impl->SetMaximumSize(-1, -1);
	m_impl->SetMinimumSize(-1, -1);
	m_impl->SetVisible(true);

	if (opened)
		m_impl->SetPosition(position.x, position.y);

	return true;
}

bool NzWindow::Create(NzWindowHandle handle)
{
	Destroy();

	m_impl = new NzWindowImpl(this);
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

void NzWindow::Destroy()
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

void NzWindow::EnableKeyRepeat(bool enable)
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

void NzWindow::EnableSmoothScrolling(bool enable)
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

NzWindowHandle NzWindow::GetHandle() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return static_cast<NzWindowHandle>(0);
	}
	#endif

	return m_impl->GetHandle();
}

unsigned int NzWindow::GetHeight() const
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

NzVector2i NzWindow::GetPosition() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return NzVector2i::Zero();
	}
	#endif

	return m_impl->GetPosition();
}

NzVector2ui NzWindow::GetSize() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return NzVector2ui::Zero();
	}
	#endif

	return m_impl->GetSize();
}

nzUInt32 NzWindow::GetStyle() const
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

NzString NzWindow::GetTitle() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return NzString();
	}
	#endif

	return m_impl->GetTitle();
}

unsigned int NzWindow::GetWidth() const
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

bool NzWindow::HasFocus() const
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

bool NzWindow::IsOpen(bool checkClosed)
{
	if (m_impl)
	{
		if (m_closed && checkClosed)
		{
			Destroy();
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

bool NzWindow::IsOpen() const
{
	return m_impl != nullptr;
}

bool NzWindow::IsMinimized() const
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

bool NzWindow::IsValid() const
{
	return m_impl != nullptr;
}

bool NzWindow::IsVisible() const
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

bool NzWindow::PollEvent(NzEvent* event)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return false;
	}
	#endif

	#if NAZARA_UTILITY_THREADED_WINDOW
	NzLockGuard lock(m_eventMutex);
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

void NzWindow::SetCursor(nzWindowCursor cursor)
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

void NzWindow::SetCursor(const NzCursor& cursor)
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

void NzWindow::SetEventListener(bool listener)
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
		NzLockGuard lock(m_eventMutex);
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

void NzWindow::SetFocus()
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

void NzWindow::SetIcon(const NzIcon& icon)
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

void NzWindow::SetMaximumSize(const NzVector2i& maxSize)
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

void NzWindow::SetMaximumSize(int width, int height)
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

void NzWindow::SetMinimumSize(const NzVector2i& minSize)
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

void NzWindow::SetMinimumSize(int width, int height)
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

void NzWindow::SetPosition(const NzVector2i& position)
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

void NzWindow::SetPosition(int x, int y)
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

void NzWindow::SetSize(const NzVector2i& size)
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

void NzWindow::SetSize(unsigned int width, unsigned int height)
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

void NzWindow::SetStayOnTop(bool stayOnTop)
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

void NzWindow::SetTitle(const NzString& title)
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

void NzWindow::SetVisible(bool visible)
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

bool NzWindow::WaitEvent(NzEvent* event)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Window not created");
		return false;
	}
	#endif

	#if NAZARA_UTILITY_THREADED_WINDOW
	NzLockGuard lock(m_eventMutex);

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

bool NzWindow::OnWindowCreated()
{
	return true;
}

void NzWindow::OnWindowDestroy()
{
}

void NzWindow::OnWindowResized()
{
}

void NzWindow::IgnoreNextMouseEvent(int mouseX, int mouseY) const
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

void NzWindow::PushEvent(const NzEvent& event)
{
	#if NAZARA_UTILITY_THREADED_WINDOW
	m_eventMutex.Lock();
	#endif

	m_events.push(event);
	if (event.type == nzEventType_Resized)
		OnWindowResized();

	#if NAZARA_UTILITY_THREADED_WINDOW
	m_eventMutex.Unlock();

	if (m_waitForEvent)
	{
		m_eventConditionMutex.Lock();
		m_eventCondition.Signal();
		m_eventConditionMutex.Unlock();
	}
	#endif
}

bool NzWindow::Initialize()
{
	return NzWindowImpl::Initialize();
}

void NzWindow::Uninitialize()
{
	NzWindowImpl::Uninitialize();
}
