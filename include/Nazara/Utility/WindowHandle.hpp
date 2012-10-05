// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWHANDLE_HPP
#define NAZARA_WINDOWHANDLE_HPP

#include <Nazara/Prerequesites.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
// http://msdn.microsoft.com/en-us/library/aa383751(v=vs.85).aspx
typedef void* NzWindowHandle;
#elif defined(NAZARA_PLATFORM_LINUX)
// http://en.wikipedia.org/wiki/Xlib#Data_types
typedef unsigned long NzWindowHandle;
#else
	#error Lack of implementation: WindowHandle
#endif

#endif // NAZARA_WINDOWHANDLE_HPP
