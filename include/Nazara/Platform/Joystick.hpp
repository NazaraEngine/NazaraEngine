// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_JOYSTICK_HPP
#define NAZARA_PLATFORM_JOYSTICK_HPP

#include <Nazara/Prerequisites.hpp>
#include <string>

namespace Nz
{
	class NAZARA_PLATFORM_API Joystick
	{
		public:
			Joystick() = delete;
			~Joystick() = delete;

			static unsigned int GetMaxJoystickCount();

			static void Update();
	};
}

#endif // NAZARA_PLATFORM_JOYSTICK_HPP
