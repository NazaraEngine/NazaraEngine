// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_HPP
#define NAZARA_PHYSICS3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Physics3D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3D
	{
		public:
			Physics3D() = delete;
			~Physics3D() = delete;

			static unsigned int GetMemoryUsed();

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_PHYSICS3D_HPP
