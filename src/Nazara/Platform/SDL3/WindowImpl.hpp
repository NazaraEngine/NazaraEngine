// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_PLATFORM_SDL3_WINDOWIMPL_HPP
#define NAZARA_PLATFORM_SDL3_WINDOWIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Export.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/Window.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_video.h>
#include <mutex>
#include <string>

namespace Nz
{
	class Window;

	class WindowImpl
	{
		public:
			WindowImpl(Window* parent);
			WindowImpl(const WindowImpl&) = delete;
			WindowImpl(WindowImpl&&) = delete; ///TODO?
			~WindowImpl() = default;

			bool Create(const VideoMode& mode, const std::string& title, WindowStyleFlags style);
			bool Create(WindowHandle handle);

			void Destroy();

			Vector2i FetchPosition() const;
			Vector2ui FetchSize() const;
			WindowStyleFlags FetchStyle() const;
			std::string FetchTitle() const;

			SDL_Window* GetHandle() const;
			WindowHandle GetSystemHandle() const;

			bool HasFocus() const;

			void IgnoreNextMouseEvent(float mouseX, float mouseY);

			bool IsMinimized() const;
			bool IsVisible() const;

			void RaiseFocus();
			void RefreshCursor();

			void StartTextInput();
			void StopTextInput();

			void UpdateCursor(const Cursor& cursor);
			void UpdateIcon(const Icon& icon);
			void UpdateMaximumSize(int width, int height);
			void UpdateMinimumSize(int width, int height);
			void UpdatePosition(int x, int y);
			void UpdateRelativeMouseMode(bool relativeMouseMode);
			void UpdateSize(unsigned int width, unsigned int height);
			void UpdateStayOnTop(bool stayOnTop);
			void UpdateTitle(const std::string& title);

			void Show(bool visible);

			WindowImpl& operator=(const WindowImpl&) = delete;
			WindowImpl& operator=(WindowImpl&&) = delete; ///TODO?

			static void ProcessEvents();
			static bool Initialize();
			static void Uninitialize();

		private:
			struct EventData;

			EventData* AllocateEventData();
			void FreeEventData(EventData* eventData);
			void HandleEvent(SDL_Event* event);
			void SetEventListener(bool listener);

			static bool SDLCALL HandleEvent(void* userdata, SDL_Event* event);

			struct EventData
			{
				std::size_t poolIndex;
				WindowImpl* window;
				SDL_Event event;
			};

			std::mutex m_eventDataPoolMutex;
			MemoryPool<EventData> m_eventDataPool;
			UInt32 m_windowId;
			SDL_Cursor* m_cursor;
			SDL_Window* m_handle;
			Window* m_parent;
			bool m_eventListener;
			bool m_ignoreNextMouseMove;
			bool m_ownsWindow;
			int m_lastEditEventLength;
	};
}

#endif // NAZARA_PLATFORM_SDL3_WINDOWIMPL_HPP
