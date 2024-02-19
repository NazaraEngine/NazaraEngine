// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Interface inspired by the SFML of Laurent Gomila (and its team)

#pragma once

#ifndef NAZARA_PLATFORM_MOUSE_HPP
#define NAZARA_PLATFORM_MOUSE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/Export.hpp>

namespace Nz
{
	class Window;

	class NAZARA_PLATFORM_API Mouse
	{
		public:
			enum Button
			{
				Left,
				Middle,
				Right,
				XButton1,
				XButton2,

				Max = XButton2
			};

			Mouse() = delete;
			~Mouse() = delete;

			static Vector2i GetPosition();
			static Vector2i GetPosition(const Window& relativeTo);
			static bool IsButtonPressed(Button button);
			static bool SetRelativeMouseMode(bool relativeMouseMode);
			static void SetPosition(const Vector2i& position);
			static void SetPosition(const Vector2i& position, const Window& relativeTo, bool ignoreEvent = true);
			static void SetPosition(int x, int y);
			static void SetPosition(int x, int y, const Window& relativeTo, bool ignoreEvent = true);

			static constexpr std::size_t ButtonCount = static_cast<std::size_t>(Max + 1);
	};
}

#endif // NAZARA_PLATFORM_MOUSE_HPP
