// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS_HPP
#define NAZARA_PHYSICS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Physics/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS_API Physics
	{
		public:
			Physics() = delete;
			~Physics() = delete;

			static unsigned int GetMemoryUsed();

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_PHYSICS_HPP
