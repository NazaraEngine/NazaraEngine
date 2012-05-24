// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_WINDOW_HPP
#define NAZARA_WINDOW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Event.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/WindowHandle.hpp>
#include <queue>

#if NAZARA_UTILITY_THREADED_WINDOW
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/ThreadCondition.hpp>
#endif

class NzWindowImpl;

class NAZARA_API NzWindow : NzNonCopyable
{
	friend class NzWindowImpl;

	public:
		enum Style
		{
			None	   = 0x0,
			Fullscreen = 0x1,

			Closable  = 0x2,
			Resizable = 0x4,
			Titlebar  = 0x8,

			Default = Closable | Resizable | Titlebar
		};

		NzWindow();
		NzWindow(NzVideoMode mode, const NzString& title, nzUInt32 style = Default);
		NzWindow(NzWindowHandle handle);
		virtual ~NzWindow();

		void Close();

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style = Default);
		bool Create(NzWindowHandle handle);

		void EnableKeyRepeat(bool enable);
		void EnableSmoothScrolling(bool enable);

		NzWindowHandle GetHandle() const;
		unsigned int GetHeight() const;
		NzVector2i GetPosition() const;
		NzVector2i GetSize() const;
		NzString GetTitle() const;
		unsigned int GetWidth() const;

		bool HasFocus() const;

		bool IsMinimized() const;
		bool IsOpen() const;
		bool IsVisible() const;

		bool PollEvent(NzEvent* event);

		void SetEventListener(bool listener);
		void SetFocus();
		void SetMaximumSize(const NzVector2i& maxSize);
		void SetMaximumSize(int width, int height);
		void SetMinimumSize(const NzVector2i& minSize);
		void SetMinimumSize(int width, int height);
		void SetPosition(const NzVector2i& position);
		void SetPosition(int x, int y);
		void SetSize(const NzVector2i& size);
		void SetSize(unsigned int width, unsigned int height);
		void SetTitle(const NzString& title);
		void SetVisible(bool visible);

		void ShowMouseCursor(bool show);

		void StayOnTop(bool stayOnTop);

		bool WaitEvent(NzEvent* event);

	protected:
		virtual void OnClose();
		virtual bool OnCreate();

		NzWindowImpl* m_impl;

	private:
		void PushEvent(const NzEvent& event);

		std::queue<NzEvent> m_events;
		#if NAZARA_UTILITY_THREADED_WINDOW
		NzMutex m_eventMutex;
		NzMutex m_eventConditionMutex;
		NzThreadCondition m_eventCondition;
		bool m_eventListener;
		bool m_waitForEvent;
		#endif
		bool m_ownsWindow;
};

#endif // NAZARA_WINDOW_HPP
