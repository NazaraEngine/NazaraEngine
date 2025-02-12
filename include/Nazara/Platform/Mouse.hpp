// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

			static Vector2f GetPosition();
			static Vector2f GetPosition(const Window& relativeTo);
			static bool IsButtonPressed(Button button);
			static void SetPosition(const Vector2f& position);
			static void SetPosition(const Vector2f& position, const Window& relativeTo, bool ignoreEvent = true);
			static void SetPosition(float x, float y);
			static void SetPosition(float x, float y, const Window& relativeTo, bool ignoreEvent = true);

			static constexpr std::size_t ButtonCount = static_cast<std::size_t>(Max + 1);
	};
}

#endif // NAZARA_PLATFORM_MOUSE_HPP
