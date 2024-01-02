// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/BulletPhysics3D.hpp>
#include <Nazara/BulletPhysics3D/BulletCollider3D.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	BulletPhysics3D::BulletPhysics3D(Config /*config*/) :
	ModuleBase("BulletPhysics3D", this)
	{
	}

	BulletPhysics3D* BulletPhysics3D::s_instance;
}
