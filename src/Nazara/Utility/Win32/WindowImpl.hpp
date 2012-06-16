// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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
class NzMutex;
class NzThread;
class NzThreadCondition;
#endif
class NzWindow;

#undef IsMinimized // Conflit avec la méthode du même nom

class NzWindowImpl : NzNonCopyable
{
	public:
		NzWindowImpl(NzWindow* parent);
		~NzWindowImpl() = default;

		void Close();

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style);
		bool Create(NzWindowHandle handle);

		void EnableKeyRepeat(bool enable);
		void EnableSmoothScrolling(bool enable);

		NzWindowHandle GetHandle() const;
		unsigned int GetHeight() const;
		NzVector2i GetPosition() const;
		NzVector2ui GetSize() const;
		NzString GetTitle() const;
		unsigned int GetWidth() const;

		bool HasFocus() const;

		void ProcessEvents(bool block);

		bool IsMinimized() const;
		bool IsVisible() const;

		void SetEventListener(bool listener);
		void SetFocus();
		void SetMaximumSize(int width, int height);
		void SetMinimumSize(int width, int height);
		void SetPosition(int x, int y);
		void SetSize(unsigned int width, unsigned int height);
		void SetTitle(const NzString& title);
		void SetVisible(bool visible);

		void ShowMouseCursor(bool show);
		void StayOnTop(bool stayOnTop);

		static bool Initialize();
		static void Uninitialize();

	private:
		bool HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
		static NzKeyboard::Key ConvertVirtualKey(WPARAM key, LPARAM flags);
		#if NAZARA_UTILITY_THREADED_WINDOW
		static void WindowThread(HWND* handle, DWORD styleEx, const wchar_t* title, DWORD style, unsigned int x, unsigned int y, unsigned int width, unsigned int height, NzWindowImpl* window, NzMutex* mutex, NzThreadCondition* condition);
		#endif

		HCURSOR m_cursor;
		HWND m_handle;
		LONG_PTR m_callback;
		NzVector2i m_maxSize;
		NzVector2i m_minSize;
		#if NAZARA_UTILITY_THREADED_WINDOW
		NzThread* m_thread;
		#endif
		NzWindow* m_parent;
		bool m_eventListener;
		bool m_keyRepeat;
		bool m_mouseInside;
		bool m_ownsWindow;
		bool m_smoothScrolling;
		#if NAZARA_UTILITY_THREADED_WINDOW
		bool m_threadActive;
		#endif
		short m_scrolling;
};
#endif // NAZARA_WINDOWIMPL_HPP
