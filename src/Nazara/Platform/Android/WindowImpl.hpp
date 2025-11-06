// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_ANDROID_WINDOWIMPL_HPP
#define NAZARA_PLATFORM_ANDROID_WINDOWIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Android/AndroidActivity.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Export.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/Window.hpp>
#include <android/native_window.h>
#include <string>

namespace Nz
{
	class Window;

	class WindowImpl
	{
		public:
			WindowImpl(Window* parent);
			WindowImpl(const WindowImpl&) = delete;
			WindowImpl(WindowImpl&&) = delete;
			~WindowImpl() = default;

			bool Create(const VideoMode& mode, const std::string& title, WindowStyleFlags style);
			bool Create(WindowHandle handle);

			void Destroy();

			Vector2i FetchPosition() const;
			Vector2ui FetchSize() const;
			WindowStyleFlags FetchStyle() const;
			std::string FetchTitle() const;

			ANativeWindow* GetHandle() const;
			WindowHandle GetSystemHandle() const;

			bool HasFocus() const;

			void IgnoreNextMouseEvent(int mouseX, int mouseY);

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
			WindowImpl& operator=(WindowImpl&&) = delete;

			static void ProcessEvents();
			static bool Initialize();
			static void Uninitialize();

		private:
			NazaraSlot(AndroidActivity, OnWindowCreated, m_onWindowCreated);
			NazaraSlot(AndroidActivity, OnWindowDestroyed, m_onWindowDestroyed);
			NazaraSlot(AndroidActivity, OnWindowResized, m_onWindowResized);

			ANativeWindow* m_window;
			Window* m_parent;
			bool m_created;
			unsigned int m_height;
			unsigned int m_width;

			static WindowImpl* s_activeWindow;
	};
}

#endif // NAZARA_PLATFORM_ANDROID_WINDOWIMPL_HPP
