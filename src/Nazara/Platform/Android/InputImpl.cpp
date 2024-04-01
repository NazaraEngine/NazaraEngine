// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Android/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/Window.hpp>
#include <NazaraUtils/CallOnExit.hpp>

namespace Nz
{
	ClipboardContentType InputImpl::GetClipboardContentType()
	{
		return ClipboardContentType::Unknown;
	}

	std::string InputImpl::GetClipboardString()
	{
		return {};
	}

	std::string InputImpl::GetKeyName(Keyboard::Scancode key)
	{
		return "Unknown";
	}

	std::string InputImpl::GetKeyName(Keyboard::VKey key)
	{
		return "Unknown";
	}

	Vector2i InputImpl::GetMousePosition()
	{
		return { 0, 0 };
	}

	Vector2i InputImpl::GetMousePosition(const Window& relativeTo)
	{
		return { 0, 0 };
	}

	bool InputImpl::IsKeyPressed(Keyboard::Scancode key)
	{
		return false;
	}

	bool InputImpl::IsKeyPressed(Keyboard::VKey key)
	{
		return false;
	}

	bool InputImpl::IsMouseButtonPressed(Mouse::Button button)
	{
		return false;
	}

	void InputImpl::SetClipboardString(const std::string& str)
	{
	}

	void InputImpl::SetMousePosition(int x, int y)
	{
	}

	void InputImpl::SetMousePosition(int x, int y, const Window& relativeTo)
	{
	}

	void InputImpl::StartTextInput()
	{
	}

	void InputImpl::StopTextInput()
	{
	}

	Keyboard::Scancode InputImpl::ToScanCode(Keyboard::VKey key)
	{
		return Keyboard::Scancode::Undefined;
	}

	Keyboard::VKey InputImpl::ToVirtualKey(Keyboard::Scancode scancode)
	{
		return Keyboard::VKey::Undefined;
	}
}
