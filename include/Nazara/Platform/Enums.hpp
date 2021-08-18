// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_PLATFORM_HPP
#define NAZARA_ENUMS_PLATFORM_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class SystemCursor
	{
		Crosshair,
		Default,
		Hand,
		Help,
		Move,
		None,
		Pointer,
		Progress,
		ResizeE,
		ResizeN,
		ResizeNE,
		ResizeNW,
		ResizeS,
		ResizeSE,
		ResizeSW,
		ResizeW,
		Text,
		Wait,

		Max = Wait
	};

	constexpr std::size_t SystemCursorCount = static_cast<std::size_t>(SystemCursor::Max) + 1;

	enum class WindowEventType
	{
		GainedFocus,
		LostFocus,
		KeyPressed,
		KeyReleased,
		MouseButtonDoubleClicked,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseEntered,
		MouseLeft,
		MouseMoved,
		MouseWheelMoved,
		Moved,
		Quit,
		Resized,
		TextEdited,
		TextEntered,

		Max = TextEntered
	};

	constexpr std::size_t WindowEventTypeCount = static_cast<std::size_t>(WindowEventType::Max) + 1;

	enum class WindowStyle
	{
		None,       ///< Window has no border nor titlebar.
		Fullscreen, ///< At the window creation, the OS tries to set it in fullscreen.

		Closable,   ///< Allows the window to be closed by a button in the titlebar, generating a Quit event.
		Resizable,  ///< Allows the window to be resized by dragging its corners or by a button of the titlebar.
		Titlebar,   ///< Adds a titlebar to the window, this option is automatically enabled if buttons of the titlebar are enabled.

		Threaded,   ///< Runs the window into a thread, allowing the application to keep updating while resizing/dragging the window.

		Max = Threaded
	};

	constexpr std::size_t WindowStyleCount = static_cast<std::size_t>(WindowStyle::Max) + 1;

	template<>
	struct EnumAsFlags<WindowStyle>
	{
		static constexpr WindowStyle max = WindowStyle::Max;
	};

	using WindowStyleFlags = Flags<WindowStyle>;

	constexpr WindowStyleFlags WindowStyle_Default = WindowStyle::Closable | WindowStyle::Resizable | WindowStyle::Titlebar;
}

#endif // NAZARA_ENUMS_PLATFORM_HPP
