// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWHANDLE_HPP
#define NAZARA_WINDOWHANDLE_HPP

#include <Nazara/Prerequisites.hpp>
#if defined(NAZARA_PLATFORM_X11)
#include <xcb/xcb.h>
#endif

namespace Nz
{
	#if defined(NAZARA_PLATFORM_WINDOWS)
	// http://msdn.microsoft.com/en-us/library/aa383751(v=vs.85).aspx
	typedef void* WindowHandle;
	#elif defined(NAZARA_PLATFORM_X11)
	// http://en.wikipedia.org/wiki/Xlib#Data_types
	using WindowHandle = xcb_window_t;
	#else
		#error Lack of implementation: WindowHandle
	#endif
}

#endif // NAZARA_WINDOWHANDLE_HPP
