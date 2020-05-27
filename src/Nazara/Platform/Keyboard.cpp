// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/SDL2/InputImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	String Keyboard::GetKeyName(Scancode scancode)
	{
		return EventImpl::GetKeyName(scancode);
	}

	String Keyboard::GetKeyName(VKey key)
	{
		return EventImpl::GetKeyName(key);
	}

	bool Keyboard::IsKeyPressed(Scancode scancode)
	{
		return EventImpl::IsKeyPressed(scancode);
	}

	bool Keyboard::IsKeyPressed(VKey key)
	{
		return EventImpl::IsKeyPressed(key);
	}

	void Keyboard::StartTextInput()
	{
		EventImpl::StartTextInput();
	}

	void Keyboard::StopTextInput()
	{
		EventImpl::StopTextInput();
	}

	Keyboard::Scancode Keyboard::ToScanCode(VKey key)
	{
		return EventImpl::ToScanCode(key);
	}

	Keyboard::VKey Keyboard::ToVirtualKey(Scancode scancode)
	{
		return EventImpl::ToVirtualKey(scancode);
	}
}
