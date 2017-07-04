// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Keyboard.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/InputImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Utility/X11/InputImpl.hpp>
#else
	#error Lack of implementation: Keyboard
#endif

#include <Nazara/Utility/Debug.hpp>

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
