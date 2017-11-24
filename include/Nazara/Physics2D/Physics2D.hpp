// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_HPP
#define NAZARA_PHYSICS2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Physics2D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS2D_API Physics2D
	{
		public:
			Physics2D() = delete;
			~Physics2D() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_PHYSICS2D_HPP
