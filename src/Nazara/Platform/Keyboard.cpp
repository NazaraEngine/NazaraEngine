// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/SDL2/InputImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	std::string Keyboard::GetKeyName(Scancode scancode)
	{
		return InputImpl::GetKeyName(scancode);
	}

	std::string Keyboard::GetKeyName(VKey key)
	{
		return InputImpl::GetKeyName(key);
	}

	bool Keyboard::IsKeyPressed(Scancode scancode)
	{
		return InputImpl::IsKeyPressed(scancode);
	}

	bool Keyboard::IsKeyPressed(VKey key)
	{
		return InputImpl::IsKeyPressed(key);
	}

	void Keyboard::StartTextInput()
	{
		InputImpl::StartTextInput();
	}

	void Keyboard::StopTextInput()
	{
		InputImpl::StopTextInput();
	}

	Keyboard::Scancode Keyboard::ToScanCode(VKey key)
	{
		return InputImpl::ToScanCode(key);
	}

	Keyboard::VKey Keyboard::ToVirtualKey(Scancode scancode)
	{
		return InputImpl::ToVirtualKey(scancode);
	}
}
