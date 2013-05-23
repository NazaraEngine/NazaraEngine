// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Un grand merci à Laurent Gomila pour la SFML qui m'aura bien aidé à réaliser cette implémentation

#define OEMRESOURCE

#include <Nazara/Utility/Win32/WindowImpl.hpp>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Icon.hpp>
#include <Nazara/Utility/Win32/CursorImpl.hpp>
#include <Nazara/Utility/Win32/IconImpl.hpp>
#include <cstdio>
#include <memory>
#include <windowsx.h>
#include <Nazara/Utility/Debug.hpp>

#ifdef _WIN64
	#define GCL_HCURSOR GCLP_HCURSOR
	#define GWL_USERDATA GWLP_USERDATA
#endif

// N'est pas définit avec MinGW
#ifndef MAPVK_VK_TO_VSC
    #define MAPVK_VK_TO_VSC 0
#endif

#undef IsMinimized // Conflit avec la méthode du même nom

namespace
{
	const wchar_t* className = L"Nazara Window";
	NzWindowImpl* fullscreenWindow = nullptr;
}

NzWindowImpl::NzWindowImpl(NzWindow* parent) :
m_cursor(nullptr),
m_handle(nullptr),
m_callback(0),
m_style(0),
m_maxSize(-1),
m_minSize(-1),
m_parent(parent),
m_keyRepeat(true),
m_mouseInside(false),
m_smoothScrolling(false),
m_scrolling(0)
{
}

bool NzWindowImpl::Create(NzVideoMode mode, const NzString& title, nzUInt32 style)
{
	bool fullscreen = (style & nzWindowStyle_Fullscreen) != 0;
	DWORD win32Style, win32StyleEx;
	unsigned int x, y;
	unsigned int width = mode.width;
	unsigned int height = mode.height;
	if (fullscreen)
	{
		DEVMODE win32Mode;
		std::memset(&win32Mode, 0, sizeof(DEVMODE));
		win32Mode.dmBitsPerPel = mode.bitsPerPixel;
		win32Mode.dmPelsHeight = mode.height;
		win32Mode.dmPelsWidth  = mode.width;
		win32Mode.dmFields	   = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		win32Mode.dmSize	   = sizeof(DEVMODE);

		if (ChangeDisplaySettings(&win32Mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// Situation extrêmement rare grâce à NzVideoMode::IsValid appelé par NzWindow
			NazaraError("Failed to change display settings for fullscreen, this video mode is not supported by your computer");
			fullscreen = false;
		}
	}

	// Testé une seconde fois car sa valeur peut changer
	if (fullscreen)
	{
		x = 0;
		y = 0;
		win32Style = WS_CLIPCHILDREN | WS_POPUP;

		// Pour cacher la barre des tâches
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
		win32StyleEx = WS_EX_APPWINDOW;

		fullscreenWindow = this;
	}
	else
	{
		win32Style = WS_VISIBLE;
		if (style & nzWindowStyle_Titlebar)
		{
			win32Style |= WS_CAPTION | WS_MINIMIZEBOX;
			if (style & nzWindowStyle_Closable)
				win32Style |= WS_SYSMENU;

			if (style & nzWindowStyle_Resizable)
				win32Style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
		}
		else
			win32Style |= WS_POPUP;

		win32StyleEx = 0;

		RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
		AdjustWindowRect(&rect, win32Style, false);
		width = rect.right-rect.left;
		height = rect.bottom-rect.top;

		x = (GetSystemMetrics(SM_CXSCREEN) - width)/2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height)/2;
	}

	m_callback = 0;

	std::unique_ptr<wchar_t> wtitle(title.GetWideBuffer());

	#if NAZARA_UTILITY_THREADED_WINDOW
	NzMutex mutex;
	NzConditionVariable condition;
	m_threadActive = true;

	// On attend que la fenêtre soit créée
	mutex.Lock();
	m_thread = new NzThread(WindowThread, &m_handle, win32StyleEx, wtitle.get(), win32Style, x, y, width, height, this, &mutex, &condition);
	condition.Wait(&mutex);
	mutex.Unlock();
	#else
	m_handle = CreateWindowExW(win32StyleEx, className, wtitle.get(), win32Style, x, y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);
	#endif

	if (fullscreen)
	{
		SetForegroundWindow(m_handle);
		ShowWindow(m_handle, SW_SHOW);
	}

	m_eventListener = true;
	m_ownsWindow = true;
	#if !NAZARA_UTILITY_THREADED_WINDOW
	m_sizemove = false;
	#endif
	m_style = style;

	return m_handle != nullptr;
}

bool NzWindowImpl::Create(NzWindowHandle handle)
{
	m_handle = reinterpret_cast<HWND>(handle);

	if (!m_handle || !IsWindow(m_handle))
	{
		NazaraError("Invalid handle");
		return false;
	}

	m_eventListener = false;
	m_ownsWindow = false;
	#if !NAZARA_UTILITY_THREADED_WINDOW
	m_sizemove = false;
	#endif
	m_style = RetrieveStyle(m_handle);

	return true;
}

void NzWindowImpl::Destroy()
{
	if (m_ownsWindow)
	{
		#if NAZARA_UTILITY_THREADED_WINDOW
		if (m_thread)
		{
			m_threadActive = false;
			PostMessageW(m_handle, WM_NULL, 0, 0); // Pour réveiller le thread

			m_thread->Join();
			delete m_thread;
		}
		#else
		if (m_handle)
			DestroyWindow(m_handle);
		#endif
	}
	else
		SetEventListener(false);
}

void NzWindowImpl::EnableKeyRepeat(bool enable)
{
	m_keyRepeat = enable;
}

void NzWindowImpl::EnableSmoothScrolling(bool enable)
{
	m_smoothScrolling = enable;
}

NzWindowHandle NzWindowImpl::GetHandle() const
{
	return m_handle;
}

unsigned int NzWindowImpl::GetHeight() const
{
	RECT rect;
	GetClientRect(m_handle, &rect);
	return rect.bottom-rect.top;
}

NzVector2i NzWindowImpl::GetPosition() const
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	return NzVector2i(rect.left, rect.top);
}

NzVector2ui NzWindowImpl::GetSize() const
{
	RECT rect;
	GetClientRect(m_handle, &rect);
	return NzVector2ui(rect.right-rect.left, rect.bottom-rect.top);
}

nzUInt32 NzWindowImpl::GetStyle() const
{
	return m_style;
}

NzString NzWindowImpl::GetTitle() const
{
	unsigned int titleSize = GetWindowTextLengthW(m_handle);
	if (titleSize == 0)
		return NzString();

	titleSize++; // Caractère nul

	std::unique_ptr<wchar_t[]> wTitle(new wchar_t[titleSize]);
	GetWindowTextW(m_handle, wTitle.get(), titleSize);

	return NzString::Unicode(wTitle.get());
}

unsigned int NzWindowImpl::GetWidth() const
{
	RECT rect;
	GetClientRect(m_handle, &rect);
	return rect.right-rect.left;
}

bool NzWindowImpl::HasFocus() const
{
	return GetForegroundWindow() == m_handle;
}

void NzWindowImpl::IgnoreNextMouseEvent(int mouseX, int mouseY)
{
	// Petite astuce ...
	m_mousePos.x = mouseX;
	m_mousePos.y = mouseY;
}

bool NzWindowImpl::IsMinimized() const
{
	return IsIconic(m_handle);
}

bool NzWindowImpl::IsVisible() const
{
	return IsWindowVisible(m_handle);
}

void NzWindowImpl::ProcessEvents(bool block)
{
	if (m_ownsWindow)
	{
		if (block)
			WaitMessage();

		MSG message;
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}
}

void NzWindowImpl::SetCursor(nzWindowCursor cursor)
{
	switch (cursor)
	{
		case nzWindowCursor_Crosshair:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Default:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Hand:
		case nzWindowCursor_Pointer:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_HAND, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Help:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_HELP, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Move:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_SIZEALL, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_None:
			m_cursor = nullptr;
			break;

		case nzWindowCursor_Progress:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_APPSTARTING, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_ResizeN:
		case nzWindowCursor_ResizeS:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_SIZENS, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_ResizeNW:
		case nzWindowCursor_ResizeSE:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_SIZENWSE, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_ResizeNE:
		case nzWindowCursor_ResizeSW:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_SIZENESW, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_ResizeE:
		case nzWindowCursor_ResizeW:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Text:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_IBEAM, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;

		case nzWindowCursor_Wait:
			m_cursor = reinterpret_cast<HCURSOR>(LoadImage(nullptr, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_SHARED));
			break;
	}

	// Pas besoin de libérer le curseur par la suite s'il est partagé
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648045(v=vs.85).aspx
	::SetCursor(m_cursor);
}

void NzWindowImpl::SetCursor(const NzCursor& cursor)
{
	m_cursor = cursor.m_impl->GetCursor();

	::SetCursor(m_cursor);
}

void NzWindowImpl::SetEventListener(bool listener)
{
	if (m_ownsWindow)
		m_eventListener = listener;
	else if (listener != m_eventListener)
	{
		if (listener)
		{
			SetWindowLongPtr(m_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			m_callback = SetWindowLongPtr(m_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MessageHandler));
			m_eventListener = true;
		}
		else if (m_eventListener)
		{
			SetWindowLongPtr(m_handle, GWLP_WNDPROC, m_callback);
			m_eventListener = false;
		}
	}
}

void NzWindowImpl::SetFocus()
{
	SetForegroundWindow(m_handle);
}

void NzWindowImpl::SetIcon(const NzIcon& icon)
{
	HICON iconHandle = icon.m_impl->GetIcon();

	SendMessage(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(iconHandle));
	SendMessage(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(iconHandle));
}

void NzWindowImpl::SetMaximumSize(int width, int height)
{
	RECT rect = {0, 0, width, height};
	AdjustWindowRect(&rect, GetWindowLongPtr(m_handle, GWL_STYLE), false);

	if (width != -1)
		m_maxSize.x = rect.right-rect.left;
	else
		m_maxSize.x = -1;

	if (height != -1)
		m_maxSize.y = rect.bottom-rect.top;
	else
		m_maxSize.y = -1;
}

void NzWindowImpl::SetMinimumSize(int width, int height)
{
	RECT rect = {0, 0, width, height};
	AdjustWindowRect(&rect, GetWindowLongPtr(m_handle, GWL_STYLE), false);

	if (width != -1)
		m_minSize.x = rect.right-rect.left;
	else
		m_minSize.x = -1;

	if (height != -1)
		m_minSize.y = rect.bottom-rect.top;
	else
		m_minSize.y = -1;
}

void NzWindowImpl::SetPosition(int x, int y)
{
	SetWindowPos(m_handle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void NzWindowImpl::SetSize(unsigned int width, unsigned int height)
{
	// SetWindowPos demande la taille totale de la fenêtre
	RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
	AdjustWindowRect(&rect, GetWindowLongPtr(m_handle, GWL_STYLE), false);

	SetWindowPos(m_handle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

void NzWindowImpl::SetStayOnTop(bool stayOnTop)
{
	if (stayOnTop)
		SetWindowPos(m_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	else
		SetWindowPos(m_handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void NzWindowImpl::SetTitle(const NzString& title)
{
	std::unique_ptr<wchar_t[]> wTitle(title.GetWideBuffer());
	SetWindowTextW(m_handle, wTitle.get());
}

void NzWindowImpl::SetVisible(bool visible)
{
	ShowWindow(m_handle, (visible) ? SW_SHOW : SW_HIDE);
}

bool NzWindowImpl::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Inutile de récupérer des évènements ne venant pas de notre fenêtre
	if (m_handle != window)
		return false;

	switch (message)
	{
		case WM_DESTROY:
			if (fullscreenWindow == this)
				ChangeDisplaySettings(nullptr, 0);

			break;

		case WM_SETCURSOR:
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648382(v=vs.85).aspx
			if (LOWORD(lParam) == HTCLIENT)
				::SetCursor(m_cursor);

			break;

		case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);

			pos->cx = std::max(pos->cx, m_minSize.x);
			pos->cy = std::max(pos->cy, m_minSize.y);

			if (m_maxSize.x >= 0)
				pos->cx = std::min(pos->cx, m_maxSize.x);

			if (m_maxSize.y >= 0)
				pos->cy = std::min(pos->cy, m_maxSize.y);

			break;
		}

		default:
			break;
	}

	if (m_eventListener)
	{
		switch (message)
		{
			case WM_CHAR:
			{
				// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
				bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
				if (m_keyRepeat || !repeated)
				{
					NzEvent event;
					event.type = nzEventType_TextEntered;
					event.text.character = static_cast<char32_t>(wParam);
					event.text.repeated = repeated;
					m_parent->PushEvent(event);
				}

				break;
			}

			case WM_CLOSE:
			{
				NzEvent event;
				event.type = nzEventType_Quit;
				m_parent->PushEvent(event);

				return true; // Afin que Windows ne ferme pas la fenêtre automatiquement
			}

			#if !NAZARA_UTILITY_THREADED_WINDOW
			case WM_ENTERSIZEMOVE:
			{
				m_sizemove = true;
				break;
			}

			case WM_EXITSIZEMOVE:
			{
				m_sizemove = false;

				// On vérifie ce qui a changé
				NzVector2i position = GetPosition();
				if (m_position != position)
				{
					m_position = position;

					NzEvent event;
					event.type = nzEventType_Moved;
					event.position.x = position.x;
					event.position.y = position.y;
					m_parent->PushEvent(event);
				}

				NzVector2ui size = GetSize();
				if (m_size != size)
				{
					m_size = size;

					NzEvent event;
					event.type = nzEventType_Resized;
					event.size.width = size.x;
					event.size.height = size.y;
					m_parent->PushEvent(event);
				}
			}
			#endif

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
				bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
				if (m_keyRepeat || !repeated)
				{
					NzEvent event;
					event.type = nzEventType_KeyPressed;
					event.key.code = ConvertVirtualKey(wParam, lParam);
					event.key.alt = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
					event.key.control = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
					event.key.repeated = repeated;
					event.key.shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
					event.key.system = (((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0));
					m_parent->PushEvent(event);
				}

				break;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
				NzEvent event;
				event.type = nzEventType_KeyReleased;
				event.key.code = ConvertVirtualKey(wParam, lParam);
				event.key.alt = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
				event.key.control = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
				event.key.shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
				event.key.system = ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000));
				m_parent->PushEvent(event);

				break;
			}

			case WM_KILLFOCUS:
			{
				NzEvent event;
				event.type = nzEventType_LostFocus;
				m_parent->PushEvent(event);

				break;
			}

			case WM_LBUTTONDBLCLK:
			{
				// Cet évènement est généré à la place d'un WM_LBUTTONDOWN lors d'un double-clic.
				// Comme nous désirons quand même notifier chaque clic, nous envoyons les deux évènements.
				NzEvent event;
				event.mouseButton.button = NzMouse::Left;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);

				event.type = nzEventType_MouseButtonDoubleClicked;
				m_parent->PushEvent(event);

				event.type = nzEventType_MouseButtonPressed;
				m_parent->PushEvent(event);

				break;
			}

			case WM_LBUTTONDOWN:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonPressed;
				event.mouseButton.button = NzMouse::Left;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_LBUTTONUP:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonReleased;
				event.mouseButton.button = NzMouse::Left;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_MBUTTONDBLCLK:
			{
				NzEvent event;
				event.mouseButton.button = NzMouse::Middle;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);

				event.type = nzEventType_MouseButtonDoubleClicked;
				m_parent->PushEvent(event);

				event.type = nzEventType_MouseButtonPressed;
				m_parent->PushEvent(event);

				break;
			}

			case WM_MBUTTONDOWN:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonPressed;
				event.mouseButton.button = NzMouse::Middle;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_MBUTTONUP:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonReleased;
				event.mouseButton.button = NzMouse::Middle;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			// Nécessite un appel précédent à TrackMouseEvent (Fait dans WM_MOUSEMOVE)
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645615(v=vs.85).aspx
			case WM_MOUSELEAVE:
			{
				m_mouseInside = false;

				NzEvent event;
				event.type = nzEventType_MouseLeft;
				m_parent->PushEvent(event);
				break;
			}

			case WM_MOUSEMOVE:
			{
				int currentX = GET_X_LPARAM(lParam);
				int currentY = GET_Y_LPARAM(lParam);

				if (!m_mouseInside)
				{
					m_mouseInside = true;

					// On créé un évènement pour être informé de la sortie de la fenêtre
					TRACKMOUSEEVENT mouseEvent;
					mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
					mouseEvent.dwFlags = TME_LEAVE;
					mouseEvent.hwndTrack = m_handle;

					TrackMouseEvent(&mouseEvent);

					NzEvent event;
					event.type = nzEventType_MouseEntered;
					m_parent->PushEvent(event);

					event.type = nzEventType_MouseMoved;

					// Le delta sera 0
					event.mouseMove.deltaX = 0;
					event.mouseMove.deltaY = 0;

					event.mouseMove.x = currentX;
					event.mouseMove.y = currentY;

					m_mousePos.x = currentX;
					m_mousePos.y = currentY;

					m_parent->PushEvent(event);
					break;
				}

				// Si la souris n'a pas bougé (Ou qu'on veut ignorer l'évènement)
				if (m_mousePos.x == currentX && m_mousePos.y == currentY)
					break;

				NzEvent event;
				event.type = nzEventType_MouseMoved;
				event.mouseMove.deltaX = currentX - m_mousePos.x;
				event.mouseMove.deltaY = currentY - m_mousePos.y;
				event.mouseMove.x = currentX;
				event.mouseMove.y = currentY;

				m_mousePos.x = currentX;
				m_mousePos.y = currentY;

				m_parent->PushEvent(event);
				break;
			}

			case WM_MOUSEWHEEL:
			{
				if (m_smoothScrolling)
				{
					NzEvent event;
					event.type = nzEventType_MouseWheelMoved;
					event.mouseWheel.delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam))/WHEEL_DELTA;
					m_parent->PushEvent(event);
				}
				else
				{
					m_scrolling += GET_WHEEL_DELTA_WPARAM(wParam);
					if (std::abs(m_scrolling) >= WHEEL_DELTA)
					{
						NzEvent event;
						event.type = nzEventType_MouseWheelMoved;
						event.mouseWheel.delta = m_scrolling/WHEEL_DELTA;
						m_parent->PushEvent(event);

						m_scrolling %= WHEEL_DELTA;
					}
				}
				break;
			}

			case WM_MOVE:
			{
				NzVector2i position = GetPosition();

				NzEvent event;
				event.type = nzEventType_Moved;
				event.position.x = position.x;
				event.position.y = position.y;
				m_parent->PushEvent(event);

				break;
			}

			case WM_RBUTTONDBLCLK:
			{
				NzEvent event;
				event.mouseButton.button = NzMouse::Right;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);

				event.type = nzEventType_MouseButtonDoubleClicked;
				m_parent->PushEvent(event);

				event.type = nzEventType_MouseButtonPressed;
				m_parent->PushEvent(event);

				break;
			}

			case WM_RBUTTONDOWN:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonPressed;
				event.mouseButton.button = NzMouse::Right;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_RBUTTONUP:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonReleased;
				event.mouseButton.button = NzMouse::Right;
				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_SETFOCUS:
			{
				NzEvent event;
				event.type = nzEventType_GainedFocus;
				m_parent->PushEvent(event);

				break;
			}

			case WM_SIZE:
			{
				#if NAZARA_UTILITY_THREADED_WINDOW
				if (wParam != SIZE_MINIMIZED)
				#else
				if (!m_sizemove && wParam != SIZE_MINIMIZED)
				#endif
				{
					NzVector2ui size = GetSize(); // On récupère uniquement la taille de la zone client
					#if !NAZARA_UTILITY_THREADED_WINDOW
					if (m_size == size)
						break;

					m_size = size;
					#endif

					NzEvent event;
					event.type = nzEventType_Resized;
					event.size.width = size.x;
					event.size.height = size.y;
					m_parent->PushEvent(event);
				}
				break;
			}

			case WM_UNICHAR:
			{
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646288(v=vs.85).aspx
				if (wParam != UNICODE_NOCHAR)
				{
					bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
					if (m_keyRepeat || !repeated)
					{
						NzEvent event;
						event.type = nzEventType_TextEntered;
						event.text.character = static_cast<char32_t>(wParam);
						event.text.repeated = repeated;
						m_parent->PushEvent(event);
					}

					return true;
				}
			}

			case WM_XBUTTONDBLCLK:
			{
				NzEvent event;
				if (HIWORD(wParam) == XBUTTON1)
					event.mouseButton.button = NzMouse::XButton1;
				else
					event.mouseButton.button = NzMouse::XButton2;

				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);

				event.type = nzEventType_MouseButtonDoubleClicked;
				m_parent->PushEvent(event);

				event.type = nzEventType_MouseButtonPressed;
				m_parent->PushEvent(event);

				break;
			}

			case WM_XBUTTONDOWN:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonPressed;

				if (HIWORD(wParam) == XBUTTON1)
					event.mouseButton.button = NzMouse::XButton1;
				else
					event.mouseButton.button = NzMouse::XButton2;

				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			case WM_XBUTTONUP:
			{
				NzEvent event;
				event.type = nzEventType_MouseButtonReleased;

				if (HIWORD(wParam) == XBUTTON1)
					event.mouseButton.button = NzMouse::XButton1;
				else
					event.mouseButton.button = NzMouse::XButton2;

				event.mouseButton.x = GET_X_LPARAM(lParam);
				event.mouseButton.y = GET_Y_LPARAM(lParam);
				m_parent->PushEvent(event);

				break;
			}

			default:
				break;
		}
	}

	return false;
}

bool NzWindowImpl::Initialize()
{
	// Nous devons faire un type Unicode pour que la fenêtre le soit également
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx
	WNDCLASSW windowClass;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = nullptr;
	windowClass.hCursor = nullptr; // Le curseur est définit dynamiquement
	windowClass.hIcon = nullptr; // L'icône est définie dynamiquement
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.lpfnWndProc = MessageHandler;
	windowClass.lpszClassName = className;
	windowClass.lpszMenuName = nullptr;
	windowClass.style = CS_DBLCLKS; // Gestion du double-clic

	return RegisterClassW(&windowClass);
}

void NzWindowImpl::Uninitialize()
{
	UnregisterClassW(className, GetModuleHandle(nullptr));
}

NzKeyboard::Key NzWindowImpl::ConvertVirtualKey(WPARAM key, LPARAM flags)
{
	switch (key)
	{
		case VK_CONTROL:	return (HIWORD(flags) & KF_EXTENDED) ? NzKeyboard::RControl : NzKeyboard::LControl;
		case VK_MENU:		return (HIWORD(flags) & KF_EXTENDED) ? NzKeyboard::RAlt : NzKeyboard::LAlt;
		case VK_SHIFT:
		{
			static UINT scancode = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
			return (((flags >> 16) & 0xFF) == scancode) ? NzKeyboard::LShift : NzKeyboard::RShift;
		}

		case 0x30:				   return NzKeyboard::Num0;
		case 0x31:				   return NzKeyboard::Num1;
		case 0x32:				   return NzKeyboard::Num2;
		case 0x33:				   return NzKeyboard::Num3;
		case 0x34:				   return NzKeyboard::Num4;
		case 0x35:				   return NzKeyboard::Num5;
		case 0x36:				   return NzKeyboard::Num6;
		case 0x37:				   return NzKeyboard::Num7;
		case 0x38:				   return NzKeyboard::Num8;
		case 0x39:				   return NzKeyboard::Num9;
		case 0x41:				   return NzKeyboard::A;
		case 0x42:				   return NzKeyboard::B;
		case 0x43:				   return NzKeyboard::C;
		case 0x44:				   return NzKeyboard::D;
		case 0x45:				   return NzKeyboard::E;
		case 0x46:				   return NzKeyboard::F;
		case 0x47:				   return NzKeyboard::G;
		case 0x48:				   return NzKeyboard::H;
		case 0x49:				   return NzKeyboard::I;
		case 0x4A:				   return NzKeyboard::J;
		case 0x4B:				   return NzKeyboard::K;
		case 0x4C:				   return NzKeyboard::L;
		case 0x4D:				   return NzKeyboard::M;
		case 0x4E:				   return NzKeyboard::N;
		case 0x4F:				   return NzKeyboard::O;
		case 0x50:				   return NzKeyboard::P;
		case 0x51:				   return NzKeyboard::Q;
		case 0x52:				   return NzKeyboard::R;
		case 0x53:				   return NzKeyboard::S;
		case 0x54:				   return NzKeyboard::T;
		case 0x55:				   return NzKeyboard::U;
		case 0x56:				   return NzKeyboard::V;
		case 0x57:				   return NzKeyboard::W;
		case 0x58:				   return NzKeyboard::X;
		case 0x59:				   return NzKeyboard::Y;
		case 0x5A:				   return NzKeyboard::Z;
		case VK_ADD:			   return NzKeyboard::Add;
		case VK_BACK:			   return NzKeyboard::Backspace;
		case VK_BROWSER_BACK:	   return NzKeyboard::Browser_Back;
		case VK_BROWSER_FAVORITES: return NzKeyboard::Browser_Favorites;
		case VK_BROWSER_FORWARD:   return NzKeyboard::Browser_Forward;
		case VK_BROWSER_HOME:	   return NzKeyboard::Browser_Home;
		case VK_BROWSER_REFRESH:   return NzKeyboard::Browser_Refresh;
		case VK_BROWSER_SEARCH:	   return NzKeyboard::Browser_Search;
		case VK_BROWSER_STOP:	   return NzKeyboard::Browser_Stop;
		case VK_CAPITAL:		   return NzKeyboard::CapsLock;
		case VK_CLEAR:			   return NzKeyboard::Clear;
		case VK_DELETE:			   return NzKeyboard::Delete;
		case VK_DIVIDE:			   return NzKeyboard::Divide;
		case VK_DOWN:			   return NzKeyboard::Down;
		case VK_END:			   return NzKeyboard::End;
		case VK_ESCAPE:			   return NzKeyboard::Escape;
		case VK_F1:				   return NzKeyboard::F1;
		case VK_F2:				   return NzKeyboard::F2;
		case VK_F3:				   return NzKeyboard::F3;
		case VK_F4:				   return NzKeyboard::F4;
		case VK_F5:				   return NzKeyboard::F5;
		case VK_F6:				   return NzKeyboard::F6;
		case VK_F7:				   return NzKeyboard::F7;
		case VK_F8:				   return NzKeyboard::F8;
		case VK_F9:				   return NzKeyboard::F9;
		case VK_F10:			   return NzKeyboard::F10;
		case VK_F11:			   return NzKeyboard::F11;
		case VK_F12:			   return NzKeyboard::F12;
		case VK_F13:			   return NzKeyboard::F13;
		case VK_F14:			   return NzKeyboard::F14;
		case VK_F15:			   return NzKeyboard::F15;
		case VK_HOME:			   return NzKeyboard::Home;
		case VK_INSERT:			   return NzKeyboard::Insert;
		case VK_LEFT:			   return NzKeyboard::Left;
		case VK_LWIN:			   return NzKeyboard::LSystem;
		case VK_MEDIA_NEXT_TRACK:  return NzKeyboard::Media_Next;
		case VK_MEDIA_PLAY_PAUSE:  return NzKeyboard::Media_Play;
		case VK_MEDIA_PREV_TRACK:  return NzKeyboard::Media_Previous;
		case VK_MEDIA_STOP:		   return NzKeyboard::Media_Stop;
		case VK_MULTIPLY:		   return NzKeyboard::Multiply;
		case VK_NEXT:			   return NzKeyboard::PageDown;
		case VK_NUMPAD0:		   return NzKeyboard::Numpad0;
		case VK_NUMPAD1:		   return NzKeyboard::Numpad1;
		case VK_NUMPAD2:		   return NzKeyboard::Numpad2;
		case VK_NUMPAD3:		   return NzKeyboard::Numpad3;
		case VK_NUMPAD4:		   return NzKeyboard::Numpad4;
		case VK_NUMPAD5:		   return NzKeyboard::Numpad5;
		case VK_NUMPAD6:		   return NzKeyboard::Numpad6;
		case VK_NUMPAD7:		   return NzKeyboard::Numpad7;
		case VK_NUMPAD8:		   return NzKeyboard::Numpad8;
		case VK_NUMPAD9:		   return NzKeyboard::Numpad9;
		case VK_NUMLOCK:		   return NzKeyboard::NumLock;
		case VK_OEM_1:			   return NzKeyboard::Semicolon;
		case VK_OEM_2:			   return NzKeyboard::Slash;
		case VK_OEM_3:			   return NzKeyboard::Tilde;
		case VK_OEM_4:			   return NzKeyboard::LBracket;
		case VK_OEM_5:			   return NzKeyboard::Backslash;
		case VK_OEM_6:			   return NzKeyboard::RBracket;
		case VK_OEM_7:			   return NzKeyboard::Quote;
		case VK_OEM_COMMA:		   return NzKeyboard::Comma;
		case VK_OEM_MINUS:		   return NzKeyboard::Dash;
		case VK_OEM_PERIOD:		   return NzKeyboard::Period;
		case VK_OEM_PLUS:		   return NzKeyboard::Equal;
		case VK_RIGHT:			   return NzKeyboard::Right;
		case VK_PRIOR:			   return NzKeyboard::PageUp;
		case VK_PAUSE:			   return NzKeyboard::Pause;
		case VK_PRINT:			   return NzKeyboard::Print;
		case VK_SCROLL:			   return NzKeyboard::ScrollLock;
		case VK_SNAPSHOT:		   return NzKeyboard::PrintScreen;
		case VK_SUBTRACT:		   return NzKeyboard::Subtract;
		case VK_RETURN:			   return NzKeyboard::Return;
		case VK_RWIN:			   return NzKeyboard::RSystem;
		case VK_SPACE:			   return NzKeyboard::Space;
		case VK_TAB:			   return NzKeyboard::Tab;
		case VK_UP:				   return NzKeyboard::Up;
		case VK_VOLUME_DOWN:	   return NzKeyboard::Volume_Down;
		case VK_VOLUME_MUTE:	   return NzKeyboard::Volume_Mute;
		case VK_VOLUME_UP:		   return NzKeyboard::Volume_Up;

		default:
			return NzKeyboard::Undefined;
	}
}

LRESULT CALLBACK NzWindowImpl::MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	NzWindowImpl* me;
	if (message == WM_CREATE)
	{
		me = reinterpret_cast<NzWindowImpl*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(window, GWL_USERDATA, reinterpret_cast<LONG_PTR>(me));
	}
	else
		me = reinterpret_cast<NzWindowImpl*>(GetWindowLongPtr(window, GWL_USERDATA));

	if (me)
	{
		if (me->HandleMessage(window, message, wParam, lParam))
			return 0;
		else if (me->m_callback)
			return CallWindowProcW(reinterpret_cast<WNDPROC>(me->m_callback), window, message, wParam, lParam);
	}

	return DefWindowProcW(window, message, wParam, lParam);
}

nzUInt32 NzWindowImpl::RetrieveStyle(HWND handle)
{
	nzUInt32 style = 0;

	LONG_PTR winStyle = GetWindowLongPtr(handle, GWL_STYLE);

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
	if (winStyle & WS_CAPTION)
	{
		style |= nzWindowStyle_Titlebar;
		if (winStyle & WS_SYSMENU)
			style |= nzWindowStyle_Closable;

		if (winStyle & WS_MAXIMIZEBOX)
			style |= nzWindowStyle_Resizable;
	}

	if (winStyle & WS_SIZEBOX)
		style |= nzWindowStyle_Resizable;

	// Pour déterminer si la fenêtre est en plein écran, il suffit de vérifier si elle recouvre l'écran
	DEVMODE mode;
	mode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	RECT rect;
	if (GetWindowRect(handle, &rect))
	{
		if (static_cast<DWORD>(rect.right-rect.left) == mode.dmPelsWidth && static_cast<DWORD>(rect.bottom-rect.top) == mode.dmPelsHeight)
			style |= nzWindowStyle_Fullscreen;
	}

	return style;
}

#if NAZARA_UTILITY_THREADED_WINDOW
void NzWindowImpl::WindowThread(HWND* handle, DWORD styleEx, const wchar_t* title, DWORD style, unsigned int x, unsigned int y, unsigned int width, unsigned int height, NzWindowImpl* window, NzMutex* mutex, NzConditionVariable* condition)
{
	*handle = CreateWindowExW(styleEx, className, title, style, x, y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), window);

	mutex->Lock();
	condition->Signal();
	mutex->Unlock(); // mutex et condition sont considérés invalides à partir d'ici

	while (window->m_threadActive)
		window->ProcessEvents(true);

	DestroyWindow(*handle);
}
#endif
