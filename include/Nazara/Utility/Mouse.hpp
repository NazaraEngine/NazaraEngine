// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspirée de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_MOUSE_HPP
#define NAZARA_MOUSE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class Window;

	class NAZARA_UTILITY_API Mouse
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
			static void SetPosition(const Vector2i& position);
			static void SetPosition(const Vector2i& position, const Window& relativeTo, bool ignoreEvent = true);
			static void SetPosition(int x, int y);
			static void SetPosition(int x, int y, const Window& relativeTo, bool ignoreEvent = true);
	};
}

#endif // NAZARA_MOUSE_HPP
