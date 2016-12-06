// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOW_HPP
#define NAZARA_WINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/EventHandler.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/WindowHandle.hpp>
#include <queue>

namespace Nz
{
	class Cursor;
	class Image;
	class Icon;
	class WindowImpl;

	class NAZARA_UTILITY_API Window
	{
		friend WindowImpl;
		friend class Mouse;
		friend class Utility;

		public:
			inline Window();
			inline Window(VideoMode mode, const String& title, WindowStyleFlags style = WindowStyle_Default);
			inline Window(WindowHandle handle);
			Window(const Window&) = delete;
			inline Window(Window&& window) noexcept;
			virtual ~Window();

			inline void Close();

			bool Create(VideoMode mode, const String& title, WindowStyleFlags style = WindowStyle_Default);
			bool Create(WindowHandle handle);

			void Destroy();

			inline void EnableCloseOnQuit(bool closeOnQuit);

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			inline void EnableEventPolling(bool enable);

			void EnableKeyRepeat(bool enable);
			void EnableSmoothScrolling(bool enable);

			EventHandler& GetEventHandler();
			WindowHandle GetHandle() const;
			unsigned int GetHeight() const;
			Vector2i GetPosition() const;
			Vector2ui GetSize() const;
			WindowStyleFlags GetStyle() const;
			String GetTitle() const;
			unsigned int GetWidth() const;

			bool HasFocus() const;

			bool IsMinimized() const;
			inline bool IsOpen(bool checkClosed = true);
			inline bool IsOpen() const;
			inline bool IsValid() const;
			bool IsVisible() const;

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			bool PollEvent(WindowEvent* event);

			void ProcessEvents(bool block = false);

			void SetCursor(WindowCursor cursor);
			void SetCursor(const Cursor& cursor);
			void SetEventListener(bool listener);
			void SetFocus();
			void SetIcon(const Icon& icon);
			void SetMaximumSize(const Vector2i& maxSize);
			void SetMaximumSize(int width, int height);
			void SetMinimumSize(const Vector2i& minSize);
			void SetMinimumSize(int width, int height);
			void SetPosition(const Vector2i& position);
			void SetPosition(int x, int y);
			void SetSize(const Vector2i& size);
			void SetSize(unsigned int width, unsigned int height);
			void SetStayOnTop(bool stayOnTop);
			void SetTitle(const String& title);
			void SetVisible(bool visible);

			NAZARA_DEPRECATED("Event pooling/waiting is deprecated, please use the EventHandler system")
			bool WaitEvent(WindowEvent* event);

			Window& operator=(const Window&) = delete;
			inline Window& operator=(Window&& window);

		protected:
			virtual bool OnWindowCreated();
			virtual void OnWindowDestroy();
			virtual void OnWindowResized();

			WindowImpl* m_impl;

		private:
			void IgnoreNextMouseEvent(int mouseX, int mouseY) const;
			inline void HandleEvent(const WindowEvent& event);
			inline void PushEvent(const WindowEvent& event);

			static bool Initialize();
			static void Uninitialize();

			std::queue<WindowEvent> m_events;
			std::vector<WindowEvent> m_pendingEvents;
			ConditionVariable m_eventCondition;
			EventHandler m_eventHandler;
			Mutex m_eventMutex;
			Mutex m_eventConditionMutex;
			bool m_asyncWindow;
			bool m_closed;
			bool m_closeOnQuit;
			bool m_eventPolling;
			bool m_ownsWindow;
			bool m_waitForEvent;
	};
}

#include <Nazara/Utility/Window.inl>

#endif // NAZARA_WINDOW_HPP
