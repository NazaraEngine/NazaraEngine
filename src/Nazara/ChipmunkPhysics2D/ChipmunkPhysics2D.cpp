// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysics2D.hpp>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	ChipmunkPhysics2D::ChipmunkPhysics2D(Config /*config*/) :
	ModuleBase("ChipmunkPhysics2D", this)
	{
	}

	ChipmunkPhysics2D* ChipmunkPhysics2D::s_instance = nullptr;
}
