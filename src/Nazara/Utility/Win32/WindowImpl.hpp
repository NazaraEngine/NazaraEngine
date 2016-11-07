// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOWIMPL_HPP
#define NAZARA_WINDOWIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/Window.hpp>
#include <windows.h>

namespace Nz
{
	#if NAZARA_UTILITY_THREADED_WINDOW
	class ConditionVariable;
	class Mutex;
	#endif
	class Window;

	#undef IsMinimized // Conflit avec la méthode du même nom

	class WindowImpl
	{
		public:
			WindowImpl(Window* parent);
			WindowImpl(const WindowImpl&) = delete;
			WindowImpl(WindowImpl&&) = delete; ///TODO?
			~WindowImpl() = default;

			bool Create(const VideoMode& mode, const String& title, UInt32 style);
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

			void IgnoreNextMouseEvent(int mouseX, int mouseY);

			bool IsMinimized() const;
			bool IsVisible() const;

			void ProcessEvents(bool block);

			void SetCursor(WindowCursor cursor);
			void SetCursor(const Cursor& cursor);
			void SetEventListener(bool listener);
			void SetFocus();
			void SetIcon(const Icon& icon);
			void SetMaximumSize(int width, int height);
			void SetMinimumSize(int width, int height);
			void SetPosition(int x, int y);
			void SetSize(unsigned int width, unsigned int height);
			void SetStayOnTop(bool stayOnTop);
			void SetTitle(const String& title);
			void SetVisible(bool visible);

			WindowImpl& operator=(const WindowImpl&) = delete;
			WindowImpl& operator=(WindowImpl&&) = delete; ///TODO?

			static bool Initialize();
			static void Uninitialize();

		private:
			bool HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

			static Keyboard::Key ConvertVirtualKey(WPARAM key, LPARAM flags);
			static LRESULT CALLBACK MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
			static UInt32 RetrieveStyle(HWND window);
			#if NAZARA_UTILITY_THREADED_WINDOW
			static void WindowThread(HWND* handle, DWORD styleEx, const String& title, DWORD style, unsigned int x, unsigned int y, unsigned int width, unsigned int height, WindowImpl* window, Mutex* mutex, ConditionVariable* condition);
			#endif

			HCURSOR m_cursor;
			HWND m_handle;
			LONG_PTR m_callback;
			UInt32 m_style;
			Vector2i m_maxSize;
			Vector2i m_minSize;
			Vector2i m_mousePos;
			Vector2i m_position;
			Vector2ui m_size;
			#if NAZARA_UTILITY_THREADED_WINDOW
			Thread m_thread;
			#endif
			Window* m_parent;
			bool m_eventListener;
			bool m_keyRepeat;
			bool m_mouseInside;
			bool m_ownsWindow;
			#if !NAZARA_UTILITY_THREADED_WINDOW
			bool m_sizemove;
			#endif
			bool m_smoothScrolling;
			#if NAZARA_UTILITY_THREADED_WINDOW
			bool m_threadActive;
			#endif
			short m_scrolling;
	};
}

#endif // NAZARA_WINDOWIMPL_HPP
