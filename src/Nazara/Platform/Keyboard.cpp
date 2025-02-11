// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/SDL3/InputImpl.hpp>

namespace Nz
{
	std::string_view Keyboard::GetKeyName(Scancode scancode)
	{
		return InputImpl::GetKeyName(scancode);
	}

	std::string_view Keyboard::GetKeyName(VKey key)
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

	Keyboard::Scancode Keyboard::ToScanCode(VKey key)
	{
		return InputImpl::ToScanCode(key);
	}

	Keyboard::VKey Keyboard::ToVirtualKey(Scancode scancode)
	{
		return InputImpl::ToVirtualKey(scancode);
	}
}
