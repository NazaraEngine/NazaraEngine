// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOW_HPP
#define NAZARA_WINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/EventHandler.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace Nz
{
	class WindowImpl;

	class NAZARA_PLATFORM_API Window
	{
		friend WindowImpl;
		friend class EventImpl;
		friend class Mouse;
		friend class Platform;

		public:
			Window();
			inline Window(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default);
			inline explicit Window(void* handle);
			Window(const Window&) = delete;
			Window(Window&& window);
			virtual ~Window();

			inline void Close();

			bool Create(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default);
			bool Create(void* handle);

			void Destroy();

			inline void EnableCloseOnQuit(bool closeOnQuit);

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			inline void EnableEventPolling(bool enable);

			void EnableKeyRepeat(bool enable);
			void EnableSmoothScrolling(bool enable);

			inline const std::shared_ptr<Cursor>& GetCursor() const;
			inline CursorController& GetCursorController();
			inline EventHandler& GetEventHandler();
			Vector2i GetPosition() const;
			Vector2ui GetSize() const;
			WindowStyleFlags GetStyle() const;
			WindowHandle GetSystemHandle() const;
			std::string GetTitle() const;

			bool HasFocus() const;

			bool IsMinimized() const;
			inline bool IsOpen(bool checkClosed = true);
			inline bool IsOpen() const;
			inline bool IsValid() const;
			bool IsVisible() const;

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			bool PollEvent(WindowEvent* event);

			void PushEvent(const WindowEvent& event);

			void ProcessEvents(bool block = false);

			void SetCursor(std::shared_ptr<Cursor> cursor);
			inline void SetCursor(SystemCursor systemCursor);
			void SetEventListener(bool listener);
			void SetFocus();
			void SetIcon(std::shared_ptr<Icon> icon);
			void SetMaximumSize(const Vector2i& maxSize);
			void SetMaximumSize(int width, int height);
			void SetMinimumSize(const Vector2i& minSize);
			void SetMinimumSize(int width, int height);
			void SetPosition(const Vector2i& position);
			void SetPosition(int x, int y);
			void SetSize(const Vector2i& size);
			void SetSize(unsigned int width, unsigned int height);
			void SetStayOnTop(bool stayOnTop);
			void SetTitle(const std::string& title);
			void SetVisible(bool visible);

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			bool WaitEvent(WindowEvent* event);

			Window& operator=(const Window&) = delete;
			Window& operator=(Window&& window);

		protected:
			void* GetHandle();

			virtual bool OnWindowCreated();
			virtual void OnWindowDestroy();
			virtual void OnWindowResized();

			MovablePtr<WindowImpl> m_impl;

		private:
			void ConnectSlots();
			void DisconnectSlots();

			inline WindowImpl* GetImpl();
			inline const WindowImpl* GetImpl() const;

			void IgnoreNextMouseEvent(int mouseX, int mouseY) const;
			void HandleEvent(const WindowEvent& event);

			static bool Initialize();
			static void Uninitialize();

			NazaraSlot(CursorController, OnCursorUpdated, m_cursorUpdateSlot);

			std::queue<WindowEvent> m_events;
			std::vector<WindowEvent> m_pendingEvents;
			std::condition_variable m_eventCondition;
			CursorController m_cursorController;
			std::shared_ptr<Cursor> m_cursor;
			EventHandler m_eventHandler;
			std::shared_ptr<Icon> m_icon;
			std::mutex m_eventMutex;
			std::mutex m_eventConditionMutex;
			bool m_asyncWindow;
			bool m_closed;
			bool m_closeOnQuit;
			bool m_eventPolling;
			bool m_ownsWindow;
			bool m_waitForEvent;
	};
}

#include <Nazara/Platform/Window.inl>

#endif // NAZARA_WINDOW_HPP
