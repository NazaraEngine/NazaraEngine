// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOW_HPP
#define NAZARA_WINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Event.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/WindowHandle.hpp>
#include <queue>

#if NAZARA_UTILITY_THREADED_WINDOW
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Mutex.hpp>
#endif

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
			Window();
			Window(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default);
			Window(WindowHandle handle);
			Window(const Window&) = delete;
			Window(Window&&) = delete; ///TODO
			virtual ~Window();

			void Close();

			bool Create(VideoMode mode, const String& title, UInt32 style = WindowStyle_Default);
			bool Create(WindowHandle handle);

			void Destroy();

			void EnableKeyRepeat(bool enable);
			void EnableSmoothScrolling(bool enable);

			WindowHandle GetHandle() const;
			unsigned int GetHeight() const;
			Vector2i GetPosition() const;
			Vector2ui GetSize() const;
			UInt32 GetStyle() const;
			String GetTitle() const;
			unsigned int GetWidth() const;

			bool HasFocus() const;

			bool IsMinimized() const;
			bool IsOpen(bool checkClosed = true);
			bool IsOpen() const;
			bool IsValid() const;
			bool IsVisible() const;

			bool PollEvent(WindowEvent* event);

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

			bool WaitEvent(WindowEvent* event);

			Window& operator=(const Window&) = delete;
			Window& operator=(Window&&) = delete; ///TODO

		protected:
			virtual bool OnWindowCreated();
			virtual void OnWindowDestroy();
			virtual void OnWindowResized();

			WindowImpl* m_impl;

		private:
			void IgnoreNextMouseEvent(int mouseX, int mouseY) const;
			void PushEvent(const WindowEvent& event);

			static bool Initialize();
			static void Uninitialize();

			std::queue<WindowEvent> m_events;
			#if NAZARA_UTILITY_THREADED_WINDOW
			ConditionVariable m_eventCondition;
			Mutex m_eventMutex;
			Mutex m_eventConditionMutex;
			bool m_eventListener;
			bool m_waitForEvent;
			#endif
			bool m_closed;
			bool m_ownsWindow;
	};
}

#endif // NAZARA_WINDOW_HPP
