// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOWIMPL_HPP
#define NAZARA_WINDOWIMPL_HPP

#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/Window.hpp>
#include <windows.h>

#if NAZARA_UTILITY_THREADED_WINDOW
class NzConditionVariable;
class NzMutex;
class NzThread;
#endif
class NzWindow;

#undef IsMinimized // Conflit avec la méthode du même nom

class NzWindowImpl : NzNonCopyable
{
	public:
		NzWindowImpl(NzWindow* parent);
		~NzWindowImpl() = default;

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style);
		bool Create(NzWindowHandle handle);

		void Destroy();

		void EnableKeyRepeat(bool enable);
		void EnableSmoothScrolling(bool enable);

		NzWindowHandle GetHandle() const;
		unsigned int GetHeight() const;
		NzVector2i GetPosition() const;
		NzVector2ui GetSize() const;
		NzString GetTitle() const;
		unsigned int GetWidth() const;

		bool HasFocus() const;

		void IgnoreNextMouseEvent(int mouseX, int mouseY);

		bool IsMinimized() const;
		bool IsVisible() const;

		void ProcessEvents(bool block);

		void SetCursor(nzWindowCursor cursor);
		void SetCursor(const NzCursor& cursor);
		void SetEventListener(bool listener);
		void SetFocus();
		void SetIcon(const NzIcon& icon);
		void SetMaximumSize(int width, int height);
		void SetMinimumSize(int width, int height);
		void SetPosition(int x, int y);
		void SetSize(unsigned int width, unsigned int height);
		void SetStayOnTop(bool stayOnTop);
		void SetTitle(const NzString& title);
		void SetVisible(bool visible);

		static bool Initialize();
		static void Uninitialize();

	private:
		bool HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
		static NzKeyboard::Key ConvertVirtualKey(WPARAM key, LPARAM flags);
		#if NAZARA_UTILITY_THREADED_WINDOW
		static void WindowThread(HWND* handle, DWORD styleEx, const wchar_t* title, DWORD style, unsigned int x, unsigned int y, unsigned int width, unsigned int height, NzWindowImpl* window, NzMutex* mutex, NzConditionVariable* condition);
		#endif

		HCURSOR m_cursor;
		HWND m_handle;
		LONG_PTR m_callback;
		NzVector2i m_maxSize;
		NzVector2i m_minSize;
		NzVector2i m_mousePos;
		#if !NAZARA_UTILITY_THREADED_WINDOW
		NzVector2i m_position;
		NzVector2ui m_size;
		#else
		NzThread* m_thread;
		#endif
		NzWindow* m_parent;
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
#endif // NAZARA_WINDOWIMPL_HPP
