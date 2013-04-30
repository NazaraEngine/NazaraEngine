// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
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
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Event.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/WindowHandle.hpp>
#include <queue>

#if NAZARA_UTILITY_THREADED_WINDOW
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Mutex.hpp>
#endif

class NzCursor;
class NzImage;
class NzIcon;
class NzWindowImpl;

class NAZARA_API NzWindow : NzNonCopyable
{
	friend NzWindowImpl;
	friend class NzMouse;
	friend class NzUtility;

	public:
		NzWindow();
		NzWindow(NzVideoMode mode, const NzString& title, nzUInt32 style = nzWindowStyle_Default);
		NzWindow(NzWindowHandle handle);
		virtual ~NzWindow();

		void Close();

		bool Create(NzVideoMode mode, const NzString& title, nzUInt32 style = nzWindowStyle_Default);
		bool Create(NzWindowHandle handle);

		void Destroy();

		void EnableKeyRepeat(bool enable);
		void EnableSmoothScrolling(bool enable);

		NzWindowHandle GetHandle() const;
		unsigned int GetHeight() const;
		NzVector2i GetPosition() const;
		NzVector2ui GetSize() const;
		nzUInt32 GetStyle() const;
		NzString GetTitle() const;
		unsigned int GetWidth() const;

		bool HasFocus() const;

		bool IsMinimized() const;
		bool IsOpen(bool checkClosed = true);
		bool IsOpen() const;
		bool IsValid() const;
		bool IsVisible() const;

		bool PollEvent(NzEvent* event);

		void SetCursor(nzWindowCursor cursor);
		void SetCursor(const NzCursor& cursor);
		void SetEventListener(bool listener);
		void SetFocus();
		void SetIcon(const NzIcon& icon);
		void SetMaximumSize(const NzVector2i& maxSize);
		void SetMaximumSize(int width, int height);
		void SetMinimumSize(const NzVector2i& minSize);
		void SetMinimumSize(int width, int height);
		void SetPosition(const NzVector2i& position);
		void SetPosition(int x, int y);
		void SetSize(const NzVector2i& size);
		void SetSize(unsigned int width, unsigned int height);
		void SetStayOnTop(bool stayOnTop);
		void SetTitle(const NzString& title);
		void SetVisible(bool visible);

		bool WaitEvent(NzEvent* event);

	protected:
		virtual bool OnWindowCreated();
		virtual void OnWindowDestroy();

		NzWindowImpl* m_impl;

	private:
		void IgnoreNextMouseEvent(int mouseX, int mouseY) const;
		void PushEvent(const NzEvent& event);

		static bool Initialize();
		static void Uninitialize();

		std::queue<NzEvent> m_events;
		#if NAZARA_UTILITY_THREADED_WINDOW
		NzConditionVariable m_eventCondition;
		NzMutex m_eventMutex;
		NzMutex m_eventConditionMutex;
		bool m_eventListener;
		bool m_waitForEvent;
		#endif
		bool m_closed;
		bool m_ownsWindow;
};

#endif // NAZARA_WINDOW_HPP
