// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Android/WindowImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Android/AndroidActivity.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <android/input.h>
#include <android/keycodes.h>

namespace Nz
{
	WindowImpl::WindowImpl(Window* parent) :
	m_window(nullptr),
	m_parent(parent),
	m_created(false)
	{
	}

	bool WindowImpl::Create(const VideoMode& mode, const std::string& title, WindowStyleFlags style)
	{
		if (s_activeWindow)
		{
			NazaraError("only one window can be active at a given time on Android");
			return false;
		}
		s_activeWindow = this;
		m_created = true;

		AndroidActivity* activity = AndroidActivity::Instance();
		m_window = activity->GetNativeWindow();

		m_onWindowCreated.Connect(activity->OnWindowCreated, [this](AndroidActivity*, ANativeWindow* window)
		{
			NazaraAssertMsg(m_window == nullptr, "received window created event without destroying the old one");
			m_window = window;

			ANativeWindow_acquire(m_window);
			ANativeWindow_setBuffersGeometry(m_window, m_width, m_height, 0);

			// "Restored" event
			{
				WindowEvent windowEvent;
				windowEvent.type = WindowEventType::Restored;
				m_parent->HandleEvent(windowEvent);
			}

			// "Resized" event
			{
				auto [width, height] = FetchSize();
				if (width != m_width || height != m_height)
				{
					m_width = width;
					m_height = height;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::Resized;
					windowEvent.size.width = m_width;
					windowEvent.size.height = m_height;

					m_parent->HandleEvent(windowEvent);
				}
			}
		});

		m_onWindowDestroyed.Connect(activity->OnWindowDestroyed, [this](AndroidActivity*, [[maybe_unused]] ANativeWindow* window)
		{
			if (window != m_window)
			{
				NazaraError("received window destroyed event for wrong window");
				return;
			}

			// "Minimized" event
			{
				WindowEvent windowEvent;
				windowEvent.type = WindowEventType::Minimized;
				m_parent->HandleEvent(windowEvent);
			}

			m_window = nullptr;
		});

		m_onWindowResized.Connect(activity->OnWindowResized, [this](AndroidActivity*, ANativeWindow* window)
		{
			if (window != m_window)
			{
				NazaraError("received window resized event for wrong window");
				return;
			}

			auto [width, height] = FetchSize();

			WindowEvent windowEvent;
			windowEvent.type = WindowEventType::Resized;
			windowEvent.size.width = width;
			windowEvent.size.height = height;

			m_parent->HandleEvent(windowEvent);
		});

		m_height = mode.height;
		m_width = mode.width;

		if (m_window)
		{
			ANativeWindow_acquire(m_window);
			ANativeWindow_setBuffersGeometry(m_window, m_width, m_height, 0);

			m_height = ANativeWindow_getHeight(m_window);
			m_width = ANativeWindow_getWidth(m_window);
		}

		return true;
	}

	bool WindowImpl::Create(WindowHandle handle)
	{
		return false;
	}

	void WindowImpl::Destroy()
	{
		if (!m_created)
			return;

		if (m_window)
		{
			ANativeWindow_release(m_window);
			m_window = nullptr;
		}

		assert(s_activeWindow == this);
		s_activeWindow = nullptr;
	}

	Vector2i WindowImpl::FetchPosition() const
	{
		return { 0, 0 };
	}

	Vector2ui WindowImpl::FetchSize() const
	{
		if (!m_window)
			return { m_width, m_height };

		Int32 width, height;
		width = ANativeWindow_getWidth(m_window);
		height = ANativeWindow_getHeight(m_window);

		return { SafeCast<unsigned int>(width), SafeCast<unsigned int>(height) };
	}

	WindowStyleFlags WindowImpl::FetchStyle() const
	{
		return {};
	}

	std::string WindowImpl::FetchTitle() const
	{
		return "";
	}

	ANativeWindow* WindowImpl::GetHandle() const
	{
		return m_window;
	}

	WindowHandle WindowImpl::GetSystemHandle() const
	{
		WindowHandle handle;
		handle.type = WindowBackend::Android;
		handle.android.window = m_window;

		return handle;
	}

	bool WindowImpl::HasFocus() const
	{
		return m_window != nullptr;
	}

	void WindowImpl::IgnoreNextMouseEvent(int /*mouseX*/, int /*mouseY*/)
	{
	}

	bool WindowImpl::IsMinimized() const
	{
		return m_window == nullptr;
	}

	bool WindowImpl::IsVisible() const
	{
		return m_window != nullptr;
	}

	void WindowImpl::RefreshCursor()
	{
	}

	void WindowImpl::StartTextInput()
	{
	}

	void WindowImpl::StopTextInput()
	{
	}

	void WindowImpl::UpdateCursor(const Cursor& cursor)
	{
	}

	void WindowImpl::RaiseFocus()
	{
	}

	void WindowImpl::UpdateIcon(const Icon& icon)
	{
	}

	void WindowImpl::UpdateMaximumSize(int width, int height)
	{
	}

	void WindowImpl::UpdateMinimumSize(int width, int height)
	{
	}

	void WindowImpl::UpdatePosition(int x, int y)
	{
	}

	void WindowImpl::UpdateRelativeMouseMode(bool relativeMouseMode)
	{
	}

	void WindowImpl::UpdateSize(unsigned int width, unsigned int height)
	{
	}

	void WindowImpl::UpdateStayOnTop(bool stayOnTop)
	{
	}

	void WindowImpl::UpdateTitle(const std::string& title)
	{
	}

	void WindowImpl::Show(bool visible)
	{
	}

	void WindowImpl::ProcessEvents()
	{
		AndroidActivity* activity = AndroidActivity::Instance();
		AInputQueue* inputQueue = activity->GetInputQueue();
		if (!inputQueue)
			return;

		AInputEvent* event;
		while (AInputQueue_getEvent(inputQueue, &event) >= 0)
		{
			if (AInputQueue_preDispatchEvent(inputQueue, event) != 0)
			{
				AInputQueue_finishEvent(inputQueue, event, 0);
				continue;
			}

			CallOnExit eventFinisher([&]{ AInputQueue_finishEvent(inputQueue, event, 1); });

			Int32 eventType = AInputEvent_getType(event);
			switch (eventType)
			{
				case AINPUT_EVENT_TYPE_KEY:
					NazaraWarning("AINPUT_EVENT_TYPE_KEY");
					break;

				case AINPUT_EVENT_TYPE_MOTION:
				{
					WindowImpl* activeWindow = s_activeWindow;
					if (!activeWindow)
						break;

					WindowEvent windowEvent;
					windowEvent.type = WindowEventType::MouseButtonPressed;
					windowEvent.mouseButton.button = Mouse::Left;
					windowEvent.mouseButton.x = AMotionEvent_getX(event, 0);
					windowEvent.mouseButton.y = AMotionEvent_getY(event, 0);
					windowEvent.mouseButton.clickCount = 1;

					activeWindow->m_parent->HandleEvent(windowEvent);
					break;
				}

				case AINPUT_EVENT_TYPE_FOCUS:
					NazaraWarning("AINPUT_EVENT_TYPE_FOCUS");
					break;

				case AINPUT_EVENT_TYPE_CAPTURE:
					NazaraWarning("AINPUT_EVENT_TYPE_CAPTURE");
					break;

				case AINPUT_EVENT_TYPE_DRAG:
					NazaraWarning("AINPUT_EVENT_TYPE_DRAG");
					break;

				default:
					NazaraWarning("unexpected event type {}", eventType);
			}
		}
	}

	bool WindowImpl::Initialize()
	{
		AndroidActivity::Instance()->Poll();
		return true;
	}

	void WindowImpl::Uninitialize()
	{
		if (s_activeWindow)
			NazaraError("WindowImpl uninitialized while a window is still active!");
	}

	WindowImpl* WindowImpl::s_activeWindow = nullptr;
}
