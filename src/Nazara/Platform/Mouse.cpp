// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Platform/SDL2/InputImpl.hpp>

namespace Nz
{
	Vector2i Mouse::GetPosition()
	{
		return InputImpl::GetMousePosition();
	}

	Vector2i Mouse::GetPosition(const Window& relativeTo)
	{
		return InputImpl::GetMousePosition(relativeTo);
	}

	bool Mouse::IsButtonPressed(Button button)
	{
		return InputImpl::IsMouseButtonPressed(button);
	}

	bool Mouse::SetRelativeMouseMode(bool relativeMouseMode)
	{
		return InputImpl::SetRelativeMouseMode(relativeMouseMode);
	}

	void Mouse::SetPosition(const Vector2i& position)
	{
		InputImpl::SetMousePosition(position.x, position.y);
	}

	void Mouse::SetPosition(const Vector2i& position, const Window& relativeTo, bool ignoreEvent)
	{
		if (ignoreEvent && position.x > 0 && position.y > 0)
			relativeTo.IgnoreNextMouseEvent(position.x, position.y);

		InputImpl::SetMousePosition(position.x, position.y, relativeTo);
	}

	void Mouse::SetPosition(int x, int y)
	{
		InputImpl::SetMousePosition(x, y);
	}

	void Mouse::SetPosition(int x, int y, const Window& relativeTo, bool ignoreEvent)
	{
		if (ignoreEvent && x > 0 && y > 0)
			relativeTo.IgnoreNextMouseEvent(x, y);

		InputImpl::SetMousePosition(x, y, relativeTo);
	}
}
