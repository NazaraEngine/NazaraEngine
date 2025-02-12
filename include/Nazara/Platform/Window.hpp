// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_PLATFORM_WINDOW_HPP
#define NAZARA_PLATFORM_WINDOW_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/CursorController.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Export.hpp>
#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/TextInputController.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <Nazara/Platform/WindowEventHandler.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <NazaraUtils/PrivateImpl.hpp>

namespace Nz
{
	class WindowImpl;

	class NAZARA_PLATFORM_API Window
	{
		friend WindowImpl;
		friend class InputImpl;
		friend class MessageBox;
		friend class Mouse;
		friend class Platform;

		public:
			Window();
			inline Window(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default);
			inline explicit Window(WindowHandle handle);
			Window(const Window&) = delete;
			Window(Window&& window) noexcept;
			~Window();

			inline void Close();

			bool Create(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default);
			bool Create(WindowHandle handle);

			void Destroy();

			inline void EnableCloseOnQuit(bool closeOnQuit);

			inline const std::shared_ptr<Cursor>& GetCursor() const;
			inline CursorController& GetCursorController();
			inline WindowEventHandler& GetEventHandler();
			WindowHandle GetHandle() const;
			const Vector2i& GetPosition() const;
			const Vector2ui& GetSize() const;
			WindowStyleFlags GetStyle() const;
			inline TextInputController& GetTextInputController();
			std::string GetTitle() const;

			void HandleEvent(const WindowEvent& event);

			bool HasFocus() const;

			bool IsMinimized() const;
			inline bool IsOpen(bool checkClosed = true);
			inline bool IsOpen() const;
			inline bool IsValid() const;
			bool IsVisible() const;

			void SetCursor(std::shared_ptr<Cursor> cursor);
			inline void SetCursor(SystemCursor systemCursor);
			void SetFocus();
			void SetIcon(std::shared_ptr<Icon> icon);
			void SetMaximumSize(const Vector2i& maxSize);
			void SetMinimumSize(const Vector2i& minSize);
			void SetPosition(const Vector2i& position);
			void SetRelativeMouseMode(bool relativeMouseMode);
			void SetSize(const Vector2i& size);
			void SetStayOnTop(bool stayOnTop);
			void SetTitle(const std::string& title);
			void SetVisible(bool visible);

			void StartTextInput();
			void StopTextInput();

			Window& operator=(const Window&) = delete;
			Window& operator=(Window&& window) noexcept;

			static void ProcessEvents();

		private:
			void ConnectSlots();
			void DisconnectSlots();

			inline WindowImpl* GetImpl();
			inline const WindowImpl* GetImpl() const;

			void IgnoreNextMouseEvent(float mouseX, float mouseY) const;

			static bool Initialize();
			static void Uninitialize();

			NazaraSlot(CursorController, OnCursorUpdated, m_cursorUpdateSlot);
			NazaraSlot(TextInputController, OnTextInputStarted, m_textInputStarted);
			NazaraSlot(TextInputController, OnTextInputStopped, m_textInputStopped);

			std::shared_ptr<Cursor> m_cursor;
			std::shared_ptr<Icon> m_icon;
			std::unique_ptr<WindowImpl> m_impl;
			CursorController m_cursorController;
			TextInputController m_textInputController;
			Vector2i m_position;
			Vector2ui m_size;
			WindowEventHandler m_eventHandler;
			bool m_closed;
			bool m_closeOnQuit;
			bool m_ownsWindow;
			bool m_waitForEvent;
	};
}

#include <Nazara/Platform/Window.inl>

#endif // NAZARA_PLATFORM_WINDOW_HPP
