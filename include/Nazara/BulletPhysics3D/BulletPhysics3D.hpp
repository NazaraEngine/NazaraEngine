// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_HPP
#define NAZARA_BULLETPHYSICS3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Core/Core.hpp>

namespace Nz
{
	class NAZARA_BULLETPHYSICS3D_API BulletPhysics3D : public ModuleBase<BulletPhysics3D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			BulletPhysics3D(Config /*config*/);
			~BulletPhysics3D() = default;

		private:
			static BulletPhysics3D* s_instance;
	};
}

#endif // NAZARA_BULLETPHYSICS3D_HPP
