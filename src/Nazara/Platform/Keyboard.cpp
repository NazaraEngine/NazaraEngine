// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Keyboard.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Platform/Win32/InputImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Platform/X11/InputImpl.hpp>
#else
	#error Lack of implementation: Keyboard
#endif

#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	String Keyboard::GetKeyName(Key key)
	{
		return EventImpl::GetKeyName(key);
	}

	bool Keyboard::IsKeyPressed(Key key)
	{
		return EventImpl::IsKeyPressed(key);
	}
}
