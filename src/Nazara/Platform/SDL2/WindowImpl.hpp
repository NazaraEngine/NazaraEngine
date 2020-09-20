// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOWIMPL_HPP
#define NAZARA_WINDOWIMPL_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Prerequisites.hpp>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_video.h>

namespace Nz
{
	class ConditionVariable;
	class Mutex;
	class Window;

	class WindowImpl
	{
		public:
			WindowImpl(Window* parent);
			WindowImpl(const WindowImpl&) = delete;
			WindowImpl(WindowImpl&&) = delete; ///TODO?
			~WindowImpl() = default;

			bool Create(const VideoMode& mode, const String& title, WindowStyleFlags style);
			bool Create(void* handle);

			void Destroy();

			void EnableKeyRepeat(bool enable);
			void EnableSmoothScrolling(bool enable);

			SDL_Window* GetHandle() const;
			Vector2i GetPosition() const;
			Vector2ui GetSize() const;
			WindowStyleFlags GetStyle() const;
			WindowHandle GetSystemHandle() const;
			String GetTitle() const;

			bool HasFocus() const;

			void IgnoreNextMouseEvent(int mouseX, int mouseY);

			bool IsMinimized() const;
			bool IsVisible() const;

			void RefreshCursor();

			void ProcessEvents(bool block);

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
			int static SDLCALL HandleEvent(void *userdata, SDL_Event * event);

			void PrepareWindow(bool fullscreen);

			//static void WindowThread(SDL_Window* handle, /*DWORD styleEx,*/ const String& title, /*DWORD style,*/ bool fullscreen, const Rectui& dimensions, WindowImpl* window, Mutex* mutex, ConditionVariable* condition);

			int m_lastEditEventLength = 0;
			SDL_Cursor* m_cursor;
			SDL_Window* m_handle;
			WindowStyleFlags m_style;
			Vector2i m_maxSize;
			Vector2i m_minSize;
			Vector2i m_mousePos;
			Vector2i m_position;
			Vector2ui m_size;
			//Thread m_thread;
			Window* m_parent;
			bool m_eventListener;
			bool m_ignoreNextMouseMove = false;
			bool m_keyRepeat;
			bool m_mouseInside;
			bool m_ownsWindow;
			bool m_sizemove;
			bool m_smoothScrolling;
			bool m_threadActive;
			short m_scrolling;
	};
}

#endif // NAZARA_WINDOWIMPL_HPP
