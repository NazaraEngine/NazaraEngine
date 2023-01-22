// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_PLATFORM_SDL2_WINDOWIMPL_HPP
#define NAZARA_PLATFORM_SDL2_WINDOWIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/Window.hpp>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_video.h>
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

			void IgnoreNextMouseEvent(int mouseX, int mouseY);

			bool IsMinimized() const;
			bool IsVisible() const;

			void RaiseFocus();
			void RefreshCursor();

			void UpdateCursor(const Cursor& cursor);
			void UpdateIcon(const Icon& icon);
			void UpdateMaximumSize(int width, int height);
			void UpdateMinimumSize(int width, int height);
			void UpdatePosition(int x, int y);
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
			void SetEventListener(bool listener);

			static int SDLCALL HandleEvent(void* userdata, SDL_Event* event);

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

#endif // NAZARA_PLATFORM_SDL2_WINDOWIMPL_HPP
