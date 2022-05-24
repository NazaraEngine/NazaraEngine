// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_EVENTHANDLER_HPP
#define NAZARA_PLATFORM_EVENTHANDLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Event.hpp>
#include <Nazara/Utils/Signal.hpp>

namespace Nz
{
	class EventHandler;

	using EventHandlerHandle = ObjectHandle<EventHandler>;

	class EventHandler : public HandledObject<EventHandler>
	{
		public:
			EventHandler() = default;
			explicit EventHandler(const EventHandler&);
			EventHandler(EventHandler&&) noexcept = default;
			~EventHandler() = default;

			inline void Dispatch(const WindowEvent& event);

			EventHandler& operator=(const EventHandler&) = delete;
			EventHandler& operator=(EventHandler&&) noexcept = default;

			NazaraSignal(OnEvent, const EventHandler* /*eventHandler*/, const WindowEvent& /*event*/);
			NazaraSignal(OnGainedFocus, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnLostFocus, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnKeyPressed, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnKeyReleased, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnMouseButtonPressed, const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseButtonReleased, const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseEntered, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseLeft, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseMoved, const EventHandler* /*eventHandler*/, const WindowEvent::MouseMoveEvent& /*event*/);
			NazaraSignal(OnMouseWheelMoved, const EventHandler* /*eventHandler*/, const WindowEvent::MouseWheelEvent& /*event*/);
			NazaraSignal(OnMoved, const EventHandler* /*eventHandler*/, const WindowEvent::PositionEvent& /*event*/);
			NazaraSignal(OnQuit, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnResized, const EventHandler* /*eventHandler*/, const WindowEvent::SizeEvent& /*event*/);
			NazaraSignal(OnTextEntered, const EventHandler* /*eventHandler*/, const WindowEvent::TextEvent& /*event*/);
			NazaraSignal(OnTextEdited, const EventHandler* /*eventHandler*/, const WindowEvent::EditEvent& /*event*/);
	};
}

#include <Nazara/Platform/EventHandler.inl>

#endif // NAZARA_PLATFORM_EVENTHANDLER_HPP
