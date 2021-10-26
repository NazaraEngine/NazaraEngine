// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_HPP
#define NAZARA_PHYSICS3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Physics3D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3D : public ModuleBase<Physics3D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Physics3D(Config /*config*/);
			~Physics3D() = default;

			unsigned int GetMemoryUsed();

		private:
			static Physics3D* s_instance;
	};
}

#endif // NAZARA_PHYSICS3D_HPP
