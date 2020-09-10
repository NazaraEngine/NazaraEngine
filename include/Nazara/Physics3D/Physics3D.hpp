// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_HPP
#define NAZARA_PHYSICS3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Physics3D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3D : public Module<Physics3D>
	{
		friend Module;

		public:
			using Dependencies = TypeList<Core>;

			Physics3D();
			~Physics3D();

			unsigned int GetMemoryUsed();

		private:
			static Physics3D* s_instance;
	};
}

#endif // NAZARA_PHYSICS3D_HPP
