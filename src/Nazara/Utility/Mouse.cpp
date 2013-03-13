// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mouse.hpp>
#include <Nazara/Utility/Window.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/InputImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Utility/Linux/InputImpl.hpp>
#else
	#error Lack of implementation: Mouse
#endif

#include <Nazara/Utility/Debug.hpp>

NzVector2i NzMouse::GetPosition()
{
	return NzEventImpl::GetMousePosition();
}

NzVector2i NzMouse::GetPosition(const NzWindow& relativeTo)
{
	return NzEventImpl::GetMousePosition(relativeTo);
}

bool NzMouse::IsButtonPressed(Button button)
{
	return NzEventImpl::IsMouseButtonPressed(button);
}

void NzMouse::SetPosition(const NzVector2i& position)
{
	NzEventImpl::SetMousePosition(position.x, position.y);
}

void NzMouse::SetPosition(const NzVector2i& position, const NzWindow& relativeTo, bool ignoreEvent)
{
	if (ignoreEvent && position.x > 0 && position.y > 0)
		relativeTo.IgnoreNextMouseEvent(position.x, position.y);

	NzEventImpl::SetMousePosition(position.x, position.y, relativeTo);
}

void NzMouse::SetPosition(int x, int y)
{
	NzEventImpl::SetMousePosition(x, y);
}

void NzMouse::SetPosition(int x, int y, const NzWindow& relativeTo, bool ignoreEvent)
{
	if (ignoreEvent && x > 0 && y > 0)
		relativeTo.IgnoreNextMouseEvent(x, y);

	NzEventImpl::SetMousePosition(x, y, relativeTo);
}
