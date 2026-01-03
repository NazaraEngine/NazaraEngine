// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_WINDOWEVENTHANDLER_HPP
#define NAZARA_PLATFORM_WINDOWEVENTHANDLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Platform/Export.hpp>
#include <Nazara/Platform/WindowEvent.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class WindowEventHandler;

	using EventHandlerHandle = ObjectHandle<WindowEventHandler>;

	class WindowEventHandler : public HandledObject<WindowEventHandler>
	{
		public:
			WindowEventHandler() = default;
			explicit WindowEventHandler(const WindowEventHandler&);
			WindowEventHandler(WindowEventHandler&&) noexcept = default;
			~WindowEventHandler() = default;

			inline void Dispatch(const WindowEvent& event);

			WindowEventHandler& operator=(const WindowEventHandler&) = delete;
			WindowEventHandler& operator=(WindowEventHandler&&) noexcept = default;

			NazaraSignal(OnCreated, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnDestruction, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnEvent, const WindowEventHandler* /*eventHandler*/, const WindowEvent& /*event*/);
			NazaraSignal(OnGainedFocus, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnLostFocus, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnKeyPressed, const WindowEventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnKeyReleased, const WindowEventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnMinimized, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseButtonPressed, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseButtonReleased, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseEntered, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseLeft, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseMoved, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseMoveEvent& /*event*/);
			NazaraSignal(OnMouseWheelMoved, const WindowEventHandler* /*eventHandler*/, const WindowEvent::MouseWheelEvent& /*event*/);
			NazaraSignal(OnMoved, const WindowEventHandler* /*eventHandler*/, const WindowEvent::PositionEvent& /*event*/);
			NazaraSignal(OnQuit, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnResized, const WindowEventHandler* /*eventHandler*/, const WindowEvent::SizeEvent& /*event*/);
			NazaraSignal(OnRestored, const WindowEventHandler* /*eventHandler*/);
			NazaraSignal(OnTextEntered, const WindowEventHandler* /*eventHandler*/, const WindowEvent::TextEvent& /*event*/);
			NazaraSignal(OnTextEdited, const WindowEventHandler* /*eventHandler*/, const WindowEvent::EditEvent& /*event*/);
	};
}

#include <Nazara/Platform/WindowEventHandler.inl>

#endif // NAZARA_PLATFORM_WINDOWEVENTHANDLER_HPP
