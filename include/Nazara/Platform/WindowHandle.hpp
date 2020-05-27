// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWHANDLE_HPP
#define NAZARA_WINDOWHANDLE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	enum class WindowManager
	{
		None,

		X11,
		Wayland,
		Windows
	};

	struct WindowHandle
	{
		WindowManager type = WindowManager::None;

		union
		{
			struct
			{
				void* display; //< Display*
				void* window;  //< Window
			} x11;

			struct 
			{
				void* display;      //< wl_display*
				void* surface;      //< wl_surface*
				void* shellSurface; //< wl_shell_surface*
			} wayland;

			struct
			{
				void* window; //< HWND
			} windows;
		};
	};
}

#endif // NAZARA_WINDOWHANDLE_HPP
